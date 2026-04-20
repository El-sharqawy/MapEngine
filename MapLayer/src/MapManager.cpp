#include "MapManager.h"
#include <nlohmann/json.hpp>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

bool CMapManager::Initialize(int iScreenW, int iScreenH)
{
    m_iScreenW = iScreenW;
    m_iScreenH = iScreenH;

    if (!m_tileGridRenderer.Initialize())
        return false;

    if (!m_polyLineRenderer.Initialize())
        return false;

    // Default center: Cairo
    m_mapCamera.SetCenter(30.0444, 31.2357);
    m_mapCamera.SetZoom(12);

    // Fetch roads for initial view
    FetchVisibleRoads();

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
}

void CMapManager::Render(int iScreenW, int iScreenH)
{
    if (!m_bInitialized)
    {
        return;
    }

    if (m_bRoadsDirty && !m_roads.empty())
    {
        m_polyLineRenderer.UploadRoads(m_roads, m_mapCamera, m_iScreenW, m_iScreenH);
        m_bRoadsDirty = false;
    }

    m_tileGridRenderer.Render(m_tileGrid, m_mapCamera, iScreenW, iScreenH);
    m_polyLineRenderer.Render(m_iScreenW, m_iScreenH); // roads on top of the tiles
}

void CMapManager::Destroy()
{
    // do nothing?
}

// Input
void CMapManager::OnMouseDrag(float fDx, float fDy)
{
    m_mapCamera.Pan(fDx, fDy, m_iScreenW, m_iScreenH);
    m_bRoadsDirty = true;
    m_bNeedsRoadFetch = true;
    m_fRoadFetchCooldown = 2.0f; // wait 1 second after last interaction
}

void CMapManager::OnMouseScroll(float fDelta, float fMouseX, float fMouseY)
{
    m_mapCamera.Zoom(fDelta, fMouseX, fMouseY, m_iScreenW, m_iScreenH);
    m_bRoadsDirty = true;
    m_bNeedsRoadFetch = true;
    m_fRoadFetchCooldown = 2.0f; // wait 1 second after last interaction
}

void CMapManager::FetchRoads(double minLat, double minLng, double maxLat, double maxLng)
{
    std::thread([=]()
        {
            if (m_bFetchInProgress)
            {
                syslog("Fetch already in progress, skipping");
                return;
            }

            m_bFetchInProgress = true;

            // ← Use plain HTTP to isolate SSL issues
            httplib::Client cli("http://overpass-api.de");
            cli.set_connection_timeout(5);
            cli.set_read_timeout(15);
            cli.set_write_timeout(5);
            cli.set_default_headers({ {"User-Agent", "AnubisMapEngine/1.0"} });

            std::string query = Anubis::BuildRoadsQuery(minLat, minLng, maxLat, maxLng);
            std::string body = "data=" + Anubis::UrlEncode(query);
            auto res = cli.Post("/api/interpreter", body, "application/x-www-form-urlencoded");

            if (!res)
            {
                syserr("Overpass fetch failed — network error: {}", (int)res.error());
                m_bFetchInProgress = false;
                return;
            }
            if (res->status != 200)
            {
                syserr("Overpass HTTP {} — body: {}", res->status, res->body.substr(0, 200));
                m_bFetchInProgress = false;
                return;
            }

            ParseAndStoreRoads(res->body);
            m_bFetchInProgress = false;
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
    if (m_mapCamera.GetZoom() < 10)
    {
        syslog("Zoom too low for road fetch, skipping");
        return;
    }

    Vector2D origin = m_mapCamera.GetOriginWorldPixel(m_iScreenW, m_iScreenH);

    Vector2D topLeft = Anubis::PixelToLatLng(origin.x, origin.y, m_mapCamera.GetZoom());
    Vector2D bottomRight = Anubis::PixelToLatLng(
        origin.x + m_iScreenW,
        origin.y + m_iScreenH,
        m_mapCamera.GetZoom());

    syslog("Fetching roads bbox: minLat={} minLng={} maxLat={} maxLng={}",
        bottomRight.x, topLeft.y, topLeft.x, bottomRight.y);

    // topLeft.x = maxLat, bottomRight.x = minLat (Y flips in Mercator)
    FetchRoads(bottomRight.x, topLeft.y, topLeft.x, bottomRight.y);
}
