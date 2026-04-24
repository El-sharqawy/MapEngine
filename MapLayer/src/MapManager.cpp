#include "MapManager.h"
#include <nlohmann/json.hpp>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include "../../LibImageUI/Stdafx.h"

bool CMapManager::Initialize(int iScreenW, int iScreenH)
{
    m_iScreenW = iScreenW;
    m_iScreenH = iScreenH;

    if (!m_tileGridRenderer.Initialize())
        return false;

    if (!m_polyLineRenderer.Initialize())
        return false;

    if (!m_polyRenderer.Initialize())
        return false;

    // Default center: Cairo
    m_mapCamera.SetCenter(30.0444, 31.2357);
    m_mapCamera.SetZoom(12);

    // Fetch roads for initial view
    FetchVisibleRoads();
    FetchVisibleBuildings();

    m_bInitialized = true;

    syslog("Initialized MapManager ({}, {})", iScreenW, iScreenH);
    return true;
}

void CMapManager::Update(float fDeltaTime, int iScreenW, int iScreenH)
{
    m_iScreenW = iScreenW;
    m_iScreenH = iScreenH;

    Vector2D cameraOrigin = m_mapCamera.GetOriginWorldPixel(iScreenW, iScreenH);
    m_tileGrid.Update(cameraOrigin, iScreenW, iScreenH, m_mapCamera.GetZoom());
    m_mapCamera.Update(fDeltaTime);

    // Debounced road fetch — fires 1s after camera stops moving
    if (m_bNeedsRoadFetch)
    {
        m_fRoadFetchCooldown -= fDeltaTime;
        if (m_fRoadFetchCooldown <= 0.0f)
        {
            m_bNeedsRoadFetch = false;
            FetchVisibleRoads();
        }
    }

    if (m_bRoadsReady)
    {
        std::lock_guard<std::mutex> lock(m_roadsMutex);
        m_roads = std::move(m_pendingRoads);
        m_bRoadsReady = false;
        m_bRoadsDirty = true;
        syslog("Roads handed to main thread: {} roads", m_roads.size());
    }

    // Fetch Buildings
    if (m_bNeedsBuildingsFetch)
    {
        m_fBulidingsFetchCooldown -= fDeltaTime;
        if (m_fBulidingsFetchCooldown <= 0.0f)
        {
            m_bNeedsBuildingsFetch = false;
            FetchVisibleBuildings();
        }
    }

    if (m_bBuildingReady)
    {
        std::lock_guard<std::mutex> lock(m_buildingsMutex);
        m_vBuildings = std::move(m_vPendingBuildings);
        m_bBuildingReady = false;
        m_bBuildingsDirty = true;
        syslog("Buildings handed to main thread: {} buildings", m_vBuildings.size());
    }

}

void CMapManager::Render(int iScreenW, int iScreenH)
{
    if (!m_bInitialized)
    {
        return;
    }

    if (m_bRoadsDirty && !m_roads.empty())
    {
        m_bRoadsDirty = false;
        m_polyLineRenderer.UploadRoads(m_roads, m_mapCamera);
    }

    if (m_bBuildingsDirty && !m_vBuildings.empty())
    {
        m_bBuildingsDirty = false;
        m_polyRenderer.UploadBuildings(m_vBuildings, m_iScreenW, m_iScreenH, m_mapCamera);
    }

    m_tileGridRenderer.Render(m_tileGrid, m_mapCamera, iScreenW, iScreenH);
    m_polyRenderer.Render(m_iScreenW, m_iScreenH, m_mapCamera);
    m_polyLineRenderer.Render(m_iScreenW, m_iScreenH, m_mapCamera); // roads on top of the tiles
}

void CMapManager::Destroy()
{
    // Clear GPU Resources
    m_tileGridRenderer.Destroy();
    m_polyRenderer.Destroy();
    m_polyLineRenderer.Destroy();
}

// Input
void CMapManager::OnMouseClick(float fMouseX, float fMouseY)
{
    if (!m_bPickingPoint) // Picking is not active
    {
        return;
    }

    Vector2D origin = m_mapCamera.GetOriginWorldPixel(m_iScreenW, m_iScreenH);
    Vector2D worldPx(origin.x + fMouseX, origin.y + fMouseY);
    Vector2D latLng = Anubis::PixelToLatLng(worldPx.x, worldPx.y, m_mapCamera.GetZoom());

    m_vPickedPoints[m_iPickedCount++] = latLng;

    if (m_iPickedCount >= 2)
    {
        m_fDistance = Anubis::HaversineDistanceKm(
            m_vPickedPoints[0].x, m_vPickedPoints[0].y,
            m_vPickedPoints[1].x, m_vPickedPoints[1].y);

        syslog("Distance: {:.2f} km", m_fDistance);
        m_iPickedCount = 0;
        m_bPickingPoint = false;
    }
}

void CMapManager::OnMouseDrag(float fDx, float fDy)
{
    m_mapCamera.Pan(fDx, fDy, m_iScreenW, m_iScreenH);
    m_bNeedsRoadFetch = true;
    m_fRoadFetchCooldown = 2.0f; // wait 1 second after last interaction

    // Buildings
    m_bNeedsBuildingsFetch = true;
    m_fBulidingsFetchCooldown = 2.0f;
}

void CMapManager::OnMouseScroll(float fDelta, float fMouseX, float fMouseY)
{
    int32_t iOldZoom = m_mapCamera.GetZoom();
    m_mapCamera.Zoom(fDelta, fMouseX, fMouseY, m_iScreenW, m_iScreenH);
    int32_t iNewZoom = m_mapCamera.GetZoom();

    if (iNewZoom != iOldZoom)
    {
        // Zoom changed — existing world pixels are wrong, must re-upload
        if (!m_roads.empty())
            m_bRoadsDirty = true;
        if (!m_vBuildings.empty())
            m_bBuildingsDirty = true;
    }

    m_bNeedsRoadFetch = true;
    m_fRoadFetchCooldown = 2.0f; // wait 1 second after last interaction

    // Buildings
    m_bNeedsBuildingsFetch = true;
    m_fBulidingsFetchCooldown = 2.0f;
}

bool CMapManager::PostOverpassQuery(const std::string& query, std::string& outBody, const std::string& queryType)
{
    // Global rate limit across all request types
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastOverpassRequest).count();

    if (elapsed < 1500)
        std::this_thread::sleep_for(std::chrono::milliseconds(1500 - elapsed));

    m_lastOverpassRequest = std::chrono::steady_clock::now();

    httplib::Client cli("http://overpass-api.de");
    cli.set_connection_timeout(5);
    cli.set_read_timeout(60);   // was 25, increase to 60
    cli.set_write_timeout(10);  // add write timeout for large POST body
    cli.set_default_headers({ {"User-Agent", "AnubisMapEngine/1.0"} });

    std::string body = "data=" + Anubis::UrlEncode(query);
    auto res = cli.Post("/api/interpreter", body, "application/x-www-form-urlencoded");

    if (!res)
    {
        // Log the actual error reason
        syserr("Overpass connection error: {} - type: {}", httplib::to_string(res.error()), queryType.c_str());
        return false;
    }
    if (res->status != 200)
    {
        syserr("Overpass failed: HTTP {} - type: {}", res->status, queryType.c_str());
        return false;
    }
    outBody = res->body;
    return true;
}

void CMapManager::FetchRoads(double minLat, double minLng, double maxLat, double maxLng)
{
    std::string query = Anubis::BuildRoadsQuery(minLat, minLng, maxLat, maxLng, m_mapCamera.GetZoom());

    std::thread([=]()
        {
            if (m_bFetchRoadsInProgress)
            {
                syslog("Fetch already in progress, skipping");
                return;
            }

            m_bFetchRoadsInProgress = true;

            std::string body;
            if (!PostOverpassQuery(query, body, "roads"))
            {
                m_bFetchRoadsInProgress = false;
                return;
            }

            ParseAndStoreRoads(body);
            m_bFetchRoadsInProgress = false;
        }).detach();
}

void CMapManager::ParseAndStoreRoads(const std::string& sJson)
{
    auto j = nlohmann::json::parse(sJson, nullptr, false);
    if (j.is_discarded())
    {
        syserr("parsing JSON data failed, is discarded");
        return;
    }

    std::vector<STileRoadData> roads;

    for (auto& element : j["elements"])
    {
        if (element["type"] != "way") continue;
        if (!element.contains("geometry")) continue;

        STileRoadData road;
        road.sHighwayType = element["tags"].value("highway", "unknown");

        for (auto& node : element["geometry"])
        {
            double lat = node["lat"];
            double lon = node["lon"];
            road.vLatLngPoints.emplace_back((float)lat, (float)lon);
        }

        if (road.vLatLngPoints.size() >= 2)
            roads.emplace_back(std::move(road));
    }

    // Push to main thread safely
    std::lock_guard<std::mutex> lock(m_roadsMutex);
    m_pendingRoads = std::move(roads);
    m_bRoadsReady = true;
}

void CMapManager::FetchVisibleRoads()
{
    int iZoom = m_mapCamera.GetZoom();
    if (iZoom < 12)
    {
        syslog("Zoom too low for road fetch {}, skipping", iZoom);
        return;
    }

    Vector2D origin = m_mapCamera.GetOriginWorldPixel(m_iScreenW, m_iScreenH);

    Vector2D topLeft = Anubis::PixelToLatLng(origin.x, origin.y, m_mapCamera.GetZoom());
    Vector2D bottomRight = Anubis::PixelToLatLng(
        origin.x + m_iScreenW,
        origin.y + m_iScreenH,
        m_mapCamera.GetZoom());

    syslog("Fetching roads bbox: minLat={} minLng={} maxLat={} maxLng={} with Zoom={}",
        bottomRight.x, topLeft.y, topLeft.x, bottomRight.y, iZoom);

    // topLeft.x = maxLat, bottomRight.x = minLat (Y flips in Mercator)
    float pad = (iZoom >= 15) ? 0.015f : 0.008f;  // was 0.008/0.003
    FetchRoads(bottomRight.x - pad, topLeft.y - pad, topLeft.x + pad, bottomRight.y + pad);
}

void CMapManager::FetchVisibleBuildings()
{
    int iZoom = m_mapCamera.GetZoom();
    if (iZoom < 15)
    {
        syslog("Zoom too low for building fetch {}, skipping", iZoom);
        return;
    }

    double dDelta = 0.015;
    Vector2D center = m_mapCamera.GetCenterLatLng(m_iScreenW, m_iScreenH);

    double minLat = center.x - dDelta, maxLat = center.x + dDelta;
    double minLng = center.y - dDelta, maxLng = center.y + dDelta;

    std::string query = Anubis::BuildBuildingsQuery(minLat, minLng, maxLat, maxLng);

    syslog("Fetching buildings bbox: minLat={} minLng={} maxLat={} maxLng={} with Zoom={}",
        minLat, minLng, maxLat, maxLng, iZoom);

    std::thread([this, query]()
        {
            if (m_bFetchBuildingsInProgress)
            {
                syslog("Fetching buildings already in progress, skipping");
                return;
            }

            m_bFetchBuildingsInProgress = true;

            std::string body;
            if (!PostOverpassQuery(query, body, "buildings"))
            {
                m_bFetchBuildingsInProgress = false;
                return;
            }

            auto parsed = ParseBuildingsResponse(body);

            std::lock_guard<std::mutex> lock(m_buildingsMutex);
            m_vPendingBuildings = std::move(parsed);
            m_bBuildingReady = true;          // correct flag
            m_bFetchBuildingsInProgress = false;
        }).detach();
}

std::vector<STileBuildingData> CMapManager::ParseBuildingsResponse(const std::string& json)
{
    std::vector<STileBuildingData> result;
    nlohmann::json doc = nlohmann::json::parse(json, nullptr, false);
    if (doc.is_discarded())
    {
        return result;
    }

    for (auto& el : doc["elements"])
    {
        if (el["type"] != "way")
            continue;
        if (!el.contains("geometry"))
            continue;

        STileBuildingData building;
        building.sBuildingType = el.value("tags", nlohmann::json{}).value("building", "yes");

        for (auto& pt : el["geometry"])
        {
            double lat = pt["lat"];
            double lng = pt["lon"];
            building.vLatLngPoints.push_back({ lat, lng });  // pure double, no cast
        }

        if (building.vLatLngPoints.size() >= 3)
        {
            result.push_back(std::move(building));
        }
    }

    syslog("ParseBuildingsResponse: result: {} buildlings", result.size());
    return result;
}

void CMapManager::RenderCompass()
{
    // Position: bottom-right corner
    float cx = (float)m_iScreenW - 60.0f;
    float cy = (float)m_iScreenH - 60.0f;
    float r = 40.0f;

    float fBearing = 0.0f; // camera.GetBearing(); // 0 = north up, rotate with camera

    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    // Outer circle
    draw->AddCircle({ cx, cy }, r, IM_COL32(255, 255, 255, 180), 64, 2.0f);

    // Inner fill
    draw->AddCircleFilled({ cx, cy }, r - 2.0f, IM_COL32(20, 20, 20, 160), 64);

    // North needle (red)
    float northAngle = fBearing * (M_PI / 180.0f); // bearing in radians
    ImVec2 northTip = {
        cx + std::sin(northAngle) * (r - 6.0f),
        cy - std::cos(northAngle) * (r - 6.0f)
    };
    ImVec2 southTip = {
        cx - std::sin(northAngle) * (r - 6.0f),
        cy + std::cos(northAngle) * (r - 6.0f)
    };
    ImVec2 center = { cx, cy };

    // Red = north half
    draw->AddTriangleFilled(northTip,
        { cx + std::cos(northAngle) * 6.0f, cy + std::sin(northAngle) * 6.0f },
        { cx - std::cos(northAngle) * 6.0f, cy - std::sin(northAngle) * 6.0f },
        IM_COL32(220, 50, 50, 255));

    // White = south half
    draw->AddTriangleFilled(southTip,
        { cx + std::cos(northAngle) * 6.0f, cy + std::sin(northAngle) * 6.0f },
        { cx - std::cos(northAngle) * 6.0f, cy - std::sin(northAngle) * 6.0f },
        IM_COL32(240, 240, 240, 255));

    // Center dot
    draw->AddCircleFilled({ cx, cy }, 4.0f, IM_COL32(255, 255, 255, 255));

    // N label
    ImVec2 nPos = {
        cx + std::sin(northAngle) * (r + 10.0f) - 4.0f,
        cy - std::cos(northAngle) * (r + 10.0f) - 6.0f
    };
    draw->AddText(nPos, IM_COL32(255, 255, 255, 255), "N");
}
