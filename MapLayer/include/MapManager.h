#pragma once

#include "MapCamera.h"
#include "TileGrid.h"
#include "TileGridRenderer.h"
#include "PolyLineRenderer.h"
#include "PolyTriangulationRenderer.h"

#include <mutex>
#include <atomic>

constexpr float MIN_REQUEST_INTERVAL_MS = 1500.0f; // 1.5s between any request

class CMapManager : public CSingleton<CMapManager>
{
public:
    bool Initialize(int iScreenW, int iScreenH);
    void Update(float fDeltaTime, int iScreenW, int iScreenH);
    void Render(int iScreenW, int iScreenH);
    void Destroy();

    // Input
    void OnMouseClick(float fMouseX, float fMouseY);
    void OnMouseDrag(float fDx, float fDy);
    void OnMouseScroll(float fDelta, float fMouseX, float fMouseY);

    bool PostOverpassQuery(const std::string& query, std::string& outBody, const std::string& queryType);

    void FetchRoads(double minLat, double minLng, double maxLat, double maxLng);
    void ParseAndStoreRoads(const std::string& sJson);
    void FetchVisibleRoads();

    CMapCamera& GetCamera() { return m_mapCamera; }
    CTileGrid& GetTileGrid() { return m_tileGrid; }
    bool IsPicking() const { return m_bPickingPoint; }
    void StartPicking() { m_bPickingPoint = true; }
    void StopPicking() { m_bPickingPoint = false; }

    int32_t GetPickedCount() const { return m_iPickedCount; }
    float GetDistance() const { return m_fDistance; }

    void FetchVisibleBuildings();
    std::vector<STileBuildingData> ParseBuildingsResponse(const std::string& json);

    void RenderCompass();

private:
    CTileGrid          m_tileGrid;
    CTileGridRenderer  m_tileGridRenderer;
    CPolyLineRenderer  m_polyLineRenderer;
    CPolyTriangulationRenderer m_polyRenderer;

    CMapCamera         m_mapCamera;

    int32_t m_iScreenW = 0;
    int32_t m_iScreenH = 0;

    bool m_bInitialized = false;

    std::mutex m_roadsMutex;
    std::vector<STileRoadData> m_pendingRoads;
    std::vector<STileRoadData> m_roads;
    std::atomic<bool> m_bRoadsReady = false;
    std::atomic<bool> m_bRoadsDirty = false;
    float m_fRoadFetchCooldown = 1.0f;
    std::atomic<bool>  m_bNeedsRoadFetch = false;
    std::atomic<bool> m_bFetchRoadsInProgress{ false };

    // Picking
    bool m_bPickingPoint = false;
    int32_t m_iPickedCount = 0;
    Vector2D m_vPickedPoints[2];
    float m_fDistance = 0.0f;

    // Buildings
    std::mutex m_buildingsMutex;
    std::vector<STileBuildingData> m_vPendingBuildings;
    std::vector<STileBuildingData> m_vBuildings;
    std::atomic<bool> m_bBuildingReady{ false };
    std::atomic<bool> m_bBuildingsDirty{ false };
    std::atomic<bool> m_bNeedsBuildingsFetch{ false };
    std::atomic<bool> m_bFetchBuildingsInProgress{ false };
    float m_fBulidingsFetchCooldown = 1.0f;

    // timer
    std::chrono::steady_clock::time_point m_lastOverpassRequest{};
};
