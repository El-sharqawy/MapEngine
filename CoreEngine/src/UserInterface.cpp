#include "UserInterface.h"
#include "WindowManager.h"
#include "ShadersManager.h"

#if defined(_WIN64)
#undef min
#undef max
#undef minmax
#endif

#define ENGINE_VERSION "Map Engine v1.0.0"

/**
 * @brief Initializes Variables and ImGui context and configures settings.
 *
 * Sets up ImGui with default configurations, styles, and
 * platform/renderer bindings for OpenGL and GLFW.
 */
void CUserInterface::Initialize()
{
	m_bShowDemoWindow = false;
	m_bShadersDirty = true;

	InitializeImGui();
}

/**
 * @brief Initializes the ImGui context and configures settings.
 *
 * This function sets up the ImGui library by creating a new context,
 * configuring input options, applying a dark style, and initializing
 * platform and renderer bindings for OpenGL and GLFW.
 */
void CUserInterface::InitializeImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark(); // Could be Light Too

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::GetStyle().FramePadding = ImVec2(8, 6); // Padding for menu items
	style.FramePadding = ImVec2(8, 6); // permanent global change, no push/pop

	ImGui_ImplGlfw_InitForOpenGL(CWindowManager::Instance().GetGLWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

/**
 * @brief Cleans up ImGui resources.
 *
 * Shuts down ImGui and releases all allocated resources.
 */
void CUserInterface::Destroy()
{
	// Check if the ImGui context exists before attempting to destroy it.
	// If the context is null, it means we have already shut down.
	if (ImGui::GetCurrentContext() == nullptr)
	{
		return;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

 /**
  * @brief Starts a new ImGui frame.
  *
  * Prepares ImGui for rendering a new frame by setting up
  * necessary state and calling ImGui's NewFrame functions and
  * Finalizes the ImGui frame and issues draw calls to render
  * the GUI elements to the screen.
  */
void CUserInterface::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (m_bShowDemoWindow)
	{
		ImGui::ShowDemoWindow(&m_bShowDemoWindow);
	}

	RenderMainMenuBar();
	RenderEngineMainUI();

	CMapManager::Instance().RenderCompass();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	/*if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}*/
}

/**
 * @brief Renders the main menu bar.
 *
 * Creates and displays the main menu bar with options
 * such as File, Edit, and View.
 */
void CUserInterface::RenderMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project"))
			{
				/* Action */
			}

			if (ImGui::MenuItem("Open...", "Ctrl+O"))
			{
				/* Action */
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				glfwSetWindowShouldClose(CWindowManager::Instance().GetGLWindow(), true);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
			{
				/* Action */
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false))
			{
				// Disabled
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

/**
 * @brief Renders the main engine UI panels.
 *
 * Displays various engine-related UI components,
 * including data, shaders, and textures panels.
 */
void CUserInterface::RenderEngineMainUI()
{
	ImGui::Begin("UI Tools");
	if (ImGui::BeginTabBar("##MainEditorTabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Engine"))
		{
			RenderEngineDataUI();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Shaders"))
		{
			RenderEngineShadersUI();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::NewLine();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

/**
 * @brief Renders the engine data UI panel.
 *
 * Displays information about the engine version,
 * build date, and build time.
 */
void CUserInterface::RenderEngineDataUI()
{
	ImGui::Text("Engine Version: %s", ENGINE_VERSION);
	ImGui::Text("Build Date: %s", __DATE__);
	ImGui::Text("Build Time: %s", __TIME__);
	ImGui::Separator();

	static bool bIsPicking = CMapManager::Instance().IsPicking();

	if (ImGui::Button("Measure Distance"))
	{
		CMapManager::Instance().StartPicking();   // sets m_bPickingPoint = true, resets count
	}

	if (CMapManager::Instance().GetPickedCount() == 1)
	{
		ImGui::TextColored({ 1,1,0,1 }, "Click second point...");
	}

	if (CMapManager::Instance().GetDistance() > 0.0f)
	{
		ImGui::Text("Distance: %.2f km", CMapManager::Instance().GetDistance());
	}
}

/**
 * @brief Renders the engine shaders UI panel.
 *
 * Displays a list of loaded shaders and their details.
 */
void CUserInterface::RenderEngineShadersUI()
{
	// ONLY rebuild the list if something changed (Dirty Flag)
	if (AreShadersDirty())
	{
		m_mCachedShaderNames.clear();
		const auto& mLoadedShaders = CShadersManager::Instance().GetShaders();

		// Reserve memory to avoid multiple reallocations during the loop
		m_mCachedShaderNames.reserve(mLoadedShaders.size());

		for (const auto& [shaderName, shaderInfo] : mLoadedShaders)
		{
			m_mCachedShaderNames.push_back(shaderName.c_str());
		}
		SetShadersDirty(false);
	}

	static GLint selectedShader = -1;
	ImGui::NewLine();
	ImGui::Text("Loaded Shaders:");
	ImGui::PushItemWidth(200.0f); // Set width to 200 pixels
	ImGui::ListBox("##shaders_list", &selectedShader, m_mCachedShaderNames.data(), static_cast<int32_t>(m_mCachedShaderNames.size()), 7);
	ImGui::PopItemWidth(); // Restore default item width

	if (ImGui::Button("Reload Shaders", ImVec2(125, 25)))
	{
		CShadersManager::Instance().ReloadAllShaders();
		SetShadersDirty(true); // Mark as dirty to rebuild the list
	}
}
