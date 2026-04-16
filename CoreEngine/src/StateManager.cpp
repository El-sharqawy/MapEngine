#include "StateManager.h"
#include "LogManager.h"
#include "CoreUtils.h"
#include "WindowManager.h"
#include "Shader.h"
#include "GLUtils.h"

/**
 * @brief Initializes the State Manager.
 *
 * This function sets the default OpenGL states and synchronizes
 * the internal state snapshot to reflect these defaults. It also
 * initializes the texture cache to prevent garbage data.
 */
void CStateManager::Initialize()
{
	m_iWindowWidth = CWindowManager::Instance().GetWidth();
	m_iWindowHeight = CWindowManager::Instance().GetHeight();

	// 1. Set Hard Defaults in OpenGL
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
	glBlendFunc(GL_ONE, GL_ZERO);
	glViewport(0, 0, m_iWindowWidth, m_iWindowHeight);
	glScissor(0, 0, m_iWindowWidth, m_iWindowHeight);

	// 2. Sync Internal Snapshot
	m_gCurState.m_mEngineCapabilities[GL_DEPTH_TEST] = true;
	m_gCurState.m_mEngineCapabilities[GL_BLEND] = false;
	m_gCurState.m_mEngineCapabilities[GL_SCISSOR_TEST] = false;

	m_gCurState.pCurrentShader = nullptr;
	m_gCurState.uiCurrentVAO = 0;
	m_gCurState.uiCurrentProgramID = 0;

	m_gCurState.blendSrc = GL_ONE;
	m_gCurState.blendDst = GL_ZERO;

	// Viewport and Scissor setup
	for (GLint i = 0; i < 4; ++i)
	{
		m_gCurState.viewport[i] = (i < 2) ? 0 : (i == 2 ? m_iWindowWidth : m_iWindowHeight);
		m_gCurState.scissorBox[i] = m_gCurState.viewport[i];
	}

	// 3. Initialize Texture Cache (to prevent garbage data)
	for (GLint i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		m_mEngineTextures[i] = 0;
		m_mEngineTargets[i] = GL_TEXTURE_2D;
	}
}

void CStateManager::Destroy()
{

}

/**
 * @brief Enables or disables a specific OpenGL capability.
 *
 * This function checks the current state of the specified capability
 * and only makes the OpenGL call to enable or disable it if the desired
 * state differs from the current state. This helps to minimize redundant
 * state changes and improve performance.
 *
 * @param eCap The OpenGL capability to modify (e.g., GL_DEPTH_TEST, GL_BLEND).
 * @param bEnable true to enable the capability, false to disable it.
 */
void CStateManager::SetEngineCapability(GLenum eCap, bool bEnable)
{
	// Make OpenGL call
	if (m_gCurState.m_mEngineCapabilities[eCap] != bEnable)
	{
		bEnable ? glEnable(eCap) : glDisable(eCap);
		m_gCurState.m_mEngineCapabilities[eCap] = bEnable;
	}
}

/**
 * @brief Sets the viewport for OpenGL.
 *
 * This function checks if the specified viewport parameters
 * are already set. If not, it calls glViewport to update the viewport,
 * minimizing redundant state changes.
 *
 * @param x The x-coordinate of the lower left corner of the viewport.
 * @param y The y-coordinate of the lower left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void CStateManager::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (m_gCurState.viewport[0] == x &&
		m_gCurState.viewport[1] == y &&
		m_gCurState.viewport[2] == width &&
		m_gCurState.viewport[3] == height)
	{
		return;
	}

	glViewport(x, y, width, height);
	m_gCurState.viewport[0] = x;
	m_gCurState.viewport[1] = y;
	m_gCurState.viewport[2] = width;
	m_gCurState.viewport[3] = height;
}

/**
 * @brief Sets the scissor rectangle for OpenGL.
 *
 * This function checks if the specified scissor rectangle parameters
 * are already set. If not, it calls glScissor to update the scissor
 * rectangle, minimizing redundant state changes.
 *
 * @param x The x-coordinate of the lower left corner of the scissor box.
 * @param y The y-coordinate of the lower left corner of the scissor box.
 * @param width The width of the scissor box.
 * @param height The height of the scissor box.
 */
void CStateManager::SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
	if (m_gCurState.scissorBox[0] == x &&
		m_gCurState.scissorBox[1] == y &&
		m_gCurState.scissorBox[2] == width &&
		m_gCurState.scissorBox[3] == height)
	{
		return;
	}

	glScissor(x, y, width, height);
	m_gCurState.scissorBox[0] = x;
	m_gCurState.scissorBox[1] = y;
	m_gCurState.scissorBox[2] = width;
	m_gCurState.scissorBox[3] = height;
}

/**
 * @brief Sets the blending function for OpenGL.
 *
 * This function checks if the specified source and destination
 * blending factors are already set. If not, it calls glBlendFunc
 * to update the blending function, minimizing redundant state changes.
 *
 * @param src The source blending factor (e.g., GL_SRC_ALPHA).
 * @param dst The destination blending factor (e.g., GL_ONE_MINUS_SRC_ALPHA).
 */
void CStateManager::SetBlendFunc(GLenum src, GLenum dst)
{
	// Compare against the CURRENT snapshot
	if (m_gCurState.blendSrc == src && m_gCurState.blendDst == dst)
		return;

	glBlendFunc(src, dst);
	m_gCurState.blendSrc = src;
	m_gCurState.blendDst = dst;
}

/**
 * @brief Binds a texture to a specified texture unit.
 *
 * This function checks if the texture and target for the given texture unit
 * are already bound. If not, it binds the specified texture to the texture unit.
 * It uses Direct State Access (DSA) methods if available, otherwise falls back
 * to legacy binding methods.
 *
 * @param uiUnit The texture unit index (0, 1, 2, ...).
 * @param eTexTarget The texture target (e.g., GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP).
 * @param uiTextureID The OpenGL texture ID to bind.
 */
void CStateManager::BindTexture(GLuint uiUnit, GLenum eTexTarget, GLuint uiTextureID)
{
	// 1. Safety check for slot range
	if (uiUnit >= 32)
	{
		return;
	}

	if (m_mEngineTextures[uiUnit] != uiTextureID || m_mEngineTargets[uiUnit] != eTexTarget)
	{
		if (Anubis::GL::IsGLVersionHigher(4, 5))
		{
			// Modern DSA: Just takes the index (0, 1, 2...)
			glBindTextureUnit(uiUnit, uiTextureID);
		}
		else
		{
			// Legacy: Needs the GL_TEXTURE0 + index enum
			glActiveTexture(GL_TEXTURE0 + uiUnit);
			glBindTexture(eTexTarget, uiTextureID);
		}
		m_mEngineTextures[uiUnit] = uiTextureID;
		m_mEngineTargets[uiUnit] = eTexTarget;
	}
}

/**
 * @brief Binds the specified Vertex Array Object (VAO).
 *
 * This function checks if the specified VAO is already bound.
 * If not, it calls glBindVertexArray to switch to the new VAO,
 * minimizing redundant state changes.
 *
 * @param uiVAO The OpenGL Vertex Array Object ID to bind.
 */
void CStateManager::BindVertexArray(GLuint uiVAO)
{
	if (m_gCurState.uiCurrentVAO != uiVAO)
	{
		glBindVertexArray(uiVAO);
		m_gCurState.uiCurrentVAO = uiVAO;
	}
}

/**
 * @brief Binds a Uniform Buffer Object (UBO) to a specified binding point.
 *
 * This function checks if the UBO at the given binding point is already bound.
 * If not, it calls glBindBufferBase to bind the specified UBO, minimizing
 * redundant state changes.
 *
 * @param uiBindingPoint The binding point index for the UBO.
 * @param uiBufferID The OpenGL buffer ID of the UBO to bind.
 */
void CStateManager::BindUniformBuffer(GLuint uiBindingPoint, GLuint uiBufferID)
{
	// 1. Safety check for binding point range
	if (uiBindingPoint >= MAX_UBO_BINDINGS)
	{
		return;
	}

	// 2. Check against currently bound UBO at this binding point
	if (m_mBoundUBO[uiBindingPoint] != uiBufferID)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, uiBindingPoint, uiBufferID);
		m_mBoundUBO[uiBindingPoint] = uiBufferID;
	}
}

/**
 * @brief Binds a Uniform Buffer Object (UBO) to a specified binding point.
 *
 * This function checks if the UBO at the given binding point is already bound.
 * If not, it calls glBindBufferBase to bind the specified UBO, minimizing
 * redundant state changes.
 *
 * @param uiBindingPoint The binding point index for the UBO.
 * @param uiBufferID The OpenGL buffer ID of the UBO to bind.
 */
void CStateManager::BindUniformBuffer(EUniformBufferBindingPoints eBindingPoint, GLuint uiBufferID)
{
	// 1. Safety check for binding point range
	if (eBindingPoint >= EUniformBufferBindingPoints::UBO_BP_MAX_NUM)
	{
		return;
	}

	// 2. Check against currently bound UBO at this binding point
	if (m_mBoundUBO[static_cast<GLuint>(eBindingPoint)] != uiBufferID)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(eBindingPoint), uiBufferID);

		// for future use, if we want to bind a range instead of the whole buffer
		// glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(eBindingPoint), uiBufferID, offset, size);

		m_mBoundUBO[static_cast<GLuint>(eBindingPoint)] = uiBufferID;
	}
}

/**
 * @brief Uses the specified shader program.
 *
 * This function checks if the specified shader program is already in use.
 * If not, it calls glUseProgram to switch to the new program, minimizing
 * redundant state changes.
 *
 * @param uiProgramID The OpenGL shader program ID to use.
 */
bool CStateManager::UseProgram(GLuint uiProgramID)
{
	if (m_gCurState.uiCurrentProgramID != uiProgramID)
	{
		glUseProgram(uiProgramID);
		m_gCurState.uiCurrentProgramID = uiProgramID;
		return true; // We actually touched the GPU
	}

	return false; // No change needed
}

/**
 * @brief Binds the specified shader.
 *
 * This function checks if the specified shader is already bound.
 * If not, it calls glUseProgram to switch to the new shader program,
 * minimizing redundant state changes.
 *
 * @param pShader Pointer to the CShader object to bind.
 * @return Pointer to the currently bound CShader object.
 */
CShader* CStateManager::BindShader(CShader* pShader)
{
	if (!pShader)
	{
		syserr("called with null shader!");
		return nullptr;
	}

	GLuint uiShaderID = pShader->GetProgramID();
	if (m_gCurState.uiCurrentProgramID != uiShaderID)
	{
		glUseProgram(uiShaderID);
		m_gCurState.pCurrentShader = pShader;
		m_gCurState.uiCurrentProgramID = uiShaderID;
	}

	return m_gCurState.pCurrentShader;
}

void CStateManager::SetInterfaceRenderState(const Matrix4& mat4Projection, const Vector2D& v2Resolution)
{
	// 1. Apply to Shader
	m_gCurState.pCurrentShader->SetMat4("u_matProjection", mat4Projection);

	// 2. Set standard UI states
	auto& state = CStateManager::Instance();
	state.SetEngineCapability(GL_BLEND, true);
	state.SetEngineCapability(GL_DEPTH_TEST, false);
	state.SetEngineCapability(GL_CULL_FACE, true);
	SetViewport(0, 0, (GLsizei)v2Resolution.width, (GLsizei)v2Resolution.height);
	SetScissorRect(0, 0, (GLsizei)v2Resolution.width, (GLsizei)v2Resolution.height);

	state.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/**
 * @brief Pushes the current state onto the state stack.
 *
 * This function saves the current OpenGL state snapshot
 * onto an internal stack, allowing for later restoration
 * of the state using PopState().
 */
void CStateManager::PushState()
{
	m_stateStack.push(m_gCurState);
}

/**
 * @brief Pops the last saved state from the state stack and restores it.
 *
 * This function retrieves the most recently saved OpenGL state snapshot
 * from the internal stack and restores all relevant states, including
 * capabilities, shader program, VAO, viewport, scissor rectangle, and
 * blending function.
 */
void CStateManager::PopState()
{
	if (m_stateStack.empty())
	{
		syserr("StateManager PopState called on empty stack!");
		return;
	}

	StateSnapshot previous = m_stateStack.top();
	m_stateStack.pop();

	// 1. Restore capabilities (Depth, Blend, Scissor test toggles)
	for (const auto& [eCap, bEnabled] : previous.m_mEngineCapabilities)
	{
		SetEngineCapability(eCap, bEnabled);
	}
	// 2. Restore Program and VAO
	if (previous.pCurrentShader)
	{
		BindShader(previous.pCurrentShader);
	}
	else
	{
		glUseProgram(0); // Safety if no shader was bound
		m_gCurState.uiCurrentProgramID = 0;
		m_gCurState.pCurrentShader = nullptr;
	}
	BindVertexArray(previous.uiCurrentVAO);

	// 3. Restore UI Specifics
	SetViewport(previous.viewport[0], previous.viewport[1], previous.viewport[2], previous.viewport[3]);
	SetScissorRect(previous.scissorBox[0], previous.scissorBox[1], previous.scissorBox[2], previous.scissorBox[3]);
	SetBlendFunc(previous.blendSrc, previous.blendDst);
}
