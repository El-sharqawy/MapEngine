#pragma once

#include "MapProjection.h"
#include "TileGrid.h"  // for TileKey
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <algorithm>

#undef min
#undef max

class CMapCamera
{
public:
    CMapCamera()
        : m_dCenterLat(30.0444)   // Default: Cairo 30.0444, 31.2357
        , m_dCenterLng(31.2357)
        , m_iZoom(12)
    {
    }

    // ─── View State ──────────────────────────────────────────────────────────

    void SetCenter(double lat, double lng)
    {
        m_dCenterLat = lat;
        m_dCenterLng = lng;
    }

    void SetZoom(int32_t zoom)
    {
        m_iZoom = std::clamp(zoom, 0, 19);
    }

    int32_t    GetZoom()      const { return m_iZoom; }
    double GetCenterLat() const { return m_dCenterLat; }
    double GetCenterLng() const { return m_dCenterLng; }

    // ─── Interaction ─────────────────────────────────────────────────────────

// Mouse drag — dx/dy in screen pixels
    void Pan(float dx, float dy, int screenW, int screenH)
    {
        // How many world pixels does the screen span at this zoom?
        double worldSize = static_cast<double>(Anubis::TileCount(m_iZoom) * TILE_SIZE_PX);
        double pixelsPerLng = worldSize / 360.0;

        // Convert screen pixel delta → lat/lng delta
        Vector2D center = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);
        center.x -= dx;
        center.y -= dy;

        // Clamp to world bounds
        center.x = std::clamp((double)center.x, 0.0, worldSize - 1.0);
        center.y = std::clamp((double)center.y, 0.0, worldSize - 1.0);

        Vector2D newLatLng = Anubis::PixelToLatLng(center.x, center.y, m_iZoom);
        m_dCenterLat = std::clamp((double)newLatLng.x, -85.05112878, 85.05112878);
        m_dCenterLng = std::clamp((double)newLatLng.y, -180.0, 180.0);
    }

    // Mouse scroll — zoom toward cursor position
    void Zoom(float delta, float mouseX, float mouseY, int screenW, int screenH)
    {
        if (delta == 0.0f)
            return;

        m_fTargetZoom = std::clamp(m_fTargetZoom + delta, 0.0f, 19.0f);
        m_fZoomAnchorX = mouseX;
        m_fZoomAnchorY = mouseY;
        m_iScreenW = screenW;
        m_iScreenH = screenH;
    }

    void Update(float fDt)
    {
        float fPrev = m_fSmoothZoom;
        m_fSmoothZoom += (m_fTargetZoom - m_fSmoothZoom) * std::min(fDt * 14.0f, 1.0f);

        if (fabsf(m_fSmoothZoom - fPrev) < 0.0001f)
            return;

        int iNewZoom = (int)floorf(m_fSmoothZoom);
        if (iNewZoom == m_iZoom)
            return;

        // 1. Get center pixel at OLD zoom
        Vector2D oldCenter = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);

        // 2. Mouse position in world pixels at OLD zoom
        float oldMouseWorldX = oldCenter.x - m_iScreenW * 0.5f + m_fZoomAnchorX;
        float oldMouseWorldY = oldCenter.y - m_iScreenH * 0.5f + m_fZoomAnchorY;

        // 3. Scale factor between zoom levels (each level doubles world pixel size)
        float fScale = powf(2.0f, (float)(iNewZoom - m_iZoom));

        // 4. Same mouse point in world pixels at NEW zoom
        float newMouseWorldX = oldMouseWorldX * fScale;
        float newMouseWorldY = oldMouseWorldY * fScale;

        // 5. Change zoom
        m_iZoom = iNewZoom;

        // 6. New center = new mouse world pos minus screen offset to anchor
        float newCenterPixelX = newMouseWorldX - m_fZoomAnchorX + m_iScreenW * 0.5f;
        float newCenterPixelY = newMouseWorldY - m_fZoomAnchorY + m_iScreenH * 0.5f;

        // 7. Convert back to lat/lng
        Vector2D newCenter = Anubis::PixelToLatLng(newCenterPixelX, newCenterPixelY, m_iZoom);
        m_dCenterLat = std::clamp((double)newCenter.x, -85.05112878, 85.05112878);
        m_dCenterLng = std::clamp((double)newCenter.y, -180.0, 180.0);
    }

    // ─── Matrices ────────────────────────────────────────────────────────────

    // Returns the ortho MVP matrix — pass directly as u_mvp to tile shader
    glm::mat4 GetProjectionMatrix(int screenW, int screenH) const
    {
        Vector2D centerPixel = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);

        float halfW = screenW * 0.5f;
        float halfH = screenH * 0.5f;

        // Screen center maps to centerPixel in world space
        float left = static_cast<float>(centerPixel.x) - halfW;
        float right = static_cast<float>(centerPixel.x) + halfW;
        float top = static_cast<float>(centerPixel.y) - halfH;
        float bottom = static_cast<float>(centerPixel.y) + halfH;

        return glm::ortho(left, right, top, bottom, -1.0f, 1.0f);
    }

    // ─── Tile Visibility ─────────────────────────────────────────────────────

    // Returns all tile keys visible in the current view + 1 tile margin
    std::vector<STileIndex> GetVisibleTiles(int screenW, int screenH) const
    {
        Vector2D centerPixel = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);

        double halfW = screenW * 0.5;
        double halfH = screenH * 0.5;

        // World-pixel bounds of screen
        double left = centerPixel.x - halfW;
        double right = centerPixel.x + halfW;
        double top = centerPixel.y - halfH;
        double bottom = centerPixel.y + halfH;

        int tileSize = TILE_SIZE_PX;
        int maxTile = Anubis::TileCount(m_iZoom) - 1;

        int minTileX = std::max(0, static_cast<int>(std::floor(left / tileSize)));
        int maxTileX = std::min(maxTile, static_cast<int>(std::floor(right / tileSize)));
        int minTileY = std::max(0, static_cast<int>(std::floor(top / tileSize)));
        int maxTileY = std::min(maxTile, static_cast<int>(std::floor(bottom / tileSize)));

        std::vector<STileIndex> visible;
        visible.reserve((maxTileX - minTileX + 1) * (maxTileY - minTileY + 1));

        for (int y = minTileY; y <= maxTileY; ++y)
            for (int x = minTileX; x <= maxTileX; ++x)
                visible.push_back(STileIndex{ x, y, m_iZoom });

        return visible;
    }

    // Returns the world-pixel coordinate of the top-left corner of the viewport
    // This is what CTileGrid::Update() needs as v2CameraOrigin
    Vector2D GetOriginWorldPixel(int iScreenW, int iScreenH) const
    {
        // Center pixel in world space
        Vector2D center = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);

        // Top-left = center minus half screen size
        return Vector2D(
            center.x - iScreenW * 0.5f,
            center.y - iScreenH * 0.5f
        );
    }

private:
    double m_dCenterLat;
    double m_dCenterLng;
    int32_t m_iZoom;
    float  m_fTargetZoom = 12.0f;  // what scroll wheel sets
    float  m_fSmoothZoom = 12.0f;  // what actually drives rendering
    float  m_fZoomAnchorX = 0.0f;
    float  m_fZoomAnchorY = 0.0f;
    int32_t    m_iScreenW = 0;
    int32_t    m_iScreenH = 0;

    Vector2D GetCameraOrigin(int iScreenW, int iScreenH) const
    {
        Vector2D centerPixel = Anubis::LatLngToPixel(m_dCenterLat, m_dCenterLng, m_iZoom);
        return Vector2D(
            centerPixel.x - iScreenW * 0.5f,
            centerPixel.y - iScreenH * 0.5f
        );
    }

    // Convert screen pixel position → world pixel position at current zoom
    Vector2D ScreenToWorld(float sx, float sy, int screenW, int screenH) const
    {
        // Camera origin = top-left world pixel of the viewport
        Vector2D origin = GetCameraOrigin(screenW, screenH);
        return Vector2D(origin.x + sx, origin.y + sy);
    }

};