#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <glad/glad.h>
#include <map>
#include <stack>
#include <Singleton.h>
#include "AnubisEnums.h"
#include "TypeVector2.h"
#include "TypeMatrix4.h"

static const GLint MAX_TEXTURE_UNITS = 32;
static const GLint MAX_CAPABILITIES = 16;
static const GLint MAX_UBO_BINDINGS = static_cast<GLint>(EUniformBufferBindingPoints::UBO_BP_MAX_NUM);

static const GLint MAX_VIEWPORTS = 4;
static const GLint MAX_SCISSORS = 4;

class CShader;

struct StateSnapshot
{
	// Basic States
	std::map<GLenum, bool> m_mEngineCapabilities; // Basic Toggle States (Enable/Disable)
	GLuint uiCurrentVAO;

	CShader* pCurrentShader;
	GLuint uiCurrentProgramID;

	// UI Specifics
	GLint scissorBox[MAX_SCISSORS];			// {x, y, width, height}
	GLint viewport[MAX_VIEWPORTS];         // {x, y, width, height}
	GLint blendSrc, blendDst;  // How transparency is calculated
};

class CStateManager : public CSingleton<CStateManager>
{
public:
	CStateManager() = default;
	~CStateManager() = default;

	void Initialize();
	void Destroy();

	void SetEngineCapability(GLenum eCap, bool bEnable);

	void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height);
	void SetBlendFunc(GLenum src, GLenum dst);

	void BindTexture(GLuint uiUnit, GLenum eTexTarget, GLuint uiTextureID);
	void BindVertexArray(GLuint uiVAO);
	void BindUniformBuffer(GLuint uiBindingPoint, GLuint uiBufferID);
	void BindUniformBuffer(EUniformBufferBindingPoints eBindingPoint, GLuint uiBufferID);
	CShader* BindShader(CShader* pShader);

	bool UseProgram(GLuint uiProgramID);
	void SetInterfaceRenderState(const Matrix4& mat4Projection, const Vector2D& v2Resolution);

	void PushState();
	void PopState();

private:
	// 1. The MASTER record (Everything goes here)
	StateSnapshot m_gCurState;			// The "Live" record of what the GPU is doing

	// 2. The Save Stack
	std::stack<StateSnapshot> m_stateStack;			// Saving & Restoring (The Stack)

	// 3. Texture Tracking (Independent of Push/Pop)
	GLuint m_mEngineTextures[MAX_TEXTURE_UNITS];
	GLenum m_mEngineTargets[MAX_TEXTURE_UNITS];
	GLuint m_mBoundUBO[MAX_UBO_BINDINGS];
	GLint m_iWindowWidth;
	GLint m_iWindowHeight;
};