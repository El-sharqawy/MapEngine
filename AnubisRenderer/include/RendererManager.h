#pragma once

#include <Singleton.h>
#include "GLUtils.h"

class CRendererManager : public CSingleton<CRendererManager>
{
public:
	/**
	 * @brief Constructor - Initializes member variables.
	 */
	CRendererManager() = default;

	/**
	 * @brief Destructor - cleans up all resources.
	 */
	~CRendererManager() = default;

	/**
	 * @brief Initializes the renderer manager.
	 *
	 * This function sets up the necessary shaders and GPU buffers for rendering.
	 * It must be called before any rendering operations are performed.
	 */
	void Initialize();

	/**
	 * @brief Destroys the renderer manager and frees resources.
	 *
	 * This function cleans up all allocated resources, including shaders and GPU buffers.
	 * It should be called when the renderer manager is no longer needed.
	 */
	void Destroy();

	/**
	 * @brief Clears all resources used by the renderer manager.
	 *
	 * This function resets the state of the renderer manager, freeing up any allocated
	 * resources such as shaders and GPU buffers.
	 */
	void Clear();

	/**
	 * @brief Updates the renderer manager for the current frame.
	 *
	 * This function performs any necessary per-frame updates for the renderer manager.
	 * It should be called once per frame to ensure proper rendering behavior.
	 */
	void Update();

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
	void RenderDynamicBatchDebugging(GLenum mode, const SLinesVertex* vertices, GLsizeiptr numVertices, const GLuint* indices, GLsizeiptr numIndices);

protected:
	/**
	 * @brief Initializes the GPU buffers used for debugging rendering.
	 *
	 * This function creates and configures the Vertex Array Object (VAO) and
	 * associated Vertex Buffer Object (VBO) and Element Buffer Object (EBO)
	 * for rendering debug lines. It sets up the necessary buffer storage and
	 * vertex attributes for efficient rendering of debugging information.
	 */
	void InitializeDebuggingBuffers();

private:
	// Shared Dynamic Batch (Lines, simple shapes)
	GLuint m_uiDebuggingVAO;

	// GPU Buffers & Data for Debugging
	Anubis::GL::SGPUBuffersGroup m_DebuggingBufferGroup;

	// Main Dynamic Batch (complex shapes)
	GLuint m_uiMainVAO;

	// GPU Buffers & Data for Main Batch
	Anubis::GL::SGPUBuffersGroup m_MainBufferGroup;

	// Initialization Flag
	bool m_bInitialized;

	bool m_bIsDebuggingEnabled;
};