#pragma once

#include "TileGrid.h"
#include "Shader.h"
#include "MapCamera.h"
#include "../../AnubisRenderer/include/GLUtils.h"
#include <Singleton.h>
        
class CTileGridRenderer
{
public:
    CTileGridRenderer() = default;
    ~CTileGridRenderer();

    bool Initialize();
    void Render(CTileGrid& grid, const CMapCamera& camera, int iScreenW, int iScreenH);
    void Destroy();

private:
    void InitQuadVAO();

    CShader  m_tileShader;
    GLuint   m_uiQuadVAO = 0;
    Anubis::GL::SGPUBuffersGroup m_bufferGroup; // VBO / EBO
    bool     m_bInitialized = false;

};