#pragma once

#include "CoordTransform.h"
#include "TypeVector2.h"
#include "MathUtils.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <glm/trigonometric.hpp>

#undef min
#undef max

namespace Anubis
{
    inline static int32_t TileCount(int32_t zoom)
    {
        return (1 << zoom); // 2^zoom
    }

    // Tile index from lat/lng at zoom level
    // Lat/Lng (degrees) -> tile XY index at a given zoom level
    inline static Vector2D LatLngToTileXY(double lat, double lng, int32_t zoom)
    {
        const double n = static_cast<double>(TileCount(zoom));

        double latRad = lat * M_PI / 180.0;
        int32_t x = static_cast<int32_t>(std::floor((lng + 180.0) / 360.0 * n));
        int32_t y = static_cast<int32_t>(std::floor((1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / M_PI) / 2.0 * n));

        // Clamp to valid tile range
        x = std::max(0, std::min(x, static_cast<int32_t>(n) - 1));
        y = std::max(0, std::min(y, static_cast<int32_t>(n) - 1));

        return Vector2D(x, y);
    }

    // Pixel offset within a tile (0.0 - 1.0 UV)
    // Useful for pinpointing exact pixel within a tile
    inline static Vector2D LatLngToTileUV(double lat, double lng, int32_t zoom)
    {
        const double n = static_cast<double>(TileCount(zoom));

        double latRad = lat * M_PI / 180.0;
        double tileX = (lng + 180.0) / 360.0 * n;
        double tileY = (1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / M_PI) / 2.0 * n;

        // Fractional part = UV within the tile
        float u = static_cast<float>(tileX - std::floor(tileX));
        float v = static_cast<float>(tileY - std::floor(tileY));

        return Vector2D(u, v);
    }

    // World-space pixel position (for camera/pan math)
    inline static Vector2D LatLngToPixel(double lat, double lng, int32_t zoom)
    {
        const double n = static_cast<double>(TileCount(zoom));
        const double size = n * static_cast<double>(TILE_SIZE_PX);

        double latRad = lat * M_PI / 180.0;
        double px = (lng + 180.0) / 360.0 * size;
        double py = (1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / M_PI) / 2.0 * size;

        return Vector2D(static_cast<float>(px), static_cast<float>(py));
    }

    // World pixel position → Lat/Lng (inverse of LatLngToPixel)
    // Used by CMapCamera::Pan() to convert pixel movement back to lat/lng delta
    inline static Vector2D PixelToLatLng(double px, double py, int32_t zoom)
    {
        const double n = static_cast<double>(TileCount(zoom));
        const double size = n * static_cast<double>(TILE_SIZE_PX);

        // Inverse of LatLngToPixel
        double lng = px / size * 360.0 - 180.0;

        // Inverse of the Mercator Y formula
        double latRad = std::atan(std::sinh(M_PI * (1.0 - 2.0 * py / size)));
        double lat = latRad * 180.0 / M_PI;

        // Clamp to valid Mercator range
        lat = std::max(-85.05112878, std::min(85.05112878, lat));
        lng = std::max(-180.0, std::min(180.0, lng));

        return Vector2D(static_cast<float>(lat), static_cast<float>(lng));
    }
    // Haversine distance between two points (display on screen)
    // Haversine formula — great-circle distance in meters
    inline static double DistanceMeters(double lat1, double lng1, double lat2, double lng2)
    {
        double dLat = (lat2 - lat1) * M_PI / 180.0;
        double dLng = (lng2 - lng1) * M_PI / 180.0;

        double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
            std::cos(lat1 * M_PI / 180.0) * std::cos(lat2 * M_PI / 180.0) *
            std::sin(dLng / 2.0) * std::sin(dLng / 2.0);

        double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        return EARTH_RADIUS_METERS * c;
    }

    // Human-readable distance string (auto-selects m vs km)
    inline static std::string FormatDistance(double meters)
    {
        if (meters < 1000.0)
            return std::to_string(static_cast<int>(std::round(meters))) + " m";

        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.2f km", meters / 1000.0);
        return std::string(buf);
    }

    // Returns bearing in degrees (0 = North, 90 = East, clockwise)
    inline static double BearingDegrees(double lat1, double lng1, double lat2, double lng2)
    {
        double dLng = (lng2 - lng1) * M_PI / 180.0;
        double lat1R = lat1 * M_PI / 180.0;
        double lat2R = lat2 * M_PI / 180.0;

        double x = std::sin(dLng) * std::cos(lat2R);
        double y = std::cos(lat1R) * std::sin(lat2R) -
            std::sin(lat1R) * std::cos(lat2R) * std::cos(dLng);

        double bearing = std::atan2(x, y) * 180.0 / M_PI;
        return std::fmod(bearing + 360.0, 360.0); // normalize to [0, 360)
    }

    static inline std::string FmtCoord(double d)
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(7) << d;
        return ss.str();
    }

    inline static std::string BuildRoadsQuery(double minLat, double minLng,
        double maxLat, double maxLng,
        int iZoom)
    {
        std::string sFilter = (iZoom <= 13)
            ? "motorway|trunk|primary|secondary"
            : (iZoom <= 15)
            ? "motorway|trunk|primary|secondary|tertiary|residential"
            : "motorway|trunk|primary|secondary|tertiary|residential|unclassified|service|living_street";

        return "[out:json][timeout:30][maxsize:268435456];"
            "("
            "  way[\"highway\"~\"" + sFilter + "\"]"
            "  (" + FmtCoord(minLat) + "," + FmtCoord(minLng) + ","
            + FmtCoord(maxLat) + "," + FmtCoord(maxLng) + ");"
            ");"
            "out geom;";
    }

    inline static std::string BuildBuildingsQuery(double minLat, double minLng, double maxLat, double maxLng)
    {
        return "[out:json][timeout:45][maxsize:134217728];"
            "("
            "  way[\"building\"](" +
            FmtCoord(minLat) + "," + FmtCoord(minLng) + "," +
            FmtCoord(maxLat) + "," + FmtCoord(maxLng) +
            ");"
            ");"
            "out geom;";
    }

    // Simple URL encoder — replaces special chars for form POST
    inline static std::string UrlEncode(const std::string& s)
    {
        std::string result;
        result.reserve(s.size() * 3);
        for (unsigned char c : s)
        {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                result += c;
            else
            {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", c);
                result += buf;
            }
        }
        return result;
    }

    inline static float PerpendicularDistance(const Vector2D& pt, const Vector2D& lineStart, const Vector2D& lineEnd)
    {
        float dx = lineEnd.x - lineStart.x;
        float dy = lineEnd.y - lineStart.y;

        float mag = std::sqrt(dx * dx + dy * dy);
        if (mag < 0.0001f) return std::sqrt((pt.x - lineStart.x) * (pt.x - lineStart.x) +
            (pt.y - lineStart.y) * (pt.y - lineStart.y));

        return std::abs(dy * pt.x - dx * pt.y + lineEnd.x * lineStart.y - lineEnd.y * lineStart.x) / mag;
    }

    inline static void DouglasPeucker(const std::vector<Vector2D>& points, float fEpsilon, std::vector<Vector2D>& out)
    {
        if (points.size() < 2) return;

        // Find the point with max distance from the line start→end
        float fMaxDist = 0.0f;
        size_t uiMaxIdx = 0;

        for (size_t i = 1; i < points.size() - 1; i++)
        {
            float d = PerpendicularDistance(points[i], points.front(), points.back());
            if (d > fMaxDist)
            {
                fMaxDist = d;
                uiMaxIdx = i;
            }
        }

        if (fMaxDist > fEpsilon)
        {
            // Recurse on both halves
            std::vector<Vector2D> left(points.begin(), points.begin() + uiMaxIdx + 1);
            std::vector<Vector2D> right(points.begin() + uiMaxIdx, points.end());

            std::vector<Vector2D> leftResult, rightResult;
            DouglasPeucker(left, fEpsilon, leftResult);
            DouglasPeucker(right, fEpsilon, rightResult);

            // Merge (remove duplicate junction point)
            out = leftResult;
            out.insert(out.end(), rightResult.begin() + 1, rightResult.end());
        }
        else
        {
            // All points between are close enough — keep only endpoints
            out.push_back(points.front());
            out.push_back(points.back());
        }
    }

    inline static float HaversineDistanceKm(double lat1, double lng1, double lat2, double lng2)
    {
        constexpr double R = 6371.0; // Earth radius in km
        double dLat = glm::radians(lat2 - lat1);
        double dLng = glm::radians(lng2 - lng1);

        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
            std::cos(glm::radians(lat1)) * std::cos(glm::radians(lat2)) *
            std::sin(dLng / 2) * std::sin(dLng / 2);

        return (float)(2.0 * R * std::asin(std::sqrt(a)));
    }

    inline std::vector<Vector2D> CatmullRomSpline(
        const std::vector<Vector2D>& pts, int iSegments = 8)
    {
        std::vector<Vector2D> result;
        if (pts.size() < 2) return pts;

        for (size_t i = 0; i < pts.size() - 1; ++i)
        {
            // Control points — clamp at endpoints
            Vector2D p0 = pts[i > 0 ? i - 1 : i];
            Vector2D p1 = pts[i];
            Vector2D p2 = pts[i + 1];
            Vector2D p3 = pts[i + 2 < pts.size() ? i + 2 : i + 1];

            for (int j = 0; j < iSegments; ++j)
            {
                float t = (float)j / (float)iSegments;
                float t2 = t * t;
                float t3 = t2 * t;

                // Catmull-Rom basis
                float b0 = -0.5f * t3 + 1.0f * t2 - 0.5f * t;
                float b1 = 1.5f * t3 - 2.5f * t2 + 1.0f;
                float b2 = -1.5f * t3 + 2.0f * t2 + 0.5f * t;
                float b3 = 0.5f * t3 - 0.5f * t2;

                result.emplace_back(
                    b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x,
                    b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y
                );
            }
        }
        result.push_back(pts.back()); // add final point
        return result;
    }

}