#include "TileGrid.h"
#include "GLUtils.h"
#include "TimerManager.h"

#include <algorithm>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

void CTileGrid::Update(Vector2D v2CameraOrigin, float fViewportW, float fViewportH, int32_t iZoom)
{
    m_vVisibleTiles.clear();

    // Clamp zoom
    iZoom = std::max(0, std::min(19, iZoom));
    int32_t iMaxTile = (1 << iZoom) - 1;

    // World-pixel bounds of the viewport
    float fLeft = floorf(v2CameraOrigin.x);
    float fTop = floorf(v2CameraOrigin.y);
    float fRight = fLeft + fViewportW;
    float fBottom = fTop + fViewportH;

    // Convert viewport corners to tile indices
    int32_t iMinTileX = static_cast<int32_t>(std::floor(fLeft / TILE_SIZE_PX));
    int32_t iMinTileY = static_cast<int32_t>(std::floor(fTop / TILE_SIZE_PX));
    int32_t iMaxTileX = static_cast<int32_t>(std::floor(fRight / TILE_SIZE_PX));
    int32_t iMaxTileY = static_cast<int32_t>(std::floor(fBottom / TILE_SIZE_PX));
    
    // Clamp to valid world bounds
    iMinTileX = std::max(0, iMinTileX);
    iMinTileY = std::max(0, iMinTileY);
    iMaxTileX = std::min(iMaxTile, iMaxTileX);
    iMaxTileY = std::min(iMaxTile, iMaxTileY);

    // Build visible tile list — row-major order (top to bottom, left to right)
    for (int32_t y = iMinTileY; y <= iMaxTileY; ++y)
    {
        for (int32_t x = iMinTileX; x <= iMaxTileX; ++x)
        {
            STileIndex tile;
            tile.iTileX = x;
            tile.iTileY = y;
            tile.iZoom = iZoom;

            // Where this tile starts on screen
            tile.v2ScreenPos = Vector2D(
                x * TILE_SIZE_PX - v2CameraOrigin.x,
                y * TILE_SIZE_PX - v2CameraOrigin.y
            );

            m_vVisibleTiles.emplace_back(tile);
        }
    }
}

Vector2D CTileGrid::WorldPixelToScreen(Vector2D v2WorldPixel, Vector2D v2CameraOrigin)
{
    return Vector2D(v2WorldPixel.x - v2CameraOrigin.x,
        v2WorldPixel.y - v2CameraOrigin.y);
}

Vector2D CTileGrid::GetTileScreenOrigin(const STileIndex& tile, Vector2D v2CameraOrigin) const
{
    Vector2D v2WorldOrigin = tile.GetOriginWorldPixel();
    return WorldPixelToScreen(v2WorldOrigin, v2CameraOrigin);
}

void CTileGrid::RequestTile(const STileIndex& idx)
{
    // 1. Already loaded or already in-flight — nothing to do
    {
        std::lock_guard<std::mutex> lock(m_tilesMutex);
        if (m_loadedTiles.count(idx) || m_pendingTiles.count(idx))
        {
            return;
        }

        m_pendingTiles.insert(idx);
    }

    // 2. Fire async fetch — disk cache first, network fallback
    std::thread([this, idx]()
        {
            std::string cachePath = "Assets/Textures/cache/" + std::to_string(idx.iZoom) + "_"
                + std::to_string(idx.iTileX) + "_"
                + std::to_string(idx.iTileY) + ".png";

            // Download if not cached
            if (!std::filesystem::exists(cachePath))
            {
                std::filesystem::create_directories("Assets/Textures/cache/");

                httplib::SSLClient cli("tile.openstreetmap.org");
                cli.set_connection_timeout(5);
                cli.set_read_timeout(10);
                cli.enable_server_certificate_verification(false); // skip cert check for now
                cli.set_default_headers({ {"User-Agent", "AnubisMapEngine/1.0"} });

                std::string url = "/" + std::to_string(idx.iZoom) + "/"
                    + std::to_string(idx.iTileX) + "/"
                    + std::to_string(idx.iTileY) + ".png";

                auto res = cli.Get(url.c_str());
                if (!res)
                {
                    printf("[TileGrid] HTTP FAILED: %s — error: %d\n",
                        url.c_str(), (int)res.error());
                    std::lock_guard<std::mutex> lock(m_tilesMutex);
                    m_pendingTiles.erase(idx);
                    return;
                }
                if (res->status != 200)
                {
                    printf("[TileGrid] HTTP %d for: %s\n", res->status, url.c_str());
                    std::lock_guard<std::mutex> lock(m_tilesMutex);
                    m_pendingTiles.erase(idx);
                    return;
                }

                printf("[TileGrid] Downloaded: %s (%zu bytes)\n", url.c_str(), res->body.size());
                std::ofstream f(cachePath, std::ios::binary);
                f.write(res->body.data(), res->body.size());
            }

            // Decode pixels on worker thread (stbi is thread-safe)
            int w, h, channels;
            stbi_set_flip_vertically_on_load(true);
            uint8_t* pData = stbi_load(cachePath.c_str(), &w, &h, &channels, 4);

            if (!pData)
            {
                std::lock_guard<std::mutex> lock(m_tilesMutex);
                m_pendingTiles.erase(idx);
                return;
            }

            // Push to ready queue — main thread will upload to GPU
            STileReadyData ready{};
            ready.index = idx;
            ready.imageData.pData = pData;   // ownership transferred
            ready.imageData.iWidth = w;
            ready.imageData.iHeight = h;
            ready.imageData.iChannels = channels;
            ready.fLoadTime = CTimerManager::Instance().GetElapsedTimeF();

            std::lock_guard<std::mutex> lock(m_readyMutex);
            m_readyQueue.push(std::move(ready));

        }).detach();
}

CTexture* CTileGrid::GetTileTexture(const STileIndex& idx)
{
    // Must be called from main thread — also uploads any pending GPU textures
    UploadPendingTiles();

    auto it = m_loadedTiles.find(idx);
    return it != m_loadedTiles.end() ? &it->second.Texture : nullptr;
}

float CTileGrid::GetTileLoadTime(const STileIndex& idx) const
{
    auto it = m_loadedTiles.find(idx);
    return it != m_loadedTiles.end() ? it->second.fLoadTime : 0.0f;
}

bool CTileGrid::IsTileLoaded(const STileIndex& idx) const
{
    return m_loadedTiles.count(idx) > 0;
}

void CTileGrid::UploadPendingTiles()
{
    std::lock_guard<std::mutex> lock(m_readyMutex);

    while (!m_readyQueue.empty())
    {
        STileReadyData ready = std::move(m_readyQueue.front());
        m_readyQueue.pop();

        // DEBUG — write first tile to PNG to verify pixel data
        static bool bWrittenOnce = false;
        if (!bWrittenOnce)
        {
            bWrittenOnce = true;
            stbi_write_png("debug_tile.png",
                ready.imageData.iWidth,
                ready.imageData.iHeight,
                4,                        // channels (you load with 4)
                ready.imageData.pData,
                ready.imageData.iWidth * 4); // stride = width * 4 bytes
            printf("[Debug] Written debug_tile.png (%dx%d)\n",
                ready.imageData.iWidth, ready.imageData.iHeight);
        }

        // Upload to GPU on main thread
        // Anubis::GL::UploadTextureDataToGPU(tex.GetTextureIDRef(), ready.imageData, GL_TEXTURE_2D, false, GL_UNSIGNED_BYTE);
        GLuint texID = 0;
        glCreateTextures(GL_TEXTURE_2D, 1, &texID);

        glTextureStorage2D(texID, 1, GL_RGBA8,
            ready.imageData.iWidth, ready.imageData.iHeight);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTextureSubImage2D(texID, 0, 0, 0,
            ready.imageData.iWidth, ready.imageData.iHeight,
            GL_RGBA,            // GL_BGRA for correct colors
            GL_UNSIGNED_BYTE,
            ready.imageData.pData);

        glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // tex.GetTextureIDRef() = texID;

        // Free CPU pixels
        stbi_image_free(ready.imageData.pData);

        // Store in cache
        {
            STileCacheEntry entry;
            entry.Texture.GetTextureIDRef() = texID;
            entry.fLoadTime = ready.fLoadTime;  // carry from ready data

            std::lock_guard<std::mutex> tileLock(m_tilesMutex);
            m_loadedTiles[ready.index] = std::move(entry);
            m_pendingTiles.erase(ready.index);
        }
    }
}
