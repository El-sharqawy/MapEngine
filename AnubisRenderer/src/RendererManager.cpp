#include "RendererManager.h"
#include "CameraManager.h"
#include "Camera.h"
#include "UniformBufferObject.h"
#include "StateManager.h"
#include "ShadersManager.h"

/**
 * @brief Initializes the renderer manager.
 *
 * This function sets up the necessary shaders and GPU buffers for rendering.
 * It must be called before any rendering operations are performed.
 */
void CRendererManager::Initialize()
{
	if (m_bInitialized)
	{
		return; // Already initialized
	}

	m_bIsDebuggingEnabled = true;

	// 1. Initialize Debugging Buffers
	if (m_bIsDebuggingEnabled)
	{
		Anubis::GL::InitializeBufferGroup<LinesVertex>(m_DebuggingBufferGroup, m_uiDebuggingVAO);
	}

	// 2. Initialize Main Buffers
	Anubis::GL::InitializeBufferGroup<Vertex>(m_MainBufferGroup, m_uiMainVAO);

	// Mark as initialized
	m_bInitialized = true;
}

/**
 * @brief Destroys the renderer manager and frees resources.
 *
 * This function cleans up all allocated resources, including shaders and GPU buffers.
 * It should be called when the renderer manager is no longer needed.
 */
void CRendererManager::Destroy()
{
	Clear();
}

/**
 * @brief Clears all resources used by the renderer manager.
 *
 * This function resets the state of the renderer manager, freeing up any allocated
 * resources such as shaders and GPU buffers.
 */
void CRendererManager::Clear()
{
	// 1. GPU Objects (VAOs must go before VBOs)
	Anubis::GL::SafeDeleteGLVertexArray(m_uiDebuggingVAO);
	Anubis::GL::SafeDeleteGLVertexArray(m_uiMainVAO);

	// 2. Helper functions for VBO/EBO
	Anubis::GL::SafeDeleteGPUBufferGroup(m_DebuggingBufferGroup);
	Anubis::GL::SafeDeleteGPUBufferGroup(m_MainBufferGroup);

	// 3. Reset Initialization Flag
	m_bInitialized = false;
}

/**
 * @brief Updates the renderer manager for the current frame.
 *
 * This function performs any necessary per-frame updates for the renderer manager.
 * It should be called once per frame to ensure proper rendering behavior.
 */
void CRendererManager::Update()
{
	// Update logic for the renderer manager can be added here.
	auto* pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (pCamera)
	{
		CStateManager::Instance().BindUniformBuffer(pCamera->GetUniformBufferObject()->GetBindingPoint(), pCamera->GetUniformBufferObject()->GetBufferID());
	}
}

/**
 * @brief Renders a dynamic batch of debug lines or shapes.
 *
 * This function uploads the provided vertex and index data to the GPU and issues
 * a draw call to render the specified geometry using the debug rendering shader.
 *
 * @param mode The OpenGL primitive type (e.g., GL_LINES, GL_TRIANGLES).
 * @param vertices Pointer to the array of SLinesVertex vertices.
 * @param numVertices Number of vertices in the array.
 * @param indices Pointer to the array of indices.
 * @param numIndices Number of indices in the array.
 */
void CRendererManager::RenderDynamicBatchDebugging(GLenum mode, const SLinesVertex* vertices, GLsizeiptr numVertices, const GLuint* indices, GLsizeiptr numIndices)
{
	if (!m_bIsDebuggingEnabled)
	{
		return;
	}

	// 1. Quick exit checks
	if (!m_bInitialized || numVertices == 0 || numIndices == 0)
	{
		syserr("RendererManager Cannot Render Debugging Batches!");
		return;
	}

	// 2. Memory Management (The "Heavy Lifting")
	// These helpers handle the Allocation, Reallocation, and VAO Linking internally
	Anubis::GL::EnsureBufferCapacity<SLinesVertex>(m_DebuggingBufferGroup, m_uiDebuggingVAO, numVertices, numIndices);
	Anubis::GL::UpdateDynamicBufferGroup<SLinesVertex>(m_DebuggingBufferGroup, vertices, numVertices, indices, numIndices);

	// 3. Set State
	auto* pDebuggingManager = CShadersManager::Instance().GetShader("DebuggingRenderer");
	CStateManager::Instance().BindShader(pDebuggingManager);
	CStateManager::Instance().BindVertexArray(m_uiDebuggingVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DebuggingBufferGroup.uiEBO);

	// 4. Draw
	// Note: No need to bind EBO here; it was linked during EnsureBufferCapacity/LinkBuffersToVAO
	glDrawElements(mode, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, nullptr);
}

/**
 * @brief Initializes the GPU buffers used for debugging rendering.
 *
 * This function creates and configures the Vertex Array Object (VAO) and
 * associated Vertex Buffer Object (VBO) and Element Buffer Object (EBO)
 * for rendering debug lines. It sets up the necessary buffer storage and
 * vertex attributes for efficient rendering of debugging information.
 */
void CRendererManager::InitializeDebuggingBuffers()
{
	// 1. Setup the Metadata inside the struct
	m_DebuggingBufferGroup.iVboCapacity = INITIAL_DEBUG_VERTEX_CAPACITY;
	m_DebuggingBufferGroup.iEboCapacity = INITIAL_DEBUG_INDEX_CAPACITY;
	m_DebuggingBufferGroup.DSAFlags = GL_DYNAMIC_STORAGE_BIT;
	m_DebuggingBufferGroup.LegacyUsage = GL_DYNAMIC_DRAW;

	// 2. Create the GPU "Names" (IDs)
	// These functions should call glCreate... or glGen..
	Anubis::GL::CreateVertexArray(m_uiDebuggingVAO);
	Anubis::GL::CreateGPUBufferGroup(m_DebuggingBufferGroup);

	// 3. Allocate the actual VRAM
	// This uses the metadata we just set to call glNamedBufferStorage or glBufferData
	Anubis::GL::AllocateBuffersStorage<SLinesVertex>(m_DebuggingBufferGroup);

	// 4. Define the Vertex Layout (What does a vertex look like?)
	// This tells the VAO how to interpret the data (Position, Color, etc.)
	Anubis::GL::SetupVertexBufferAttributes<SLinesVertex>(m_uiDebuggingVAO);

	// 5. Physical Link (Which buffers belong to this VAO?)
	// This tells the VAO which specific VBO/EBO IDs to pull from
	Anubis::GL::LinkBuffersToVAO<SLinesVertex>(m_uiDebuggingVAO, m_DebuggingBufferGroup);
}

