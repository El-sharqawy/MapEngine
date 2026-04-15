#pragma once

#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h> // <<--- Here we go
#pragma comment(lib, "winmm.lib")
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "AnubisEnums.h"
#include "InputManager.h"
#include "TimerManager.h"

#if defined(_WIN32)
#define syserr(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); } while(0)
#define syslog(...) do { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); fflush(stdout); } while(0)
#elif defined(__linux__)
#define syserr(...) do { fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); } while(0)
#define syslog(...) do { fprintf(stdout, ##__VA_ARGS__); fprintf(stdout, "\n"); fflush(stdout); } while(0)
#endif

class CWindow
{
public:
	CWindow() = default;
	~CWindow();

	void Destroy();

	bool InitializeWindow(const std::string& stWindowTitle);

	EWindowMode GetWindowMode() const;
	GLFWwindow* GetGLWindow() const;

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

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	static void framebuffer_size_callback(GLFWwindow* window, GLint width, GLint height);
	static void mouse_callback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
	static void scroll_callback(GLFWwindow* window, GLdouble xoffset, GLdouble yoffset);
	static void keys_callback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods);
	static void mouse_button_callback(GLFWwindow* window, GLint button, GLint action, GLint mods);

protected:
	// Class Sigleton Members
	CTimerManager timer_manager;
	CInputManager input_manager;
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
};