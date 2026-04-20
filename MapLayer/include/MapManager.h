#pragma once

#include "MapCamera.h"
#include "TileGrid.h"
#include "TileGridRenderer.h"
#include "PolyLineRenderer.h"

#include <mutex>
#include <atomic>

class CMapManager : public CSingleton<CMapManager>
{
public:
    bool Initialize(int iScreenW, int iScreenH);
    void Update(float fDeltaTime, int iScreenW, int iScreenH);
    void Render(int iScreenW, int iScreenH);
    void Destroy();

    // Input
    void OnMouseDrag(float fDx, float fDy);
    void OnMouseScroll(float fDelta, float fMouseX, float fMouseY);

    void FetchRoads(double minLat, double minLng, double maxLat, double maxLng);
    void ParseAndStoreRoads(const std::string& sJson);
    void FetchVisibleRoads();

    CMapCamera& GetCamera() { return m_mapCamera; }
    CTileGrid& GetTileGrid() { return m_tileGrid; }

private:
    CTileGrid          m_tileGrid;
    CTileGridRenderer  m_tileGridRenderer;
    CPolyLineRenderer  m_polyLineRenderer;

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
    std::atomic<bool> m_bFetchInProgress{ false };

};
