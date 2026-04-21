#pragma once

#include "Shader.h"
#include "MapCamera.h"
#include "TileGrid.h"
#include "../../AnubisRenderer/include/GLUtils.h"

class CPolyTriangulationRenderer
{
public:
    ~CPolyTriangulationRenderer();

    bool Initialize();
    void UploadBuildings(const std::vector<STileBuildingData>& buildings, int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera);
    void Render(int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera);
    void Destroy();

private:
    Vector4D GetBuildingColor(const std::string& sType);

    GLuint m_uiVAO = 0;
    Anubis::GL::SGPUBuffersGroup m_bufferGroup{};
    int32_t m_iTotalIndices = 0;
    bool m_bInitialized = false;
    int32_t m_iUploadedZoom = -1;
    Vector2D m_uploadOrigin = { 0.0f, 0.0f };
};
