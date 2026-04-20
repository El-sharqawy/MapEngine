#include "PolyLineRenderer.h"
#include "ShadersManager.h"
#include "StateManager.h"

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

void CPolyLineRenderer::UploadRoads(const std::vector<STileRoadData>& roads, const CMapCamera& camera, int iScreenW, int iScreenH)
{
    std::vector<SLinesVertex> verts;
    m_vLineCounts.clear();
    std::vector<uint32_t> indices;

    for (const STileRoadData& road : roads)
    {
        // Pick color by road type
        Vector4D color = GetRoadColor(road.sHighwayType);
        int32_t count = 0;

        for (const Vector2D& pt : road.vLatLngPoints)
        {
            Vector2D screen = camera.LatLngToScreen(pt.x, pt.y, iScreenW, iScreenH);
            verts.emplace_back(Vector3D(screen.x, screen.y, 0.0f), color);
            count++;
        }
        m_vLineCounts.push_back(count);
    }

    indices.push_back(0);

    // Upload using your existing buffer helpers
    Anubis::GL::EnsureBufferCapacity<SLinesVertex>(m_bufferGroup, m_uiQuadVAO, verts.size(), indices.size());
    Anubis::GL::UpdateDynamicBufferGroup<SLinesVertex>(m_bufferGroup, verts.data(), verts.size(), indices.data(), indices.size());

    m_iTotalRoads = (int32_t)roads.size();
}

void CPolyLineRenderer::Render(int32_t iScreenW, int32_t iScreenH)
{
    if (!m_bInitialized || m_vLineCounts.empty())
    {
        // syserr("Failed to Render Polylines");
        return;
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

    state.BindShader(pShader);
    pShader->SetBool("u_bRenderingPolyLines", true);
    pShader->SetMat4("u_Projection", proj);

    state.SetEngineCapability(GL_BLEND, false);
    state.SetEngineCapability(GL_DEPTH_TEST, false);
    state.SetEngineCapability(GL_CULL_FACE, false);

    state.BindVertexArray(m_uiQuadVAO);
    glLineWidth(2.0f);

    int32_t offset = 0;
    for (int32_t count : m_vLineCounts)
    {
        glDrawArrays(GL_LINE_STRIP, offset, count);
        offset += count;
    }

    glLineWidth(1.0f);

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
