#include "MapManager.h"

bool CMapManager::Initialize(int iScreenW, int iScreenH)
{
    m_iScreenW = iScreenW;
    m_iScreenH = iScreenH;

    if (!m_tileGridRenderer.Initialize())
        return false;

    // Default center: Cairo
    m_mapCamera.SetCenter(30.0444, 31.2357);
    m_mapCamera.SetZoom(12);

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
}

void CMapManager::Render(int iScreenW, int iScreenH)
{
    if (!m_bInitialized)
    {
        return;
    }
    m_tileGridRenderer.Render(m_tileGrid, m_mapCamera, iScreenW, iScreenH);
}

void CMapManager::Destroy()
{
    // do nothing?
}

// Input
void CMapManager::OnMouseDrag(float fDx, float fDy)
{
    m_mapCamera.Pan(fDx, fDy, m_iScreenW, m_iScreenH);
}

void CMapManager::OnMouseScroll(float fDelta, float fMouseX, float fMouseY)
{
    m_mapCamera.Zoom(fDelta, fMouseX, fMouseY, m_iScreenW, m_iScreenH);
}
