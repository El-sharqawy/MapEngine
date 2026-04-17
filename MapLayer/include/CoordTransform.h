#pragma once

#include "TypeVector2.h"
#include "TypeVector3.h"
#include "TypeVector4.h"

#include <cmath>

// Earth radius in meters (Web Mercator / EPSG:3857)
static constexpr double EARTH_RADIUS_METERS = 6378137.0;
static constexpr float  TILE_SIZE_PX = 256.0f; // OSM standard tile size

#include <numbers>  // C++20
static constexpr double MAP_PI = std::numbers::pi;  // Full precision: 3.141592653589793238...

/**
 * @brief Pure math utility — converts between geographic coordinates,
 *        Web Mercator world space, and tile indices.
 *
 * Coordinate spaces used:
 *   - Geographic : lat/lng in degrees (WGS84)
 *   - Mercator   : meters, origin at (0,0) = equator/prime meridian
 *   - World Pixel: pixel units at a given zoom, origin top-left of world
 *   - Tile Index : integer (z, x, y) tile address
 */
class CCoordTransform
{
public:
    // ----------------------------------------------------------------
    //  Geographic <-> Web Mercator (meters)
    // ----------------------------------------------------------------

    /** Convert longitude (degrees) to Mercator X in meters */
    static double LonToMercatorX(double dLon);

    /** Convert latitude (degrees) to Mercator Y in meters */
    static double LatToMercatorY(double dLat);

    /** Convert Mercator X (meters) back to longitude (degrees) */
    static double MercatorXToLon(double dMercX);

    /** Convert Mercator Y (meters) back to latitude (degrees) */
    static double MercatorYToLat(double dMercY);

    // ----------------------------------------------------------------
    //  Mercator (meters) <-> World Pixel space at zoom level z
    // ----------------------------------------------------------------

    /**
     * Convert Mercator meters -> world pixel position at zoom z.
     * World pixel origin is top-left (northwest corner of the world).
     * One tile = TILE_SIZE_PX pixels. Total world = (2^z * TILE_SIZE_PX) pixels.
     */
    static Vector2D MercatorToWorldPixel(double dMercX, double dMercY, int iZoom);

    /** Convert world pixel -> Mercator meters at zoom z */
    static void WorldPixelToMercator(Vector2D v2Pixel, int iZoom, double& dOutMercX, double& dOutMercY);

    // ----------------------------------------------------------------
    //  Convenience: lat/lng directly to world pixel
    // ----------------------------------------------------------------

    /** lat/lng degrees -> world pixel at zoom z (Y increases downward = south) */
    static Vector2D LatLonToWorldPixel(double dLat, double dLon, int iZoom);

    // ----------------------------------------------------------------
    //  Tile index math
    // ----------------------------------------------------------------

    /** Convert world pixel position -> tile index (x, y) at zoom z */
    static void WorldPixelToTileXY(Vector2D v2Pixel, int& iOutTileX, int& iOutTileY);

    /** Convert lat/lng -> tile index (x, y) at zoom z */
    static void LatLonToTileXY(double dLat, double dLon, int iZoom, int& iOutTileX, int& iOutTileY);

    /**
     * Get the world-pixel origin (top-left corner) of tile (z, x, y).
     * This is the position you place the tile quad at in world space.
     */
    static Vector2D TileOriginWorldPixel(int iTileX, int iTileY);

    // ----------------------------------------------------------------
    //  Distance
    // ----------------------------------------------------------------

    /**
     * Haversine great-circle distance between two lat/lng points.
     * @return Distance in meters.
     */
    static double HaversineDistance(double dLat1, double dLon1, double dLat2, double dLon2);
};