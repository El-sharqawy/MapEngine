#include "TileIndex.h"

std::string STileIndex::GetURL() const
{
	// OSM tile URL format: https://tile.openstreetmap.org/{z}/{x}/{y}.png
    return "https://tile.openstreetmap.org/"
        + std::to_string(iZoom) + "/"
        + std::to_string(iTileX) + "/"
        + std::to_string(iTileY) + ".png";
}

Vector2D STileIndex::GetOriginWorldPixel() const
{
    return CCoordTransform::TileOriginWorldPixel(iTileX, iTileY);
}

Vector2D STileIndex::GetCenterWorldPixel() const
{
    SVector2Df origin = GetOriginWorldPixel();
    return SVector2Df(origin.x + TILE_SIZE_PX * 0.5f,
                      origin.y + TILE_SIZE_PX * 0.5f);
}

bool STileIndex::IsValid() const
{
    if (iZoom < 0 || iZoom > 19)
    {
        return false;
    }

    int32_t iMax = GetMaxIndex();
    return iTileX >= 0 && iTileX <= iMax
        && iTileY >= 0 && iTileY <= iMax;
}


