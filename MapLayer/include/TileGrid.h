#pragma once

#include <vector>
#include <map>
#include <set>
#include <queue>
#include <mutex>
#include "TileIndex.h"
#include "Texture.h"

struct STileReadyData
{
    STileIndex  index;
    SImageData  imageData;  // raw pixels, not yet on GPU
};

/**
 * @brief Computes which tiles are visible given a camera view in world-pixel space.
 *
 * Usage:
 *   CTileGrid grid;
 *   grid.Update(cameraOriginPixel, viewportWidthPx, viewportHeightPx, zoom);
 *   for (const STileIndex& tile : grid.GetVisibleTiles()) { ... }
 */
class CTileGrid
{
public:
    CTileGrid() = default;

    /**
     * Recompute the visible tile set.
     *
     * @param v2CameraOrigin  Top-left corner of the viewport in world-pixel space
     * @param fViewportW      Viewport width in pixels
     * @param fViewportH      Viewport height in pixels
     * @param iZoom           Current zoom level
     */
    void Update(Vector2D v2CameraOrigin, float fViewportW, float fViewportH, int32_t iZoom);

    /** All tiles currently visible (recomputed on each Update call) */
    const std::vector<STileIndex>& GetVisibleTiles() const { return m_vVisibleTiles; }

    /** How many tiles are visible */
    int32_t GetTileCount() const { return static_cast<int32_t>(m_vVisibleTiles.size()); }

    /**
     * Convert a world-pixel position to screen-pixel position.
     * Screen origin (0,0) = top-left of viewport.
     *
     * @param v2WorldPixel    Position in world-pixel space
     * @param v2CameraOrigin  Top-left of viewport in world-pixel space
     */
    static Vector2D WorldPixelToScreen(Vector2D v2WorldPixel, Vector2D v2CameraOrigin);

    /**
     * Get the screen-space quad corners for a tile (for VAO upload).
     * Returns top-left corner in screen space — tile is always TILE_SIZE_PX wide/tall.
     */
    Vector2D GetTileScreenOrigin(const STileIndex& tile, Vector2D v2CameraOrigin) const;

    size_t GetVisibleTileCount() const { return m_vVisibleTiles.size(); }

    void RequestTile(const STileIndex& idx);   // trigger async fetch
    CTexture* GetTileTexture(const STileIndex& idx); // null if not loaded yet
    bool IsTileLoaded(const STileIndex& idx) const;

    void UploadPendingTiles();

private:
    std::vector<STileIndex> m_vVisibleTiles;
    std::map<STileIndex, CTexture> m_loadedTiles;
    std::set<STileIndex> m_pendingTiles;  // fetches in-flight

    std::mutex m_tilesMutex; // Mutlithread

    std::queue<STileReadyData>  m_readyQueue;
    std::mutex                  m_readyMutex; // Mutlithread
};