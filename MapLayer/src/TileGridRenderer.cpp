#include "TileGridRenderer.h"
#include "RendererManager.h"
#include "ShadersManager.h"
#include "TypeMatrix4.h"
#include "OrthographicProjection.h"
#include "EngineMathMatrix.h"
#include "StateManager.h"

CTileGridRenderer::~CTileGridRenderer()
{
    Destroy();
}

bool CTileGridRenderer::Initialize()
{
    // Unit quad in world-pixel space [0,0] -> [1,1]
    // Renderer will scale it to 256x256 via u_mvp
    InitQuadVAO();

    m_bInitialized = true;
    return true;
}

void CTileGridRenderer::InitQuadVAO()
{
    Anubis::GL::InitializeBufferGroup<SUIVertex>(m_bufferGroup, m_uiQuadVAO);

    // Unit quad [0,0] -> [1,1] — scaled to tile size via u_mvp
    std::vector<SUIVertex> vertices =
    {
        SUIVertex(0.0f, 0.0f, 0.0f, 1.0f),  // top-left UV=(0,1)
        SUIVertex(1.0f, 0.0f, 1.0f, 1.0f),  // top-right UV=(1,1)
        SUIVertex(1.0f, 1.0f, 1.0f, 0.0f),  // bottom-right UV=(1,0)
        SUIVertex(0.0f, 1.0f, 0.0f, 0.0f),  // bottom-left UV=(0,0)
    };

    std::vector<uint32_t> indices = { 0, 1, 2,  2, 3, 0 };

    // 2. Memory Management (The "Heavy Lifting")
    // These helpers handle the Allocation, Reallocation, and VAO Linking internally
    Anubis::GL::UpdateDynamicBufferGroup<SUIVertex>(m_bufferGroup, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void CTileGridRenderer::Render(CTileGrid& grid, const CMapCamera& camera, int iScreenW, int iScreenH)
{
    if (!m_bInitialized)
    {
        syserr("Not Initialized");
        return;
    }

    // Chech Shader
    auto* pUIShader = CShadersManager::Instance().GetShader("UserInterface");
    if (!pUIShader)
    {
        syserr("TileGridRenderer Shader is Initialized");
        return;
    }

    // glm::mat4 proj = camera.GetProjectionMatrix(iScreenW, iScreenH);

    // Save State
    auto& state = CStateManager::Instance();
    state.PushState();

    state.BindShader(pUIShader);

    glm::mat4 proj = glm::ortho(
        0.0f, (float)iScreenW,
        (float)iScreenH, 0.0f,  // ← bottom=screenH, top=0  (Y increases downward)
        -1.0f, 1.0f
    );

    pUIShader->Use();
    pUIShader->SetMat4("u_Projection", proj);

    // 3. Set standard UI states
    state.SetEngineCapability(GL_BLEND, true);
    state.SetEngineCapability(GL_DEPTH_TEST, false);
    state.SetEngineCapability(GL_CULL_FACE, false);
    state.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 2. Bind the shared UI Geometry (The VAO you created in Initialize)
    state.BindVertexArray(m_uiQuadVAO);

    for (const STileIndex& idx : grid.GetVisibleTiles())
    {
        CTexture* pTex = grid.GetTileTexture(idx);
        if (!pTex)
        {
            grid.RequestTile(idx);
            continue;
        }

        float screenX = idx.v2ScreenPos.x;
        float screenY = idx.v2ScreenPos.y;

        Matrix4 model = EngineMath::Translate(Matrix4(1.0f), Vector3D(screenX, screenY, 0.0f));
        model = EngineMath::Scale(model, Vector3D(TILE_SIZE_PX, TILE_SIZE_PX, 1.0f));

        if (pTex)
        {
            state.BindTexture(0, GL_TEXTURE_2D, pTex->GetTextureID());
            // pTex->BindTexture(0);
            pUIShader->SetInt("u_Texture", 0);
            pUIShader->SetBool("u_HasTexture", true);
            pUIShader->SetVec4("u_Color", Vector4D(0.0f)); // white = no tint
        }
        else
        {
            grid.RequestTile(idx); // keep requesting until loaded
            pUIShader->SetBool("u_HasTexture", false);

            // Checkerboard fallback while loading
            float r = (float)((idx.iTileX * 73856093) & 0xFF) / 255.0f;
            float g = (float)((idx.iTileY * 19349663) & 0xFF) / 255.0f;
            float b = (float)(((idx.iTileX ^ idx.iTileY) * 83492791) & 0xFF) / 255.0f;

            pUIShader->SetVec4("u_Color", Vector4D(r, g, b, 0.9f));
        }

        // Use the pre-calculated bounds to create a simple translation/scale
        // No need for parentTransform!
        pUIShader->SetMat4("u_Transform", model);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // 4. Cleanup/Restore State
    state.BindVertexArray(0);
    state.PopState();
}

void CTileGridRenderer::Destroy()
{
    // 1. GPU Objects (VAOs must go before VBOs)
    Anubis::GL::SafeDeleteGLVertexArray(m_uiQuadVAO);

    // 2. Helper functions for VBO/EBO
    Anubis::GL::SafeDeleteGPUBufferGroup(m_bufferGroup);

    // 3. Reset Initialization Flag
    m_bInitialized = false;
}
