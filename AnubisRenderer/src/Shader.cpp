#include "Stdafx.h"
#include "Shader.h"
#include "LogManager.h"
#include "CoreUtils.h"
#include "StateManager.h"
#include "GLUtils.h"

/**
 * Creates a new shader with given name.
 *
 * @param name Optional name for debugging (default: "Shader")
 */
CShader::CShader(const std::string& name)
{
	m_stName = name;
	m_uiProgramID = 0;
	m_bIsInitialized = false;
	m_bIsLinked = false;
	m_bIsCompute = false;
}

/**
 * Destructor - automatically cleans up OpenGL resources.
 */
CShader::~CShader()
{
	glDeleteProgram(m_uiProgramID);
}

/**
 * Initializes and creates a new shader program.
 */
void CShader::InitializeShader()
{
	if (m_bIsInitialized)
	{
		return;
	}

	m_uiProgramID = glCreateProgram();

	if (m_uiProgramID != 0)
	{
		m_bIsInitialized = true;
	}
}

/**
 * Attaches a shader from file to the program.
 * Shader type is automatically detected from file extension.
 *
 * @param stShaderPath Path to shader file (.vert, .frag, .geom, etc.)
 * @return true if successfully compiled and attached, false otherwise
 */
bool CShader::AttachShader(const std::string& stShaderPath)
{
	// Validate program state
	if (!m_bIsInitialized)
	{
		InitializeShader();
	}

	if (m_bIsLinked)
	{
		syserr("Cannot attach shader '{}': program '{}' already linked", GetShaderName(stShaderPath).c_str(), m_stName.c_str());
		return false;
	}

	if (m_bIsCompute)
	{
		// IDK YET
		return (false);
	}

	// Load shader source from file
	std::string shaderCode = LoadShaderFromFile(stShaderPath);
	if (shaderCode.empty())
	{
		syserr("Failed to load shader file: {}", stShaderPath.c_str());
		return (false);
	}

	// Determine shader type from file extension
	TShaderType shaderType = GetShaderType(stShaderPath);
	if (shaderType.m_uiType == 0)
	{
		syserr("Unknown shader type for file: {}", stShaderPath.c_str());
		return false;
	}

	// Compile shader
	GLuint uiShaderID = glCreateShader(shaderType.m_uiType);
	const char* shaderCodeStr = shaderCode.c_str();
	glShaderSource(uiShaderID, 1, &shaderCodeStr, nullptr);
	glCompileShader(uiShaderID);

	// Check for compilation errors
	if (CheckCompileErrors(uiShaderID, shaderType.m_stName, GetShaderName(stShaderPath)) == false)
	{
		glDeleteShader(uiShaderID);	
		return (false);
	}

	// Attach to program
	glAttachShader(m_uiProgramID, uiShaderID);
	m_vecShaders.push_back(uiShaderID);

	// syslog("Successfully Attached shader: {}", GetShaderName(stShaderPath).c_str());
	return (true);
}

/**
 * Links all attached shaders into a complete program.
 * Must be called after all shaders are attached and before Use().
 *
 * @return true if linking succeeded, false otherwise
 */
bool CShader::LinkProgram()
{
	if (!m_bIsInitialized)
	{
		syserr("Cannot link program {}: not initialized", m_stName);
		return (false);
	}

	if (m_bIsLinked)
	{
		syserr("Program {} is already linked", m_stName);
		return (false);
	}

	if (m_vecShaders.empty())
	{
		syserr("Cannot link program {}: no shaders attached", m_stName);
		return (false);
	}

	// Link the program
	glLinkProgram(m_uiProgramID);

	// Check for linking errors
	if (!CheckCompileErrors(m_uiProgramID, "program", m_stName))
	{
		syserr("Failed to link program {}", m_stName);
		return (false);
	}

	m_bIsLinked = true;
	// syslog("Program {} linked successfully", m_stName);

	// Clean up shader objects (no longer needed after linking)
	for (GLuint shaderID : m_vecShaders)
	{
		glDeleteShader(shaderID);
	}
	m_vecShaders.clear();

	PreCacheUniforms();

	return true;
}

/**
 * Pre-caches all active uniform locations into a map for quick access.
 */
void CShader::PreCacheUniforms()
{
	GLint numUniforms = 0;
	glGetProgramiv(m_uiProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);

	GLint maxNameLength = 0;
	glGetProgramiv(m_uiProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

	std::vector<GLchar> nameBuffer(maxNameLength);

	for (GLint i = 0; i < numUniforms; ++i)
	{
		GLsizei length;
		GLint size;
		GLenum type;

		// Get metadata for this specific active uniform
		glGetActiveUniform(m_uiProgramID, i, maxNameLength, &length, &size, &type, nameBuffer.data());

		std::string name(nameBuffer.data(), length);

		// Find the actual location ID
		GLint location = glGetUniformLocation(m_uiProgramID, name.c_str());

		// Populate our cache
		m_UniformLocationCache[name] = location;
	}
}

void CShader::ClearValueCache() const
{
	m_mUniformBoolsCache.clear();								// Cache of storage block values
	m_mUniformIntegersCache.clear();							// Cache of storage block values
	m_mUniformFloatsCache.clear();								// Cache of storage block values
	m_mUniformVector2DCache.clear();							// Cache of storage block values
	m_mUniformVector3DCache.clear();							// Cache of storage block values
	m_mUniformVector4DCache.clear();							// Cache of storage block values
	m_mUniformMatrix2Cache.clear();								// Cache of storage block values
	m_mUniformMatrix3Cache.clear();								// Cache of storage block values
	m_mUniformMatrix4Cache.clear();								// Cache of storage block values
	m_mUniformSamplersCache.clear();							// Cache of storage block values
	m_mUniformBindlessSamplersCache.clear();					// Cache of storage block values
}

/**
 * Activates this shader program for rendering.
 * Program must be linked before calling this.
 */
void CShader::Use()
{
	if (IsReady())
	{
		// 1. Tell the manager to bind this shader.
		// The manager handles the glUseProgram call only if needed.
		CStateManager::Instance().BindShader(this);

		// 2. DO NOT clear the cache. 
		// The next time you call SetMat4(), the shader will check its 
		// m_matrixCache. If the matrix is the same as the last time 
		// THIS shader was used, it will skip the glUniform call.
		//ClearValueCache();
	}
}

/**
 * Gets the OpenGL program ID.
 *
 * @return Program ID, or 0 if not initialized
 */
GLuint CShader::GetProgramID() const
{
	return (m_uiProgramID);
}

/**
 * Checks if the program is ready to use.
 *
 * @return true if initialized and linked
 */
bool CShader::IsReady() const
{
	return (m_bIsInitialized) && (m_bIsLinked) && (m_uiProgramID);
}

/**
 * Gets the shader program name.
 *
 * @return Program name for debugging
 */
const std::string& CShader::GetName() const
{
	return (m_stName);
}

/**
 * Loads shader source code from a file.
 *
 * @param stShaderPath Path to shader file
 * @return Shader source code, or empty string on error
 */
std::string CShader::LoadShaderFromFile(const std::string& stShaderPath)
{
	/* 1. retrieve the shader source code from filePath */
	std::string shaderCode;
	std::ifstream fShaderFile; /* shader file */

	/* ensure ifstream objects can throw exceptions: */
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		/* Open Files */
		fShaderFile.open(stShaderPath);

		std::stringstream sShaderStream;

		/* read file’s buffer contents into streams */
		sShaderStream << fShaderFile.rdbuf();

		/* close file handlers */
		fShaderFile.close();

		/* convert streams into strings */
		shaderCode = sShaderStream.str();

		bool bEnableShaderInjection = true;
		if (bEnableShaderInjection)
		{
			// --- INJECTION START ---
			// 1. Define your injected code based on engine state
			std::string injection = "\n// --- AnubisEngine Injected Header ---\n";
			if (Anubis::GL::IsGLVersionHigher(4, 5))
			{
				injection += "#define MODERN_OPENGL_PATH\n";
			}
			else
			{
				injection += "//#define MODERN_OPENGL_PATH\n";
			}

			// 2. Find the #version line in the loaded file
			size_t versionPos = shaderCode.find("#version");
			if (versionPos != std::string::npos)
			{
				// Find the end of that line
				size_t firstNewLine = shaderCode.find("\n", versionPos);

				// Insert the injection right after the #version line
				shaderCode.insert(firstNewLine + 1, injection);
			}
			else
			{
				// If no version found (unlikely), prepend a default version
				shaderCode = "#version 460 core\n" + injection + shaderCode;
			}
		}

	}
	catch (const std::ifstream::failure& err)
	{
		syserr("Failed to Load the Shader File {}, reason: {}", GetShaderName(stShaderPath), err.what());
		return "";
	}

	return (shaderCode);
}

/**
 * Checks for shader compilation or program linking errors.
 * Prints detailed error messages if compilation/linking fails.
 *
 * @param shader    Shader or program ID
 * @param type      "vertex", "fragment", "program", etc.
 * @param name      Shader filename for error reporting
 * @return true if no errors, false if compilation/linking failed
 */
bool CShader::CheckCompileErrors(GLuint uiShaderID, const std::string& stShaderType, const std::string& stShaderName)
{
	GLint iSuccess = 0;
	char c_szInfoLog[1024];

	if (stShaderType != "program")
	{
		glGetShaderiv(uiShaderID, GL_COMPILE_STATUS, &iSuccess);
		if (!iSuccess)
		{
			glGetShaderInfoLog(uiShaderID, 1024, nullptr, c_szInfoLog);
			syserr("Error shader: {}, Compilation Error of Type {}", stShaderName, stShaderType);
			syserr("Error Log: {}", c_szInfoLog);
		}
	}
	else
	{
		glGetProgramiv(uiShaderID, GL_LINK_STATUS, &iSuccess);
		if (!iSuccess)
		{
			glGetProgramInfoLog(uiShaderID, 1024, nullptr, c_szInfoLog);
			syserr("Error Linking Program of Type {}", stShaderType);
			syserr("Error Log: {}", c_szInfoLog);
		}
	}

	return (iSuccess);
}

/**
 * Extracts filename from full path for error messages.
 *
 * @param stShaderPath Full file path
 * @return Filename only
 */
std::string CShader::GetShaderName(const std::string& stShaderPath)
{
	std::string pathStr = stShaderPath;
	const size_t last_slash_idx = pathStr.find_last_of("/");
	if (std::string::npos != last_slash_idx)
	{
		pathStr.erase(0, last_slash_idx + 1);
	}

	return (pathStr);
}

/**
 * Extracts shader type from full path.
 *
 * @param stShaderPath Full file path
 * @return shader type str only
 */
std::string CShader::GetShaderTypeName(const std::string& stShaderPath)
{
	std::string stType = GetShaderName(stShaderPath);
	const size_t last_slash_idx = stType.find_last_of(".");
	if (std::string::npos != last_slash_idx)
	{
		stType.erase(0, last_slash_idx + 1);
	}

	return (stType);
}

/**
 * Determines shader type from file extension.
 * Supported: .vert, .frag, .geom, .tesc, .tese, .comp
 *
 * @param stShaderPath Path to shader file
 * @return Shader type info with OpenGL enum and name
 */
TShaderType CShader::GetShaderType(const std::string& stShaderPath)
{
	std::string stType = GetShaderTypeName(stShaderPath);
	TShaderType shaderType{};

	if (stType == "vert")
	{
		shaderType = TShaderType("vertex", GL_VERTEX_SHADER);
	}
	else if (stType == "frag")
	{
		shaderType = TShaderType("fragment", GL_FRAGMENT_SHADER);
	}
	else if (stType == "geom")
	{
		shaderType = TShaderType("geometry", GL_GEOMETRY_SHADER);
	}
	else if (stType == "comp")
	{
		m_bIsCompute = true;
		shaderType = TShaderType("compute", GL_COMPUTE_SHADER);
	}
	else if (stType == "tes")
	{
		shaderType = TShaderType("tess_evaluation", GL_TESS_EVALUATION_SHADER);
	}
	else if (stType == "tcs")
	{
		shaderType = TShaderType("tess_control", GL_TESS_CONTROL_SHADER);
	}

	return (shaderType);
}

/**
 * Gets the location of a uniform variable in the shader program.
 *
 * This function checks if the uniform location is already cached.
 * If not, it retrieves the location from OpenGL and caches it for future use.
 *
 * @param name: The name of the uniform variable in the shader.
 * @return The location of the uniform variable.
 */
GLint CShader::GetUniformLocation(const std::string& name) const
{
	// 1. Single lookup: find the iterator
	auto it = m_UniformLocationCache.find(name);

	// 2. If found, return it immediately
	if (it != m_UniformLocationCache.end())
	{
		return it->second;
	}

	// Not found: query OpenGL and cache it
	// 3. If not found, ask OpenGL (expensive driver call)
	GLint location = glGetUniformLocation(m_uiProgramID, name.c_str());

	// 4. Store it (we use the name we already have)
	m_UniformLocationCache[name] = location;
	return location;
}

/**
 * Sets a boolean uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided boolean.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param value: The boolean value to be set.
 */
void CShader::SetBool(const std::string& name, bool value)
{
	GLint iBoolLoc = GetUniformLocation(name);
	if (iBoolLoc == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformBoolsCache.find(iBoolLoc);
	if (it != m_mUniformBoolsCache.end() && it->second == value)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1i(iBoolLoc, (GLint)value);

	// 5. Update Cache
	if (it != m_mUniformBoolsCache.end())
	{
		it->second = value;
	}
	else
	{
		m_mUniformBoolsCache[iBoolLoc] = value;
	}
}

/**
 * Sets an integer uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided integer.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param value: The integer value to be set.
 *
 */
void CShader::SetInt(const std::string& name, GLint value)
{
	// 1. Get Location
	GLint iIntLoc = GetUniformLocation(name);
	if (iIntLoc == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformIntegersCache.find(iIntLoc);
	if (it != m_mUniformIntegersCache.end() && it->second == value)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1i(iIntLoc, value);

	// 5. Update Cache
	if (it != m_mUniformIntegersCache.end())
	{
		it->second = value;
	}
	else
	{
		m_mUniformIntegersCache[iIntLoc] = value;
	}
}

/**
 * Sets an integer array uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided integer.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param index: The integer index to be set.
 * @param value: The integer value to be set.
 *
 */
void CShader::SetIntArray(const std::string& name, GLint index, GLint value)
{
	// 1. Get Location
	std::string fullName = name + "[" + std::to_string(index) + "]";
	GLint iIntArrLoc = GetUniformLocation(fullName);

	if (iIntArrLoc == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformIntegersCache.find(iIntArrLoc);
	if (it != m_mUniformIntegersCache.end() && it->second == value)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1i(iIntArrLoc, value);

	// 5. Update Cache
	if (it != m_mUniformIntegersCache.end())
	{
		it->second = value;
	}
	else
	{
		m_mUniformIntegersCache[iIntArrLoc] = value;
	}
}

/**
 * Sets an float uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided float.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param value: The float value to be set.
 *
 */
void CShader::SetFloat(const std::string& name, GLfloat value)
{
	// 1. Get Location
	GLint iFloatLoc = GetUniformLocation(name);
	if (iFloatLoc == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformFloatsCache.find(iFloatLoc);
	if (it != m_mUniformFloatsCache.end() && it->second == value)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1f(iFloatLoc, value);

	// 5. Update Cache
	if (it != m_mUniformFloatsCache.end())
	{
		it->second = value;
	}
	else
	{
		m_mUniformFloatsCache[iFloatLoc] = value;
	}
}

/**
 *  Sets a 2D vector uniform in the shader program using two floats.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided x and y components.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param value1: The first float value (x-component).
 * @param value2: The second float value (y-component).
 *
 */
void CShader::Set2Float(const std::string& name, GLfloat value1, GLfloat value2)
{
	// 1. Get Location
	GLint iFloatLoc = GetUniformLocation(name);
	if (iFloatLoc == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector2D currentVal(value1, value2);

	// 2. Chech Cache
	auto it = m_mUniformVector2DCache.find(iFloatLoc);
	if (it != m_mUniformVector2DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform2f(iFloatLoc, value1, value2);

	// 5. Update Cache
	if (it != m_mUniformVector2DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector2DCache[iFloatLoc] = currentVal;
	}
}

/**
 * Sets a 2D vector uniform in the shader program using individual components.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value using the provided x and y components.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param x: The x-component of the vector.
 * @param y: The y-component of the vector.
 */
void CShader::SetVec2(const std::string& name, GLfloat x, GLfloat y)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector2D currentVal(x, y);

	// 2. Chech Cache
	auto it = m_mUniformVector2DCache.find(iVectorLocation);
	if (it != m_mUniformVector2DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform2f(iVectorLocation, x, y);

	// 5. Update Cache
	if (it != m_mUniformVector2DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector2DCache[iVectorLocation] = currentVal;
	}
}

/**
 * Sets a 3D vector uniform in the shader program using individual components.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value using the provided x, y, and z components.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param x: The x-component of the vector.
 * @param y: The y-component of the vector.
 * @param z: The z-component of the vector.
 */
void CShader::SetVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector3D currentVal(x, y, z);

	// 2. Chech Cache
	auto it = m_mUniformVector3DCache.find(iVectorLocation);
	if (it != m_mUniformVector3DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform3f(iVectorLocation, x, y, z);

	// 5. Update Cache
	if (it != m_mUniformVector3DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector3DCache[iVectorLocation] = currentVal;
	}
}

/**
 * Sets a 4D vector uniform in the shader program using individual components.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value using the provided x, y, z, and w components.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param x: The x-component of the vector.
 * @param y: The y-component of the vector.
 * @param z: The z-component of the vector.
 * @param w: The w-component of the vector.
 */
void CShader::SetVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector4D currentVal(x, y, z, w);

	// 2. Chech Cache
	auto it = m_mUniformVector4DCache.find(iVectorLocation);
	if (it != m_mUniformVector4DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform4f(iVectorLocation, x, y, z, w);

	// 5. Update Cache
	if (it != m_mUniformVector4DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector4DCache[iVectorLocation] = currentVal;
	}
}

void CShader::SetSampler2D(const std::string& name, GLuint iTextureID, GLint iTexSlot)
{
	// 0. Bind the texture to the correct slot, SHOULDN"T BE SKIPPED!
	CStateManager::Instance().BindTexture(iTexSlot, GL_TEXTURE_2D, iTextureID);

	// 1. Update the Uniform (Which slot should the shader look at?)
	// We use the SamplersCache to avoid redundant glUniform1i calls
	GLint iTexLocation = GetUniformLocation(name);
	if (iTexLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformSamplersCache.find(iTexLocation);
	if (it != m_mUniformSamplersCache.end() && it->second == iTexSlot)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1i(iTexLocation, iTexSlot);

	// 5. Update Cache
	if (it != m_mUniformSamplersCache.end())
	{
		it->second = iTexSlot;
	}
	else
	{
		m_mUniformSamplersCache[iTexLocation] = iTexSlot;
	}
}

void CShader::SetSampler3D(const std::string& name, GLuint iTextureID, GLint iTexSlot)
{
	// 0. Bind the texture to the correct slot, SHOULDN"T BE SKIPPED!
	CStateManager::Instance().BindTexture(iTexSlot, GL_TEXTURE_3D, iTextureID);

	// 1. Update the Uniform (Which slot should the shader look at?)
	// We use the SamplersCache to avoid redundant glUniform1i calls
	GLint iTexLocation = GetUniformLocation(name);
	if (iTexLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformSamplersCache.find(iTexLocation);
	if (it != m_mUniformSamplersCache.end() && it->second == iTexSlot)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform1i(iTexLocation, iTexSlot);

	// 5. Update Cache
	if (it != m_mUniformSamplersCache.end())
	{
		it->second = iTexSlot;
	}
	else
	{
		m_mUniformSamplersCache[iTexLocation] = iTexSlot;
	}
}

/**
 * Sets an sampler2D Bindless texture uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided integer.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param value: The unsigned integer64 value of texture Handler.
 *
 */
void CShader::SetBindlessSampler2D(const std::string& name, GLuint64 value)
{
	// 1. Get Location
	GLint iBindlessTexLoc = GetUniformLocation(name);
	if (iBindlessTexLoc == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformBindlessSamplersCache.find(iBindlessTexLoc);
	if (it != m_mUniformBindlessSamplersCache.end() && it->second == value)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformHandleui64ARB(iBindlessTexLoc, value);

	// 5. Update Cache
	if (it != m_mUniformBindlessSamplersCache.end())
	{
		it->second = value;
	}
	else
	{
		m_mUniformBindlessSamplersCache[iBindlessTexLoc] = value;
	}
}

/**
 * Binds a uniform block to a specific binding point.
 *
 * This function locates the uniform block in the shader by its name
 * and binds it to the specified binding point.
 *
 * @param stBlockName: The name of the uniform block in the shader.
 * @param uiBindingPoint: The binding point to bind the uniform block to.
 */
void CShader::BindUniformBlock(const std::string& stBlockName, GLuint uiBindingPoint)
{
	// 1. Validate
	if (!IsReady())
	{
		return;
	}

	GLuint uiBlockIndex = GL_INVALID_INDEX;

	// 2. Get/Cache the Block Index (String Lookup)
	auto itIndex = m_mUniformBlockIndexCache.find(stBlockName);
	if (itIndex != m_mUniformBlockIndexCache.end())
	{
		uiBlockIndex = itIndex->second;
	}
	else
	{
		uiBlockIndex = glGetUniformBlockIndex(m_uiProgramID, stBlockName.c_str());
		m_mUniformBlockIndexCache[stBlockName] = uiBlockIndex;
	}

	// 3. If the block does not exist, exit
	if (uiBlockIndex == GL_INVALID_INDEX)
	{
		return;
	}

	// 4. Check Cache
	auto it = m_mUniformBlockCache.find(uiBlockIndex);
	if (it != m_mUniformBlockCache.end() && it->second == uiBindingPoint)
	{
		return; // Already bound to this point
	}

	// 5. Upload change to OpenGL
	// Note: You do NOT need to BindShader(this) for this specific call!
	glUniformBlockBinding(m_uiProgramID, uiBlockIndex, uiBindingPoint);

	// 6. Update Cache
	m_mUniformBlockCache[uiBlockIndex] = uiBindingPoint;
}

/**
 * Sets a 2D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 2D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec2: The 2D GLM vector to be set.
 */
void CShader::SetVec2(const std::string& name, const glm::vec2& vec2)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector2D currentVal(vec2);

	// 2. Chech Cache
	auto it = m_mUniformVector2DCache.find(iVectorLocation);
	if (it != m_mUniformVector2DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform2fv(iVectorLocation, 1, currentVal);

	// 5. Update Cache
	if (it != m_mUniformVector2DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector2DCache[iVectorLocation] = currentVal;
	}
}

/**
 * Sets a 3D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 3D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec3: The 3D GLM vector to be set.
 */
void CShader::SetVec3(const std::string& name, const glm::vec3& vec3)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector3D currentVal(vec3);

	// 2. Chech Cache
	auto it = m_mUniformVector3DCache.find(iVectorLocation);
	if (it != m_mUniformVector3DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform3fv(iVectorLocation, 1, currentVal);

	// 5. Update Cache
	if (it != m_mUniformVector3DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector3DCache[iVectorLocation] = currentVal;
	}
}

/**
 * Sets a 4D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 4D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec4: The 4D GLM vector to be set.
 */
void CShader::SetVec4(const std::string& name, const glm::vec4& vec4)
{
	// 1. Get Location
	GLuint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Vector4D currentVal(vec4);

	// 2. Chech Cache
	auto it = m_mUniformVector4DCache.find(iVectorLocation);
	if (it != m_mUniformVector4DCache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform4fv(iVectorLocation, 1, currentVal);

	// 5. Update Cache
	if (it != m_mUniformVector4DCache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformVector4DCache[iVectorLocation] = currentVal;
	}
}

/**
 * Sets a 2x2 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 2x2 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 2x2 GLM matrix to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetMat2(const std::string& name, const glm::mat2& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Matrix2 currentVal(matrix);

	// 2. Chech Cache
	auto it = m_mUniformMatrix2Cache.find(iMatLocation);
	if (it != m_mUniformMatrix2Cache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix2fv(iMatLocation, 1, bTranspose, currentVal);

	// 5. Update Cache
	if (it != m_mUniformMatrix2Cache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformMatrix2Cache[iMatLocation] = currentVal;
	}
}

/**
 * Sets a 3x3 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 3x3 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 3x3 GLM matrix to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetMat3(const std::string& name, const glm::mat3& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Matrix3 currentVal(matrix);

	// 2. Chech Cache
	auto it = m_mUniformMatrix3Cache.find(iMatLocation);
	if (it != m_mUniformMatrix3Cache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix3fv(iMatLocation, 1, bTranspose, currentVal);

	// 5. Update Cache
	if (it != m_mUniformMatrix3Cache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformMatrix3Cache[iMatLocation] = currentVal;
	}
}

/**
 * Sets a 4x4 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 4x4 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 4x4 GLM matrix to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetMat4(const std::string& name, const glm::mat4& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// Pack into a temporary object for comparison
	Matrix4 currentVal(matrix);

	// 2. Chech Cache
	auto it = m_mUniformMatrix4Cache.find(iMatLocation);
	if (it != m_mUniformMatrix4Cache.end() && it->second == currentVal)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix4fv(iMatLocation, 1, bTranspose, currentVal);

	// 5. Update Cache
	if (it != m_mUniformMatrix4Cache.end())
	{
		it->second = currentVal;
	}
	else
	{
		m_mUniformMatrix4Cache[iMatLocation] = currentVal;
	}
}

/**
 * Sets a 2D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 2D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec2: The 2D vector to be set.
 */
void CShader::SetVec2(const std::string& name, const Vector2D& vec2)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformVector2DCache.find(iVectorLocation);
	if (it != m_mUniformVector2DCache.end() && it->second == vec2)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform2fv(iVectorLocation, 1, vec2);

	// 5. Update Cache
	if (it != m_mUniformVector2DCache.end())
	{
		it->second = vec2;
	}
	else
	{
		m_mUniformVector2DCache[iVectorLocation] = vec2;
	}
}

/**
 * Sets a 3D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 3D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec3: The 3D vector to be set.
 */
void CShader::SetVec3(const std::string& name, const Vector3D& vec3)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformVector3DCache.find(iVectorLocation);
	if (it != m_mUniformVector3DCache.end() && it->second == vec3)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform3fv(iVectorLocation, 1, vec3);

	// 5. Update Cache
	if (it != m_mUniformVector3DCache.end())
	{
		it->second = vec3;
	}
	else
	{
		m_mUniformVector3DCache[iVectorLocation] = vec3;
	}
}

/**
 * Sets a 4D vector uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 4D vector.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param vec4: The 4D vector to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetVec4(const std::string& name, const Vector4D& vec4)
{
	// 1. Get Location
	GLint iVectorLocation = GetUniformLocation(name);
	if (iVectorLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformVector4DCache.find(iVectorLocation);
	if (it != m_mUniformVector4DCache.end() && it->second == vec4)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniform4fv(iVectorLocation, 1, vec4);

	// 5. Update Cache
	if (it != m_mUniformVector4DCache.end())
	{
		it->second = vec4;
	}
	else
	{
		m_mUniformVector4DCache[iVectorLocation] = vec4;
	}
}

/**
 * Sets a 2x2 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 2x2 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 2x2 matrix to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetMat2(const std::string& name, const Matrix2& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformMatrix2Cache.find(iMatLocation);
	if (it != m_mUniformMatrix2Cache.end() && it->second == matrix)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix2fv(iMatLocation, 1, bTranspose, matrix);

	// 5. Update Cache
	if (it != m_mUniformMatrix2Cache.end())
	{
		it->second = matrix;
	}
	else
	{
		m_mUniformMatrix2Cache[iMatLocation] = matrix;
	}
}

/**
 * Sets a 3x3 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 3x3 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 3x3 matrix to be set.
 */
void CShader::SetMat3(const std::string& name, const Matrix3& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformMatrix3Cache.find(iMatLocation);
	if (it != m_mUniformMatrix3Cache.end() && it->second == matrix)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix3fv(iMatLocation, 1, bTranspose, matrix);

	// 5. Update Cache
	if (it != m_mUniformMatrix3Cache.end())
	{
		it->second = matrix;
	}
	else
	{
		m_mUniformMatrix3Cache[iMatLocation] = matrix;
	}
}

/**
 * Sets a 4x4 matrix uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided 4x4 matrix.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param matrix: The 4x4 matrix to be set.
 * @param bTranspose: Whether to transpose the matrix when setting it.
 */
void CShader::SetMat4(const std::string& name, const Matrix4& matrix, bool bTranspose)
{
	// 1. Get Location
	GLuint iMatLocation = GetUniformLocation(name);
	if (iMatLocation == -1)
	{
		return;
	}

	// 2. Chech Cache
	auto it = m_mUniformMatrix4Cache.find(iMatLocation);
	if (it != m_mUniformMatrix4Cache.end() && it->second == matrix)
	{
		return;
	}

	// 3. Ensure Bound (StateManager handles the redundancy check)
	CStateManager::Instance().BindShader(this);

	// 4. Upload to GPU
	glUniformMatrix4fv(iMatLocation, 1, bTranspose, matrix);

	// 5. Update Cache
	if (it != m_mUniformMatrix4Cache.end())
	{
		it->second = matrix;
	}
	else
	{
		m_mUniformMatrix4Cache[iMatLocation] = matrix;
	}
}

/**
 * Sets a texture uniform in the shader program.
 *
 * This function locates the uniform variable in the shader by its name
 * and sets its value to the provided texture.
 *
 * @param name: The name of the uniform variable in the shader.
 * @param pTexture: Pointer to the texture to be set.
 * @param uiUnit: The texture unit to bind the texture to.
 */
void CShader::SetTexture(const std::string& name, CTexture* pTexture, GLuint uiUnit)
{
	if (!pTexture)
	{
		syserr("Texture is null for uniform {}", name.c_str());
		return;
	}

	GLuint iTextureID = pTexture->GetTextureID();
	GLint iTexValue = static_cast<GLint>(uiUnit);
	if (pTexture->IsBindless())
	{
		GLuint64 iTextureHandle = pTexture->GetBindlessHandle();
		SetBindlessSampler2D(name, iTextureHandle);
	}
	else
	{
		SetSampler2D(name, iTextureID, iTexValue);
	}
}