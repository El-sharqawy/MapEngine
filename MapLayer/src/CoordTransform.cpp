#include "CoordTransform.h"
#include <algorithm>

// the actual Mercator bounds at the 85.05° latitude clamp
// This is what OSM / Google Maps use — NOT simply R*PI
static constexpr double WEB_MERCATOR_MAX = 20037508.342789244;

// ----------------------------------------------------------------
//  Geographic <-> Web Mercator
// ----------------------------------------------------------------

/** Convert longitude (degrees) to Mercator X in meters */
double CCoordTransform::LonToMercatorX(double dLon)
{
    return dLon * (MAP_PI / 180.0) * EARTH_RADIUS_METERS;
}

/** Convert latitude (degrees) to Mercator Y in meters */
double CCoordTransform::LatToMercatorY(double dLat)
{
    // Clamp latitude to avoid infinity at poles
    dLat = std::max(-85.05112878, std::min(85.05112878, dLat));
    double dLatRad = dLat * (MAP_PI / 180.0);
    return std::log(std::tan(MAP_PI / 4.0 + dLatRad / 2.0)) * EARTH_RADIUS_METERS;
}

/** Convert Mercator X (meters) back to longitude (degrees) */
double CCoordTransform::MercatorXToLon(double dMercX)
{
    return dMercX / EARTH_RADIUS_METERS * (180.0 / MAP_PI);
}

/** Convert Mercator Y (meters) back to latitude (degrees) */
double CCoordTransform::MercatorYToLat(double dMercY)
{
    return (2.0 * std::atan(std::exp(dMercY / EARTH_RADIUS_METERS)) - MAP_PI / 2.0) * (180.0 / MAP_PI);
}

// ----------------------------------------------------------------
//  Mercator <-> World Pixel
// ----------------------------------------------------------------

/**
 * Convert Mercator meters -> world pixel position at zoom z.
 * World pixel origin is top-left (northwest corner of the world).
 * One tile = TILE_SIZE_PX pixels. Total world = (2^z * TILE_SIZE_PX) pixels.
 */
Vector2D CCoordTransform::MercatorToWorldPixel(double dMercX, double dMercY, int iZoom)
{
    double dWorldSize = static_cast<double>(TILE_SIZE_PX) * (1ULL << iZoom);

    float fPixelX = static_cast<float>((dMercX + WEB_MERCATOR_MAX) / (2.0 * WEB_MERCATOR_MAX) * dWorldSize);

    double dLat = MercatorYToLat(dMercY);

    double dLatRad = dLat * (std::numbers::pi / 180.0);
    double dLogVal = std::log(std::tan(dLatRad) + 1.0 / std::cos(dLatRad));

    float fPixelY = static_cast<float>(
        (1.0 - dLogVal / std::numbers::pi) / 2.0 * dWorldSize
        );

    return Vector2D(fPixelX, fPixelY);
}

/** Convert world pixel -> Mercator meters at zoom z */
void CCoordTransform::WorldPixelToMercator(Vector2D v2Pixel, int iZoom,
    double& dOutMercX, double& dOutMercY)
{
    double dWorldSize = static_cast<double>(TILE_SIZE_PX) * (1ULL << iZoom);
    // double dWorldSize = static_cast<double>(TILE_SIZE_PX) * std::pow(2.0, iZoom);

    dOutMercX = (v2Pixel.x / dWorldSize) * (2.0 * WEB_MERCATOR_MAX) - WEB_MERCATOR_MAX;
    dOutMercY = (1.0 - v2Pixel.y / dWorldSize) * (2.0 * WEB_MERCATOR_MAX) - WEB_MERCATOR_MAX;
}

// ----------------------------------------------------------------
//  Convenience: lat/lng directly to world pixel
// ----------------------------------------------------------------

/** lat/lng degrees -> world pixel at zoom z (Y increases downward = south) */
Vector2D CCoordTransform::LatLonToWorldPixel(double dLat, double dLon, int iZoom)
{
    double dMercX = LonToMercatorX(dLon);
    double dMercY = LatToMercatorY(dLat);
    return MercatorToWorldPixel(dMercX, dMercY, iZoom);
}

/** Convert world pixel position -> tile index (x, y) at zoom z */
void CCoordTransform::WorldPixelToTileXY(Vector2D v2Pixel, int& iOutTileX, int& iOutTileY)
{
    iOutTileX = static_cast<int>(v2Pixel.x / TILE_SIZE_PX);
    iOutTileY = static_cast<int>(v2Pixel.y / TILE_SIZE_PX);
}

/** Convert lat/lng -> tile index (x, y) at zoom z */
void CCoordTransform::LatLonToTileXY(double dLat, double dLon, int iZoom, int& iOutTileX, int& iOutTileY)
{
    double dMercX = LonToMercatorX(dLon);
    double dMercY = LatToMercatorY(dLat);

    SVector2Df v2Pixel = MercatorToWorldPixel(dMercX, dMercY, iZoom);
    WorldPixelToTileXY(v2Pixel, iOutTileX, iOutTileY);
}

/**
 * Get the world-pixel origin (top-left corner) of tile (z, x, y).
 * This is the position you place the tile quad at in world space.
 */
SVector2Df CCoordTransform::TileOriginWorldPixel(int iTileX, int iTileY)
{
    return SVector2Df(
        static_cast<float>(iTileX) * TILE_SIZE_PX,
        static_cast<float>(iTileY) * TILE_SIZE_PX
    );
}

/**
 * Haversine great-circle distance between two lat/lng points.
 * @return Distance in meters.
 */
double CCoordTransform::HaversineDistance(double dLat1, double dLon1, double dLat2, double dLon2)
{
    auto toRad = [](double d) { return d * MAP_PI / 180.0; }; // C++ function

    double dDLat = toRad(dLat2 - dLat1);
    double dDLon = toRad(dLon2 - dLon1);

    double a = std::sin(dDLat / 2) * std::sin(dDLat / 2)
        + std::cos(toRad(dLat1)) * std::cos(toRad(dLat2))
        * std::sin(dDLon / 2) * std::sin(dDLon / 2);

    return 2.0 * EARTH_RADIUS_METERS * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}
