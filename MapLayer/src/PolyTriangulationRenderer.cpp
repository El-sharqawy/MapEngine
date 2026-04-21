#include "PolyTriangulationRenderer.h"
#include "ShadersManager.h"
#include "StateManager.h"

#undef min
#undef max

#include <mapbox/earcut.hpp>

namespace mapbox {
    namespace util {
        template <>
        struct nth<0, Vector2D> {
            static float get(const Vector2D& p) { return p.x; }
        };
        template <>
        struct nth<1, Vector2D> {
            static float get(const Vector2D& p) { return p.y; }
        };
    } // namespace util
} // namespace mapbox

CPolyTriangulationRenderer::~CPolyTriangulationRenderer()
{
	Destroy();
}

bool CPolyTriangulationRenderer::Initialize()
{
	Anubis::GL::InitializeBufferGroup<SLinesVertex>(m_bufferGroup, m_uiVAO);

	m_bInitialized = true;
	return true;
}

void CPolyTriangulationRenderer::UploadBuildings(const std::vector<STileBuildingData>& buildings, int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera)
{
    m_iUploadedZoom = camera.GetZoom();  // record it

    std::vector<SLinesVertex>  verts;
    std::vector<uint32_t>     indices;

    m_uploadOrigin = camera.GetOriginWorldPixel(iScreenW, iScreenH); // store screenW/H or pass them
    Vector2D origin = m_uploadOrigin;

    for (const STileBuildingData& building : buildings)
    {
        if (building.vLatLngPoints.size() < 3)
            continue;

        Vector4D color = GetBuildingColor(building.sBuildingType);

        // Convert lat/lng → screen positions
        // earcut expects vector of rings; each ring is vector of {x,y}
        std::vector<std::vector<Vector2D>> polygon(1);

        for (const SLatLng& pt : building.vLatLngPoints)
        {
            double lat = pt.lat;
            double lng = pt.lng;

            double scale = 256.0 * std::pow(2.0, camera.GetZoom());
            double worldX = (lng + 180.0) / 360.0 * scale;
            double sinLat = std::sin(lat * M_PI / 180.0);
            double worldY = (0.5 - std::log((1.0 + sinLat) / (1.0 - sinLat)) / (4.0 * M_PI)) * scale;

            float sx = (float)(worldX - (double)origin.x);
            float sy = (float)(worldY - (double)origin.y);

            polygon[0].push_back({ sx, sy });
        }

        // Triangulate
        std::vector<uint32_t> tris = mapbox::earcut<uint32_t>(polygon);
        if (tris.empty()) continue;

        uint32_t base = (uint32_t)verts.size();

        // Push vertices
        for (const Vector2D& p : polygon[0])
            verts.emplace_back(Vector3D(p.x, p.y, 0.0f), color);

        // Push indices offset by base
        for (uint32_t idx : tris)
            indices.push_back(base + idx);
    }

    if (verts.empty() || indices.empty())
    {
        m_iTotalIndices = 0;
        return;
    }

    syslog("{} verts, {} indices", verts.size(), indices.size());

    Anubis::GL::EnsureBufferCapacity<SLinesVertex>(m_bufferGroup, m_uiVAO, verts.size(), indices.size());
    Anubis::GL::UpdateDynamicBufferGroup<SLinesVertex>(m_bufferGroup,
        verts.data(), verts.size(), indices.data(), indices.size());

    m_iTotalIndices = (int32_t)indices.size();
}

void CPolyTriangulationRenderer::Render(int32_t iScreenW, int32_t iScreenH, const CMapCamera& camera)
{
    if (!m_bInitialized || m_iTotalIndices == 0)
    {
        return;
    }

    if (camera.GetZoom() != m_iUploadedZoom)
    {
        return;  // skip rendering stale data, wait for re-upload
    }

    auto* pShader = CShadersManager::Instance().GetShader("BasicRenderer");
    if (!pShader) return;

    glm::mat4 proj = glm::ortho(0.0f, (float)iScreenW,
        (float)iScreenH, 0.0f, -1.0f, 1.0f);

    Vector2D currentOrigin = camera.GetOriginWorldPixel(iScreenW, iScreenH);
    float fDeltaX = currentOrigin.x - m_uploadOrigin.x;
    float fDeltaY = currentOrigin.y - m_uploadOrigin.y;

    auto& state = CStateManager::Instance();
    state.PushState();

    state.BindShader(pShader);
    pShader->Use();
    pShader->SetBool("u_bRenderingPolyLines", true);
    pShader->SetMat4("u_Projection", proj);
    pShader->SetVec2("u_CameraOrigin", fDeltaX, fDeltaY);  // ← re-enable, now a delta

    state.SetEngineCapability(GL_BLEND, true);
    state.SetEngineCapability(GL_DEPTH_TEST, false);
    state.SetEngineCapability(GL_CULL_FACE, false);
    state.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state.BindVertexArray(m_uiVAO);
    glDrawElements(GL_TRIANGLES, m_iTotalIndices, GL_UNSIGNED_INT, 0);
    syslog("Building Render: zoom={} uploadedZoom={} indices={} origin=({:.1f},{:.1f})", camera.GetZoom(), m_iUploadedZoom, m_iTotalIndices, m_uploadOrigin.x, m_uploadOrigin.y);

    state.BindVertexArray(0);

    state.PopState();
}

void CPolyTriangulationRenderer::Destroy()
{
	// 1. GPU Objects (VAOs must go before VBOs)
	Anubis::GL::SafeDeleteGLVertexArray(m_uiVAO);

	// 2. Helper functions for VBO/EBO
	Anubis::GL::SafeDeleteGPUBufferGroup(m_bufferGroup);

	// 3. Reset Initialization Flag
	m_bInitialized = false;
}

Vector4D CPolyTriangulationRenderer::GetBuildingColor(const std::string& sType)
{
    if (sType == "commercial")
        return Vector4D(0.8f, 0.6f, 0.3f, 0.75f);
    if (sType == "industrial")
        return Vector4D(0.6f, 0.6f, 0.7f, 0.75f);
    if (sType == "residential")
        return Vector4D(0.7f, 0.8f, 0.6f, 0.75f);
    return Vector4D(0.75f, 0.72f, 0.68f, 0.75f); // default beige
}
