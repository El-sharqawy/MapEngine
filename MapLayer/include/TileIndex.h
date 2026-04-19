#pragma once

#include <string>
#include "CoordTransform.h"

/**
 * @brief Represents a single OSM tile address (zoom, x, y).
 *
 * Immutable value type — create one per tile, store in vectors.
 * Provides URL generation, world-pixel bounds, and neighbor queries.
 */
struct STileIndex
{
    int32_t iZoom = 0;
    int32_t iTileX = 0;
    int32_t iTileY = 0;
    Vector2D v2ScreenPos;

    STileIndex() = default;

    STileIndex(int32_t x, int32_t y, int32_t z) : iTileX(x), iTileY(y), iZoom(z) {}

    // Equality
    bool operator==(const STileIndex& other) const
    {
        return iTileX == other.iTileX && iTileY == other.iTileY && iZoom == other.iZoom;
    }

    bool operator!=(const STileIndex& other) const { return !(*this == other); }

    // Stable sort key for map/cache use
    bool operator<(const STileIndex& other) const
    {
        if (iTileX != other.iTileX)
        {
            return iTileX < other.iTileX;
        }
        if (iTileY != other.iTileY)
        {
            return iTileY < other.iTileY;
        }
        return iZoom < other.iZoom;
    }

    /** OSM tile PNG URL: https://tile.openstreetmap.org/{z}/{x}/{y}.png */
    std::string GetURL() const;

    /** Top-left corner of this tile in world-pixel space */
    Vector2D GetOriginWorldPixel() const;

    /** Center of this tile in world-pixel space */
    Vector2D GetCenterWorldPixel() const;

    /** Size of one tile in world-pixel space (always TILE_SIZE_PX) */
    float GetSizePx() const { return TILE_SIZE_PX; }

    /** Clamp tile X/Y to valid range for this zoom level [0, 2^z - 1] */
    bool IsValid() const;

    /** Max tile index at this zoom level (2^z - 1) */
    int32_t GetMaxIndex() const { return (1 << iZoom) - 1; }
};
