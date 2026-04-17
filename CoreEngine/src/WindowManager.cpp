#include "WindowManager.h"

CWindowManager::~CWindowManager()
{
	Destroy();
}

void CWindowManager::Destroy()
{
	// 1. Shutdown Renderer Manager
	CRendererManager::Instance().Destroy();

	// 2. Shutdown Debug Manager
	CDebugRenderer::Instance().Destroy();

	// 4. Shutdown Shaders Manager
	CShadersManager::Instance().Destroy();

	// 5. Shutdown State Manager
	CStateManager::Instance().Destroy();

	// 3. Shutdown Camera Manager
	CCameraManager::Instance().Destroy();

	// 6. Shutdown Log Manager
	CLogManager::Instance().Destroy();

	if (m_pGLWindow != nullptr)
	{
		glfwDestroyWindow(m_pGLWindow);
	}
}

bool CWindowManager::InitializeWindow(const std::string& stWindowTitle)
{
	syslog("We are all alone on life's journey, held captive by the limitations of human consciousness.");

	if (glfwInit() == false)
	{
		syserr("Failed to Initialize GLFW");
		return (false);
	}

	// Setup OpenGL Version (4.6) and Enable core profile (Delete Deperecated functions)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // 6
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // GLFW_OPENGL_ANY_PROFILE, exclude legacy code

	// Enable Multi Sampling, for a smoother drawing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Enable Context Debugging for OpenGL
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	// More Window Flags
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // Make it with "title bar" and "minimize, exit" buttons
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Make it inVisible, later must call "glfwShowWindow"
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE); // Focus the window once we show it

	// Get Monitor Data
	m_pGLMonitor = glfwGetPrimaryMonitor();
	m_pGLVideoMode = glfwGetVideoMode(m_pGLMonitor);

	// Setup Colors & Refresh rate based on our monitor
	glfwWindowHint(GLFW_RED_BITS, m_pGLVideoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, m_pGLVideoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, m_pGLVideoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, m_pGLVideoMode->refreshRate);

	// Setup Window Size & Deminsions
	m_iFullScreenWidth = m_pGLVideoMode->width;		// Full monitor width
	m_iFullScreenHeight = m_pGLVideoMode->height;	// Full monitor height

	// Windowed size is 75% of full screen size!
	m_iWindowedWidth = (m_iFullScreenWidth * 75) / 100;
	m_iWindowedHeight = (m_iFullScreenHeight * 75) / 100;

	// Create Window
	if (GetWindowMode() == EWindowMode::MODE_WINDOWED)
	{
		m_iWidth = m_iWindowedWidth;
		m_iHeight = m_iWindowedHeight;

		m_pGLWindow = glfwCreateWindow(m_iWidth, m_iHeight, stWindowTitle.c_str(), nullptr, nullptr);
	}
	else if (GetWindowMode() == EWindowMode::MODE_FULLSCREEN)
	{
		m_iWidth = m_iFullScreenWidth;
		m_iHeight = m_iFullScreenHeight;

		m_pGLWindow = glfwCreateWindow(m_iWidth, m_iHeight, stWindowTitle.c_str(), m_pGLMonitor, nullptr);
	}

	if (m_pGLWindow == nullptr)
	{
		syserr("Failed to Initialize GLFW Window");
		return (false);
	}

	if (GetWindowMode() == EWindowMode::MODE_WINDOWED)
	{
		glfwSetWindowPos(m_pGLWindow, (m_iFullScreenWidth - m_iWidth) / 2, (m_iFullScreenHeight - m_iHeight) / 2);
	}

	// Make window context
	glfwMakeContextCurrent(GetGLWindow());

	// Disable VSync (0 = Disabled, 1 = Enabled)
	glfwSwapInterval(GLFW_FALSE);

	// Set Current Window Pointer
	glfwSetWindowUserPointer(GetGLWindow(), this);

	// Initialize GLAD library
	if (gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)) == false)
	{
		syserr("Failed to Initialize GLAD Library");
		return (false);
	}

	// Functions Callbacks
	glfwSetFramebufferSizeCallback(GetGLWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(GetGLWindow(), mouse_callback);
	glfwSetScrollCallback(GetGLWindow(), scroll_callback);
	glfwSetKeyCallback(GetGLWindow(), keys_callback);
	glfwSetMouseButtonCallback(GetGLWindow(), mouse_button_callback);
	glfwSetCursorPos(GetGLWindow(), static_cast<double>(m_iWidth) / 2, static_cast<double>(m_iHeight) / 2);

	// Show our window
	glfwShowWindow(GetGLWindow());

#if defined(_WIN32) || defined(_WIN64)
	// timeBeginPeriod(1); // Set system timer resolution to 1ms
#endif

	// Initialize Systems
	InitializeSubSystems();

	return (true);
}

EWindowMode CWindowManager::GetWindowMode() const
{
	return (m_eWindowMode);
}

GLFWwindow* CWindowManager::GetGLWindow() const
{
	return (m_pGLWindow);
}

int32_t CWindowManager::GetWidth() const
{
	return m_iWidth;
}

int32_t CWindowManager::GetHeight() const
{
	return m_iHeight;
}

float CWindowManager::GetWidthF() const
{
	return static_cast<float>(GetWidth());
}

float CWindowManager::GetHeightF() const
{
	return static_cast<float>(GetHeight());
}

void CWindowManager::Update()
{
	while (glfwWindowShouldClose(GetGLWindow()) == false)
	{
		CTimerManager::Instance().Update();
		float dt = CTimerManager::Instance().GetDeltaTimeF();

		// 0. OS events
		glfwPollEvents();

		// 1. Gameplay / high-level input processing
		ProcessInput(dt);
		
		// 2. Input state
		CInputManager::Instance().Update(dt);		// finalize per-frame key/mouse state

		// 3. Update Camera
		CCameraManager::Instance().UpdateCameras();

		// 4. Clear Buffers
		SVector4Df fogColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		glClearColor(fogColor.x, fogColor.y, fogColor.z, fogColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 5. Renderer Manager (Draw all 3D objects)
		CRendererManager::Instance().Update(); // Draw Terrain, Meshes, etc.
		CDebugRenderer::Instance().RenderAll(); // Draw Lines on top of 3D

		// 6. End Frame
		glfwSwapBuffers(GetGLWindow());
	}

	// CleanUP
}

void CWindowManager::ProcessInput(float deltaTime)
{
	auto& inputMgr = CInputManager::Instance();
	auto& timerMgr = CTimerManager::Instance();
	auto& cameraMgr = CCameraManager::Instance();

	// Close The App
	if (inputMgr.IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		RequestShutdown();
	}

	// Toggle Windowed / FS
	if (inputMgr.IsKeyPressed(GLFW_KEY_F11))
	{
		if (GetWindowMode() == EWindowMode::MODE_WINDOWED)
		{
			SetWindowMode(EWindowMode::MODE_FULLSCREEN);
		}
		else
		{
			SetWindowMode(EWindowMode::MODE_WINDOWED);
		}
	}

	// Print some Data
	if (inputMgr.IsKeyPressed(GLFW_KEY_H))
	{
		syslog("DeltaTime: %f", timerMgr.GetDeltaTimeF());
		syslog("ElapsedTime: %f", timerMgr.GetElapsedTimeF());
		syslog("FPS: %f", timerMgr.GetFPSF());
	}

	// Camera Movement
	float DeltaTime = timerMgr.GetDeltaTimeF();
	CCamera* pCamera = cameraMgr.GetCurrentCamera();

	if (inputMgr.IsKeyDown(GLFW_KEY_W))
	{
		pCamera->ProcessKeyboard(ECameraDirections::DIRECTION_FORWARD, DeltaTime);
	}
	if (inputMgr.IsKeyDown(GLFW_KEY_S))
	{
		pCamera->ProcessKeyboard(ECameraDirections::DIRECTION_BACKWARD, DeltaTime);
	}
	if (inputMgr.IsKeyDown(GLFW_KEY_A))
	{
		pCamera->ProcessKeyboard(ECameraDirections::DIRECTION_LEFT, DeltaTime);
	}
	if (inputMgr.IsKeyDown(GLFW_KEY_D))
	{
		pCamera->ProcessKeyboard(ECameraDirections::DIRECTION_RIGHT, DeltaTime);
	}
}

void CWindowManager::RequestShutdown()
{
	// 1. Request GLFW to close the window -- 
	glfwSetWindowShouldClose(GetGLWindow(), true);
}

void CWindowManager::SetKeyboardKey(int32_t iKey, bool pressed)
{
	if (iKey < 0 || iKey > GLFW_KEY_LAST)
	{
		syserr("Invalid Input, key %d out of range", iKey);
		return;
	}

	CInputManager::Instance().OnKey(iKey, pressed);
}

void CWindowManager::SetMouseKey(int32_t iKey, bool pressed)
{
	if (iKey < 0 || iKey > GLFW_MOUSE_BUTTON_LAST)
	{
		syserr("Invalid Input, key %d out of range", iKey);
		return;
	}

	CInputManager::Instance().OnMouseButton(iKey, pressed);
}

void CWindowManager::SetMousePosition(float fX, float fY)
{
	CInputManager::Instance().OnMouseMove(Vector2D(fX, fY));
}

void CWindowManager::SetMouseScroll(float fMouseScrollVal)
{
	CInputManager::Instance().OnMouseScroll(fMouseScrollVal);
}

void CWindowManager::SetWindowMode(const EWindowMode& windowMode)
{
	m_eWindowMode = windowMode;
	if (windowMode == EWindowMode::MODE_WINDOWED)
	{
		ResizeWindow(m_iWindowedWidth, m_iWindowedHeight);

		glfwSetWindowMonitor(GetGLWindow(), nullptr, 0, 0, m_iWidth, m_iHeight, m_pGLVideoMode->refreshRate);
		glfwSetWindowPos(GetGLWindow(), (m_iFullScreenWidth - m_iWidth) / 2, (m_iFullScreenHeight - m_iHeight) / 2);
	}
	else if (windowMode == EWindowMode::MODE_FULLSCREEN)
	{
		ResizeWindow(m_iFullScreenWidth, m_iFullScreenHeight);
		glfwSetWindowMonitor(GetGLWindow(), m_pGLMonitor, 0, 0, m_iWidth, m_iHeight, m_pGLVideoMode->refreshRate);
		glfwSetWindowPos(GetGLWindow(), 0, 0);
	}
}

void CWindowManager::ResizeWindow(int32_t iWidth, int32_t iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, iWidth, iHeight);
}

void CWindowManager::InitializeSubSystems()
{
	// 0. Initialize OpenGL Debugging (if in debug mode)
	CStateManager::Instance().Initialize();

	// 1. Initialize Shaders Manager
	CShadersManager::Instance().Initialize();

	// 2. Initialize Camera Manager
	CCameraManager::Instance().Initialize();

	// 3. Initialize Debug Renderer
	CDebugRenderer::Instance().Initialize();

	// 4. Initialize Renderer Manager
	CRendererManager::Instance().Initialize();
}

void CWindowManager::framebuffer_size_callback(GLFWwindow* window, GLint width, GLint height)
{
	// Store the raw window pointer.
	CWindowManager* appWindow = (CWindowManager*)glfwGetWindowUserPointer(window);
	if (!appWindow)
	{
		return;
	}

	appWindow->ResizeWindow(width, height);
}

void CWindowManager::mouse_callback(GLFWwindow* window, GLdouble xpos, GLdouble ypos)
{
	// Store the raw window pointer.
	CWindowManager* appWindow = (CWindowManager*)glfwGetWindowUserPointer(window);
	if (!appWindow)
	{
		return;
	}

	appWindow->SetMousePosition(static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
}

void CWindowManager::scroll_callback(GLFWwindow* window, GLdouble xoffset, GLdouble yoffset)
{
	// Store the raw window pointer.
	CWindowManager* appWindow = (CWindowManager*)glfwGetWindowUserPointer(window);
	if (!appWindow)
	{
		return;
	}

	appWindow->SetMouseScroll(static_cast<GLfloat>(yoffset));
}

void CWindowManager::keys_callback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
{
	// Store the raw window pointer.
	CWindowManager* appWindow = (CWindowManager*)glfwGetWindowUserPointer(window);
	if (!appWindow)
	{
		return;
	}

	appWindow->SetKeyboardKey(key, action);
}

void CWindowManager::mouse_button_callback(GLFWwindow* window, GLint button, GLint action, GLint mods)
{
	// Store the raw window pointer.
	CWindowManager* appWindow = (CWindowManager*)glfwGetWindowUserPointer(window);
	if (!appWindow)
	{
		return;
	}

	appWindow->SetMouseKey(button, action);
}
