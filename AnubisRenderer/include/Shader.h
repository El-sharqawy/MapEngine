#pragma once

#include <string>
#include <unordered_map>

#include "TypeVector2.h"
#include "TypeVector3.h"
#include "TypeVector4.h"
#include "TypeMatrix2.h"
#include "TypeMatrix3.h"
#include "TypeMatrix4.h"
#include "Texture.h"

typedef struct SShaderType
{
	std::string m_stName;
	GLuint m_uiType;

	SShaderType()
	{
		m_stName = "unkonwn";
		m_uiType = 0;
	}

	SShaderType(const std::string& stName, GLuint uiType)
	{
		m_stName = stName;
		m_uiType = uiType;
	}
} TShaderType;

class CShader
{
public:
	/**
	 * Creates a new shader with given name.
	 *
	 * @param name Optional name for debugging (default: "Shader")
	 */
	explicit CShader(const std::string& name = "Shader");

	/**
	 * Destructor - automatically cleans up OpenGL resources.
	 */
	~CShader();

	/**
	 * Prevent copying(OpenGL resources shouldn't be copied)
	 */
	CShader(const CShader&) = delete;
	CShader& operator=(const CShader&) = delete;

	/**
	 * Initializes and creates a new shader program.
	 */
	void InitializeShader();

	/**
	 * Attaches a shader from file to the program.
	 * Shader type is automatically detected from file extension.
	 *
	 * @param stShaderPath Path to shader file (.vert, .frag, .geom, etc.)
	 * @return true if successfully compiled and attached, false otherwise
	 */
	bool AttachShader(const std::string& stShaderPath);

	/**
	 * Links all attached shaders into a complete program.
	 * Must be called after all shaders are attached and before Use().
	 *
	 * @return true if linking succeeded, false otherwise
	 */
	bool LinkProgram();

	void PreCacheUniforms();

	void ClearValueCache() const;

	/**
	 * Activates this shader program for rendering.
	 * Program must be linked before calling this.
	 */
	void Use();

	/**
	 * Gets the OpenGL program ID.
	 *
	 * @return Program ID, or 0 if not initialized
	 */
	GLuint GetProgramID() const;

	/**
	 * Checks if the program is ready to use.
	 *
	 * @return true if initialized and linked
	 */
	bool IsReady() const;

	/**
	 * Gets the shader program name.
	 *
	 * @return Program name for debugging
	 */
	const std::string& GetName() const;

private:
	/**
	 * Loads shader source code from a file.
	 *
	 * @param stShaderPath Path to shader file
	 * @return Shader source code, or empty string on error
	 */
	std::string LoadShaderFromFile(const std::string& stShaderPath);

	/**
	 * Checks for shader compilation or program linking errors.
	 * Prints detailed error messages if compilation/linking fails.
	 *
	 * @param shader    Shader or program ID
	 * @param type      "vertex", "fragment", "program", etc.
	 * @param name      Shader filename for error reporting
	 * @return true if no errors, false if compilation/linking failed
	 */
	bool CheckCompileErrors(GLuint uiShaderID, const std::string& stShaderType, const std::string& stShaderName);

	/**
	 * Extracts filename from full path for error messages.
	 *
	 * @param stShaderPath Full file path
	 * @return Filename only
	 */
	std::string GetShaderName(const std::string& stShaderPath);

	/**
	 * Extracts shader type from full path.
	 *
	 * @param stShaderPath Full file path
	 * @return shader type str only
	 */
	std::string GetShaderTypeName(const std::string& stShaderPath);

	/**
	 * Determines shader type from file extension.
	 * Supported: .vert, .frag, .geom, .tesc, .tese, .comp
	 *
	 * @param stShaderPath Path to shader file
	 * @return Shader type info with OpenGL enum and name
	 */
	TShaderType GetShaderType(const std::string& stShaderPath);

public:
	GLint GetUniformLocation(const std::string& name) const;

	/* general utility uniform functions */
	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, GLint value);
	void SetIntArray(const std::string& name, GLint index, GLint value);
	void SetFloat(const std::string& name, GLfloat value);
	void Set2Float(const std::string& name, GLfloat value1, GLfloat value2);
	void SetVec2(const std::string& name, GLfloat x, GLfloat y);
	void SetVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z);
	void SetVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void SetSampler2D(const std::string& name, GLuint iTextureID, GLint iTexValue);
	void SetSampler3D(const std::string& name, GLuint iTextureID, GLint iTexValue);
	void SetBindlessSampler2D(const std::string& name, GLuint64 value);
	void BindUniformBlock(const std::string& blockName, GLuint bindingPoint);

	/* glm utility uniform functions */
	void SetVec2(const std::string& name, const glm::vec2& vec2);
	void SetVec3(const std::string& name, const glm::vec3& vec3);
	void SetVec4(const std::string& name, const glm::vec4& vec4);
	void SetMat2(const std::string& name, const glm::mat2& matrix, bool bTranspose = false);
	void SetMat3(const std::string& name, const glm::mat3& matrix, bool bTranspose = false);
	void SetMat4(const std::string& name, const glm::mat4& matrix, bool bTranspose = false);

	/* my math utility uniform functions */
	void SetVec2(const std::string& name, const Vector2D& vec2);
	void SetVec3(const std::string& name, const Vector3D& vec3);
	void SetVec4(const std::string& name, const Vector4D& vec4);
	void SetMat2(const std::string& name, const Matrix2& matrix, bool bTranspose = false);
	void SetMat3(const std::string& name, const Matrix3& matrix, bool bTranspose = false);
	void SetMat4(const std::string& name, const Matrix4& matrix, bool bTranspose = false);

	void SetTexture(const std::string& name, CTexture* pTexture, GLuint uiUnit);

private:
	std::string m_stName;               // Program name for debugging
	GLuint m_uiProgramID;               // OpenGL program object ID
	bool m_bIsInitialized;              // Program object created
	bool m_bIsLinked;                   // Shaders linked successfully
	bool m_bIsCompute;					// If his is compute shader
	std::vector<GLuint> m_vecShaders;   // Temporary storage for shader IDs
	mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;						// Cache of uniform locations

	mutable std::unordered_map<GLint, bool> m_mUniformBoolsCache;								// Cache of storage block values
	mutable std::unordered_map<GLint, GLint> m_mUniformIntegersCache;							// Cache of storage block values
	mutable std::unordered_map<GLint, GLfloat> m_mUniformFloatsCache;							// Cache of storage block values
	mutable std::unordered_map<GLint, Vector2D> m_mUniformVector2DCache;						// Cache of storage block values
	mutable std::unordered_map<GLint, Vector3D> m_mUniformVector3DCache;						// Cache of storage block values
	mutable std::unordered_map<GLint, Vector4D> m_mUniformVector4DCache;						// Cache of storage block values
	mutable std::unordered_map<GLint, Matrix2> m_mUniformMatrix2Cache;							// Cache of storage block values
	mutable std::unordered_map<GLint, Matrix3> m_mUniformMatrix3Cache;							// Cache of storage block values
	mutable std::unordered_map<GLint, Matrix4> m_mUniformMatrix4Cache;							// Cache of storage block values
	mutable std::unordered_map<GLint, GLint> m_mUniformSamplersCache;							// Cache of storage block values
	mutable std::unordered_map<GLint, GLuint64> m_mUniformBindlessSamplersCache;				// Cache of storage block values
	mutable std::unordered_map<std::string, GLint> m_mUniformBlockIndexCache;					// Cache of storage block values
	mutable std::unordered_map<GLint, GLint> m_mUniformBlockCache;								// Cache of storage block values
};

