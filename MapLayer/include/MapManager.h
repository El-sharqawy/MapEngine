#pragma once

#include "MapCamera.h"
#include "TileGrid.h"
#include "TileGridRenderer.h"

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

    CMapCamera& GetCamera() { return m_mapCamera; }
    CTileGrid& GetTileGrid() { return m_tileGrid; }

private:
    CTileGrid          m_tileGrid;
    CTileGridRenderer  m_tileGridRenderer;
    CMapCamera         m_mapCamera;

    int32_t m_iScreenW = 0;
    int32_t m_iScreenH = 0;

    bool m_bInitialized = false;
};
