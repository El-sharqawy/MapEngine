#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "AnubisEnums.h"
#include "Camera.h"

// singleton classes
#include <Singleton.h>
#include "TimerManager.h"
#include "LogManager.h"
#include "InputManager.h"
#include "ShadersManager.h"
#include "CameraManager.h"
#include "StateManager.h"

#include "DebugRenderer.h"
#include "RendererManager.h"

class CWindowManager: public CSingleton<CWindowManager>
{
public:
	CWindowManager() = default;
	~CWindowManager();

	void Destroy();

	bool InitializeWindow(const std::string& stWindowTitle);

	EWindowMode GetWindowMode() const;
	GLFWwindow* GetGLWindow() const;

	int32_t GetWidth() const;
	int32_t GetHeight() const;
	float GetWidthF() const;
	float GetHeightF() const;

	void Update();
	void ProcessInput(float deltaTime);
	void RequestShutdown();

	// User Input
	void SetKeyboardKey(int32_t iKey, bool pressed);
	void SetMouseKey(int32_t iKey, bool pressed);
	void SetMousePosition(float fX, float fY);
	void SetMouseScroll(float fMouseScrollVal);

	void SetWindowMode(const EWindowMode& windowMode);

protected: // protected static GLFW functions

	void ResizeWindow(int32_t iWidth, int32_t iHeight);
	void InitializeSubSystems();

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	static void framebuffer_size_callback(GLFWwindow* window, GLint width, GLint height);
	static void mouse_callback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
	static void scroll_callback(GLFWwindow* window, GLdouble xoffset, GLdouble yoffset);
	static void keys_callback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods);
	static void mouse_button_callback(GLFWwindow* window, GLint button, GLint action, GLint mods);

protected:
	// Class Sigleton Members
	CTimerManager timer_manager;
	CLogManager log_manager;
	CInputManager input_manager;
	CStateManager StateManager;

	// Renderer
	CCameraManager CameraManager;
	CShadersManager ShadersManager;
	CDebugRenderer DebugRender;
	CRendererManager RendererManager;

private:
	GLFWwindow* m_pGLWindow = nullptr;
	GLFWmonitor* m_pGLMonitor = nullptr;
	const GLFWvidmode* m_pGLVideoMode = nullptr;

	// Window size
	int32_t m_iWidth = 0;
	int32_t m_iHeight = 0;

	int32_t m_iFullScreenWidth = 0;
	int32_t m_iFullScreenHeight = 0;

	int32_t m_iWindowedWidth = 0;
	int32_t m_iWindowedHeight = 0;

	EWindowMode m_eWindowMode = EWindowMode::MODE_WINDOWED;

	std::unique_ptr<CCamera> m_pCamera = nullptr;
};