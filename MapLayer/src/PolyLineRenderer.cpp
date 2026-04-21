#include "PolyLineRenderer.h"
#include "ShadersManager.h"
#include "StateManager.h"

#include <algorithm>

#undef min
#undef max

CPolyLineRenderer::~CPolyLineRenderer()
{
    Destroy();
}

bool CPolyLineRenderer::Initialize()
{
    InitQuadVAO();

    m_bInitialized = true;
    return true;
}

void CPolyLineRenderer::UploadRoads(const std::vector<STileRoadData>& roads, const CMapCamera& camera)
{
    m_iUploadedZoom = camera.GetZoom();  // record it

    std::vector<SLinesVertex> verts;
    m_vLineCounts.clear();
    std::vector<uint32_t> indices;

    // Epsilon in screen pixels — higher = more simplification
    // float fEpsilon = std::max(1.0f, 8.0f - (float)camera.GetZoom() * 0.5f);
    float fEpsilon = std::max(0.1f, 2.0f - (float)camera.GetZoom() * 0.1f);

    for (const STileRoadData& road : roads)
    {
        Vector4D color = GetRoadColor(road.sHighwayType);
        float fWidth = GetRoadWidth(road.sHighwayType, camera.GetZoom());

        // Convert to screen first
        std::vector<Vector2D> worldPts;
        worldPts.reserve(road.vLatLngPoints.size());
        for (const Vector2D& pt : road.vLatLngPoints)
        {
            worldPts.push_back(Anubis::LatLngToPixel(pt.x, pt.y, camera.GetZoom()));
        }

        // Simplify
        std::vector<Vector2D> simplified;
        Anubis::DouglasPeucker(worldPts, fEpsilon, simplified);
        if (simplified.size() < 2)
        {
           continue;
        }

        int iSplineSegs = (m_iUploadedZoom >= 16) ? 8 : 4; // more segments at high zoom
        std::vector<Vector2D> smooth = Anubis::CatmullRomSpline(simplified, iSplineSegs);

        // Build thick line — but now with world px positions
        BuildThickLine(smooth, fWidth, color, verts, indices);
    }

    syslog("UploadRoads: {} verts, {} indices", verts.size(), indices.size());

    // Upload using your existing buffer helpers
    Anubis::GL::EnsureBufferCapacity<SLinesVertex>(m_bufferGroup, m_uiQuadVAO, verts.size(), indices.size());
    Anubis::GL::UpdateDynamicBufferGroup<SLinesVertex>(m_bufferGroup, verts.data(), verts.size(), indices.data(), indices.size());

    m_iTotalIndices = (int32_t)indices.size();
    m_iTotalRoads = (int32_t)roads.size();

    syslog("UploadRoads: {} roads, {} verts after simplification (epsilon={})", m_iTotalRoads, verts.size(), fEpsilon);
}

void CPolyLineRenderer::BuildThickLine(const std::vector<Vector2D>& pts, float fWidth, const Vector4D& color, std::vector<SLinesVertex>& verts, std::vector<uint32_t>& indices)
{
    float fHalf = fWidth * 0.5f;

    for (size_t i = 0; i + 1 < pts.size(); i++)
    {
        Vector2D a = pts[i];
        Vector2D b = pts[i + 1];

        // Perpendicular normal
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 0.5f)
        {
            continue;
        }

        float nx = -dy / len * fHalf;
        float ny = dx / len * fHalf;

        uint32_t base = (uint32_t)verts.size();

        // 4 corners of the quad
        verts.emplace_back(Vector3D(a.x + nx, a.y + ny, 0.0f), color);
        verts.emplace_back(Vector3D(a.x - nx, a.y - ny, 0.0f), color);
        verts.emplace_back(Vector3D(b.x + nx, b.y + ny, 0.0f), color);
        verts.emplace_back(Vector3D(b.x - nx, b.y - ny, 0.0f), color);

        // Two triangles
        indices.insert(indices.end(), { base, base + 1, base + 2,  base + 1, base + 3, base + 2 });
    }
}

void CPolyLineRenderer::Render(int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera)
{
    if (!m_bInitialized || m_iTotalIndices == 0)
    {
        // syserr("Failed to Render Polylines");
        return;
    }

    if (camera.GetZoom() != m_iUploadedZoom)
    {
        return;  // skip rendering stale data, wait for re-upload
    }

    auto* pShader = CShadersManager::Instance().GetShader("DebuggingRenderer"); // existing lines shader
    if (!pShader)
    {
        return;
    }

    auto& state = CStateManager::Instance();
    state.PushState();

    glm::mat4 proj = glm::ortho(
        0.0f, (float)iScreenW,
        (float)iScreenH, 0.0f,
        -1.0f, 1.0f
    );

    Vector2D origin = camera.GetOriginWorldPixel(iScreenW, iScreenH);

    state.BindShader(pShader);
    pShader->SetBool("u_bRenderingPolyLines", true);
    pShader->SetMat4("u_Projection", proj);
    pShader->SetVec2("u_CameraOrigin", origin.x, origin.y);  // updated every frame

    state.SetEngineCapability(GL_BLEND, false);
    state.SetEngineCapability(GL_DEPTH_TEST, false);
    state.SetEngineCapability(GL_CULL_FACE, false);

    state.BindVertexArray(m_uiQuadVAO);

    // Single draw call for all roads
    glDrawElements(GL_TRIANGLES, m_iTotalIndices, GL_UNSIGNED_INT, 0);

    state.BindVertexArray(0);
    state.PopState();
}

void CPolyLineRenderer::Destroy()
{
    // 1. GPU Objects (VAOs must go before VBOs)
    Anubis::GL::SafeDeleteGLVertexArray(m_uiQuadVAO);

    // 2. Helper functions for VBO/EBO
    Anubis::GL::SafeDeleteGPUBufferGroup(m_bufferGroup);

    // 3. Reset Initialization Flag
    m_bInitialized = false;
}

void CPolyLineRenderer::InitQuadVAO()
{
    Anubis::GL::InitializeBufferGroup<LinesVertex>(m_bufferGroup, m_uiQuadVAO);
}

Vector4D CPolyLineRenderer::GetRoadColor(const std::string& sType)
{
    if (sType == "motorway" || sType == "trunk")
        return Vector4D(0.9f, 0.3f, 0.2f, 1.0f);   // red
    if (sType == "primary")
        return Vector4D(0.9f, 0.6f, 0.1f, 1.0f);   // orange
    if (sType == "secondary")
        return Vector4D(0.9f, 0.85f, 0.2f, 1.0f);  // yellow
    if (sType == "residential" || sType == "tertiary")
        return Vector4D(1.0f, 1.0f, 1.0f, 0.8f);   // white
    return Vector4D(0.7f, 0.7f, 0.7f, 0.6f);       // gray fallback
}

float CPolyLineRenderer::GetRoadWidth(const std::string& sType, int32_t iZoom)
{
    float fBase;
    if (sType == "motorway" || sType == "trunk")
        fBase = 3.0f;
    else if (sType == "primary")
        fBase = 2.0f;
    else if (sType == "secondary")
        fBase = 1.5f;
    else
        fBase = 1.0f;

    // Scale with zoom — zoom 12 = 1x, zoom 15 = 2x, zoom 10 = 0.5x
    float fZoomScale = std::powf(2.0f, (iZoom - 12) * 0.5f);
    return glm::clamp(fBase * fZoomScale, 1.0f, 12.0f);
}
