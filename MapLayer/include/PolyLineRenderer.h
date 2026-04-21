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
    void UploadRoads(const std::vector<STileRoadData>& roads, const CMapCamera& camera);
    void BuildThickLine(const std::vector<Vector2D>& pts, float fWidth, const Vector4D& color, std::vector<SLinesVertex>& verts, std::vector<uint32_t>& indices);

    void Render(int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera);
    void Destroy();

private:
    void InitQuadVAO();
    Vector4D GetRoadColor(const std::string& sType);
    float GetRoadWidth(const std::string& sType, int32_t iZoom);

    GLuint m_uiQuadVAO = 0;
    Anubis::GL::SGPUBuffersGroup m_bufferGroup; // VBO / EBO
    bool m_bInitialized = false;

    int32_t m_iTotalRoads;
    int32_t m_iTotalIndices;
    int32_t m_iUploadedZoom = -1;

    std::vector<int32_t> m_vLineCounts;
};