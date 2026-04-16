#pragma once

/**
 * @file ShadersManager.h
 * @brief Manages loading, compiling, and caching of shader programs.
 */
#include <singleton.h>
#include <unordered_map>
#include <string>
#include <filesystem>

class CShader;

struct SShaderResource
{
	std::string name;
	std::string vertexPath;
	std::string fragmentPath;
	CShader* pShader = nullptr; // The actual compiled object

	std::filesystem::file_time_type vertTime{}; // Last modified time
	std::filesystem::file_time_type fragTime{}; // Last modified time

};

class CShadersManager : public CSingleton<CShadersManager>
{
public:
	CShadersManager() = default;
	~CShadersManager() = default;

	void Initialize();
	void Destroy();

	bool LoadShaders(const std::string& strShadersFile);
	void RegisterShader(const SShaderResource& shaderResource);

	CShader* GetShader(const std::string& strPath);

	const std::unordered_map<std::string, SShaderResource>& GetShaders() const;
	std::unordered_map<std::string, SShaderResource>& GetShaders();

	void ReloadAllShaders();
	void UpdateHotReload();
	void ReloadShaderProgram(const std::string& name, SShaderResource& res);

private:
	std::unordered_map<std::string, SShaderResource> m_mShaderRegistry;
	CShader* m_pErrorShader = nullptr;
};