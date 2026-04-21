#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../../LibImageUI/Stdafx.h"
#include <singleton.h>

#define arr_size(arr) (sizeof(arr) / sizeof(arr[0]))

class CUserInterface : public CSingleton<CUserInterface>
{
public:
	/**
	 * @brief Default constructor for CUserInterface.
	 *
	 * Initializes member variables to default states.
	 */
	CUserInterface() = default;

	/**
	 * @brief Destructor that cleans up ImGui resources.
	 */
	~CUserInterface() = default;

	/**
	 * @brief Initializes Variables and ImGui context and configures settings.
	 *
	 * Sets up ImGui with default configurations, styles, and
	 * platform/renderer bindings for OpenGL and GLFW.
	 */
	void Initialize();

	/**
	 * @brief Initializes the ImGui context and configures settings.
	 *
	 * This function sets up the ImGui library by creating a new context,
	 * configuring input options, applying a dark style, and initializing
	 * platform and renderer bindings for OpenGL and GLFW.
	 */
	void InitializeImGui();

	/**
	 * @brief Cleans up ImGui resources.
	 *
	 * Shuts down ImGui and releases all allocated resources.
	 */
	void Destroy();

	/**
	 * @brief Starts a new ImGui frame.
	 *
	 * Prepares ImGui for rendering a new frame by setting up
	 * necessary state and calling ImGui's NewFrame functions and
	 * Finalizes the ImGui frame and issues draw calls to render
	 * the GUI elements to the screen.
	 */
	void Render();

	/**
	 * @brief Renders the main menu bar.
	 *
	 * Creates and displays the main menu bar with options
	 * such as File, Edit, and View.
	 */
	void RenderMainMenuBar();

	/**
	 * @brief Renders the main engine UI panels.
	 *
	 * Displays various engine-related UI components,
	 * including data, shaders, and textures panels.
	 */
	void RenderEngineMainUI();

	/**
	 * @brief Renders the engine data UI panel.
	 *
	 * Displays information about the engine version,
	 * build date, and build time.
	 */
	void RenderEngineDataUI();

	/**
	 * @brief Renders the engine shaders UI panel.
	 *
	 * Displays a list of loaded shaders and their details.
	 */
	void RenderEngineShadersUI();


	bool IsDemoWindowShown() const { return m_bShowDemoWindow; }
	void SetShowDemoWindow(bool bShow) { m_bShowDemoWindow = bShow; }

	bool AreShadersDirty() const { return m_bShadersDirty; }
	void SetShadersDirty(bool bDirty) { m_bShadersDirty = bDirty; }

private:
	bool m_bShowDemoWindow = false;

	std::vector<const char*> m_mCachedShaderNames;
	bool m_bShadersDirty; // Start true to build the list the first time
};