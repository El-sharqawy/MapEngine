#pragma once

#include "Shader.h"
#include "MapCamera.h"
#include "TileGrid.h"
#include "../../AnubisRenderer/include/GLUtils.h"

class CPolyLineRenderer
{
public:
    CPolyLineRenderer() = default;
    ~CPolyLineRenderer();

    bool Initialize();
    void UploadRoads(const std::vector<STileRoadData>& roads, const CMapCamera& camera, int iScreenW, int iScreenH);

    void Render(int32_t iScreenW, int32_t iScreenH);
    void Destroy();

private:
    void InitQuadVAO();
    Vector4D GetRoadColor(const std::string& sType);

    GLuint m_uiQuadVAO = 0;
    Anubis::GL::SGPUBuffersGroup m_bufferGroup; // VBO / EBO
    bool m_bInitialized = false;

    int32_t m_iTotalRoads;

    std::vector<int32_t> m_vLineCounts;
};