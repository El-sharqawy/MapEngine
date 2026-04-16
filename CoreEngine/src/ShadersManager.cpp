#include "ShadersManager.h"
#include "Shader.h"
#include "AnubisDefines.h"
#include "LogManager.h"
#include "CoreUtils.h"
#include <nlohmann/json.hpp>

void CShadersManager::Initialize()
{
	std::fstream fileStream(SHADERS_PATH);
	if (!fileStream.is_open())
	{
		syserr("Failed to open textures configuration file: {}", SHADERS_PATH);
		syslog("Attemp to recreate one");

		nlohmann::json jsonConfig;
		jsonConfig["shaders"] = nlohmann::json::array();

		// Create a default shaders configuration file
		nlohmann::json shader;
		shader["name"] = "BasicRenderer";
		shader["vertex_path"] = "Assets/Shaders/Renderer/basic_renderer.vert";
		shader["fragment_path"] = "Assets/Shaders/Renderer/basic_renderer.frag";
		jsonConfig["shaders"].push_back(shader);

		// Create a default debuggin shaders
		shader["name"] = "DebuggingRenderer";
		shader["vertex_path"] = "Assets/Shaders/Renderer/debug_renderer.vert";
		shader["fragment_path"] = "Assets/Shaders/Renderer/debug_renderer.frag";
		jsonConfig["shaders"].push_back(shader);

		std::ofstream outFile(SHADERS_PATH, std::ios::out);
		if (outFile.is_open())
		{
			outFile << jsonConfig.dump(4); // Pretty print with 4 spaces indentation
			outFile.flush();
			outFile.close();
			syslog("Created default shaders configuration file: {}", SHADERS_PATH);
		}
		else
		{
			syserr("Failed to create default shaders configuration file: {}", SHADERS_PATH);
		}
	}

	m_pErrorShader = new CShader();
	m_pErrorShader->InitializeShader();
	m_pErrorShader->AttachShader("Assets/Shaders/Errors/error_shader.vert");
	m_pErrorShader->AttachShader("Assets/Shaders/Errors/error_shader.frag");
	m_pErrorShader->LinkProgram();

	// Load shader configuration from a file or hardcode some entries here
	LoadShaders(SHADERS_PATH);
}

void CShadersManager::Destroy()
{
	for (auto& pair : m_mShaderRegistry)
	{
		if (pair.second.pShader)
		{
			Anubis::SafeDelete(pair.second.pShader);
		}
	}

	Anubis::SafeDelete(m_pErrorShader);
}

bool CShadersManager::LoadShaders(const std::string& strShadersFile)
{
	std::fstream fileStream(strShadersFile, std::ios::in);
	if (!fileStream.is_open())
	{
		syserr("Failed to open shaders configuration file: {}", strShadersFile);
		return (false);
	}

	try
	{
		nlohmann::json jsonConfig;
		try
		{
			fileStream >> jsonConfig;
		}
		catch (const std::exception& e)
		{
			syserr("Failed to parse shaders configuration file: {}. Error: {}", strShadersFile, e.what());
			fileStream.close();
			return false;
		}

		// 1. Validation
		if (!jsonConfig.contains("shaders") || !jsonConfig["shaders"].is_array())
		{
			// logger.error("Invalid texture file format");
			syserr("JSON parse error for File {}, error: Invalid texture file format", strShadersFile);
			return (false);
		}

		// 2. Getting the count (Corrected)
		// Use the jsonConfig object and access the key "shaders"
		size_t shaders_count = jsonConfig["shaders"].size(); // remove Eraser

		syslog("Loading {} shaders from configuration file: {}", shaders_count, strShadersFile);

		// 3. Iterating and Registering
		for (const auto& shader : jsonConfig["shaders"])
		{
			// Ensure the item has the required fields before using them
			if (shader.contains("name") && shader.contains("vertex_path") && shader.contains("fragment_path"))
			{
				SShaderResource shaderResource{};
				shaderResource.name = shader["name"];
				shaderResource.vertexPath = shader["vertex_path"];
				shaderResource.fragmentPath = shader["fragment_path"];
				RegisterShader(shaderResource);
			}
		}
		return (true);
	}
	catch (nlohmann::json::parse_error& e)
	{
		syserr("JSON Parse Error: {}", e.what());
		return (false);
	}
	catch (nlohmann::json::type_error& e)
	{
		syserr("JSON Type Error (Wrong data format): {}", e.what());
		return (false);
	}

	return (false);
}

void CShadersManager::RegisterShader(const SShaderResource& shaderResource)
{
	auto res = shaderResource;
	if (std::filesystem::exists(res.vertexPath))
	{
		res.vertTime = std::filesystem::last_write_time(res.vertexPath);
	}
	if (std::filesystem::exists(res.fragmentPath))
	{
		res.fragTime = std::filesystem::last_write_time(res.fragmentPath);
	}

	m_mShaderRegistry[shaderResource.name] = shaderResource;
}

CShader* CShadersManager::GetShader(const std::string& shaderName)
{
	// 1. Look up the entry in our registry
	auto it = m_mShaderRegistry.find(shaderName);
	if (it == m_mShaderRegistry.end())
	{
		syserr("Shader '{}' not found in registry.", shaderName);
		return (m_pErrorShader);
	}

	// 2. If it's already compiled/loaded, return it immediately (The Cache)
	if (it->second.pShader != nullptr)
	{
		return (it->second.pShader);
	}

	// 3. If it's NOT loaded, compile it now (On-Demand Loading)
#if defined(ENABLE_DEBUG_LOGS)
	syslog("Compiling shader '{}' for the first time...", shaderName);
#endif

	CShader* newShader = new CShader();
	newShader->InitializeShader();
	if (!newShader->AttachShader(it->second.vertexPath))
	{
		syserr("Failed to attach vertex shader for '{}'.", shaderName);
		Anubis::SafeDelete(newShader);
		return (m_pErrorShader);
	}
	if (!newShader->AttachShader(it->second.fragmentPath))
	{
		syserr("Failed to attach fragment shader for '{}'.", shaderName);
		Anubis::SafeDelete(newShader);
		return (m_pErrorShader);
	}
	if (!newShader->LinkProgram())
	{
		syserr("Failed to link shader program for '{}'.", shaderName);
		Anubis::SafeDelete(newShader);
		return (m_pErrorShader);
	}

#if defined(ENABLE_DEBUG_LOGS)
	syslog("Shader '{}' compiled and linked successfully.", shaderName);
#endif
	// 4. Cache it for future use
	it->second.pShader = newShader;
	return newShader;
}

const std::unordered_map<std::string, SShaderResource>& CShadersManager::GetShaders() const
{
	return (m_mShaderRegistry);
}

std::unordered_map<std::string, SShaderResource>& CShadersManager::GetShaders()
{
	return (m_mShaderRegistry);
}
void CShadersManager::ReloadAllShaders()
{
#if defined(ENABLE_SHADERS_LOGS)
	syslog("Clearing Shaders Cache: Deleting compiled programs...");
#endif

	uint32_t count = 0;
	for (auto& [name, resource] : m_mShaderRegistry)
	{
		if (resource.pShader != nullptr)
		{
			// 1. Check if it's the error shader to avoid double-deletion 
			// (assuming m_pErrorShader is managed elsewhere or is a static resource)
			if (resource.pShader == m_pErrorShader)
			{
				resource.pShader = nullptr;
				continue;
			}

			// 2. Properly destroy the shader (this should call glDeleteProgram internally)
			// If your CShader class has a Destroy/Cleanup method, call it here.
			// newShader->InitializeShader() was called in GetShader, so we must clean up.
			Anubis::SafeDelete(resource.pShader);

			// 3. Reset the pointer so GetShader knows it needs to be recompiled
			resource.pShader = nullptr;
			count++;
		}
	}

#if defined(ENABLE_SHADERS_LOGS)
	syslog("Successfully cleared {} compiled shaders. They will re-compile on next use.", count);
#endif
}

void CShadersManager::UpdateHotReload()
{
	for (auto& [name, resource] : m_mShaderRegistry)
	{
		bool bNeedsReload = false;

		std::filesystem::file_time_type newVertTime;
		std::filesystem::file_time_type newFragTime;

		// Check vertex shader file
		if (std::filesystem::exists(resource.vertexPath))
		{
			newVertTime = std::filesystem::last_write_time(resource.vertexPath);
		}

		// Check fragment shader file
		if (std::filesystem::exists(resource.fragmentPath))
		{
			newFragTime = std::filesystem::last_write_time(resource.fragmentPath);
		}

		// Compare modification times
		if (newVertTime != std::filesystem::file_time_type{} && newVertTime > resource.vertTime)
		{
			bNeedsReload = true;
			resource.vertTime = newVertTime;
		}
		if (newFragTime != std::filesystem::file_time_type{} && newFragTime > resource.fragTime)
		{
			bNeedsReload = true;
			resource.fragTime = newFragTime;
		}

		// No changes detected, skip to next shader
		if (!bNeedsReload)
		{
			continue;
		}

		// Reload the shader program
		ReloadShaderProgram(name, resource);
	}
}

void CShadersManager::ReloadShaderProgram(const std::string& name, SShaderResource& res)
{
	syslog("Reloading shader program: {}", name.c_str());

	// Keep a pointer to the old shader in case build fail
	CShader* oldShader = res.pShader;

	CShader* pNewShader = new CShader(name);
	pNewShader->InitializeShader();
	if (!pNewShader->AttachShader(res.vertexPath))
	{
		syserr("Failed to attach vertex shader for '{}'. Keeping the old shader.", name.c_str());
		Anubis::SafeDelete(pNewShader);
		return;
	}

	if (!pNewShader->AttachShader(res.fragmentPath))
	{
		syserr("Failed to attach fragment shader for '{}'. Keeping the old shader.", name.c_str());
		Anubis::SafeDelete(pNewShader);
		return;
	}

	if (!pNewShader->LinkProgram())
	{
		syserr("Failed to link shader program for '{}'. Keeping the old shader.", name.c_str());
		Anubis::SafeDelete(pNewShader);
		return;
	}

	// If we reach here, the new shader is successfully compiled and linked, swap it in
	res.pShader = pNewShader;

	// Delete the old shader
	Anubis::SafeDelete(oldShader);
}

