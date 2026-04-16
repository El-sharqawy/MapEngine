#pragma once

#include "AnubisTypes.h"

class CTexture
{
public:
	/**
	 * @brief Constructor - initializes member variables.
	 */
	CTexture();

	/**
	 * @brief Destructor - cleans up all resources.
	 */
	~CTexture();

	// Disable copying - textures are unique GPU resources
	/**
	 * @brief Copy Constructor - deleted to prevent copying of CTexture.
	 *
	 * @param other The CTexture object to copy from.
	 */
	CTexture(const CTexture& other) = delete;

	/**
	 * @brief Copy Assignment Operator - deleted to prevent copying of CTexture.
	 *
	 * @param other The CTexture object to copy from.
	 */
	CTexture& operator=(const CTexture& other) = delete;

	// Allow move semantics
	/**
	 * @brief Move Constructor - transfers ownership of resources from another CTexture.
	 *
	 * @param other The CTexture object to move from.
	 */
	CTexture(CTexture&& other) noexcept;

	/**
	 * @brief Move Assignment Operator - transfers ownership of resources from another CTexture.
	 *
	 * @param other The CTexture object to move from.
	 */
	CTexture& operator=(CTexture&& other) noexcept;

	/**
	 * @brief Destroys the texture, freeing GPU and CPU resources.
	 */
	void Destroy();

	/**
	 * @brief Loads an image file from disk into a simple data structure.
	 *
	 * This is a utility function that uses the stbi_load library to read
	 * an image file. It forces the image to have 4 channels (RGBA).
	 *
	 * @param filepath The path to the image file.
	 * @return An ImageData struct containing the raw pixel data and dimensions.
	 */
	SImageData LoadImageData(const std::filesystem::path& filePath);

	/**
	 * @brief Loads texture data from a file.
	 *
	 * @param filePath The file path to load the texture from.
	 * @return true if the texture was loaded successfully, false otherwise.
	 */
	bool LoadTexture(const std::filesystem::path& filePath);

	/**
	 * @brief Binds the texture to the specified texture unit.
	 *
	 * @param uiUnitIndex The texture unit index to bind the texture to.
	 */
	void BindTexture(GLuint uiUnitIndex) const;

	/**
	 * @brief Creates a new OpenGL texture and returns its ID.
	 *
	 * @return The generated OpenGL texture ID.
	 */
	GLuint GenerateGLTexture();

	/**
	 * @brief Generates a solid color 2D texture.
	 *
	 * @param iWidth The width of the texture.
	 * @param iHeight The height of the texture.
	 * @param v4Color The RGBA color to fill the texture with.
	 * @param debugName The name identifier for debugging purposes.
	 * @return true if the texture was generated successfully, false otherwise.
	 */
	bool GenerateColoredTexture2D(GLint iWidth, GLint iHeight, const SVector4Df& v4Color = SVector4Df(1.0f), const std::string& debugName = "GeneratedTexture");

	/**
	 * @brief Gets the OpenGL texture ID.
	 *
	 * @return The OpenGL texture ID.
	 */
	GLuint GetTextureID() const;

	/**
	 * @brief Gets the width of the texture.
	 *
	 * @return The width of the texture.
	 */
	GLint GetWidth() const;

	/**
	 * @brief Gets the height of the texture.
	 *
	 * @return The height of the texture.
	 */
	GLint GetHeight() const;

	/**
	 * @brief Gets the number of channels (bytes per pixel) of the texture.
	 *
	 * @return The number of channels (bytes per pixel).
	 */
	GLint GetChannelsBPP() const;

	/**
	 * @brief Gets the bindless handle of the texture.
	 *
	 * @return The 64-bit bindless handle.
	 */
	GLuint64 GetBindlessHandle() const;

	/**
	 * @brief Checks if the texture is loaded.
	 *
	 * @return true if the texture is loaded, false otherwise.
	 */
	bool IsLoaded() const;

	/**
	 * @brief Checks if the texture is compressed.
	 *
	 * @return true if the texture is compressed, false otherwise.
	 */
	bool IsCompressed() const;

	/**
	 * @brief Checks if the texture is bindless.
	 *
	 * @return true if the texture is bindless, false otherwise.
	 */
	bool IsBindless() const;

	/**
	 * @brief Checks if the texture is resident in GPU memory.
	 *
	 * @return true if the texture is resident, false otherwise.
	 */
	bool IsResident() const;

	/**
	 * @brief Checks if the texture has mipmaps.
	 *
	 * @return true if the texture has mipmaps, false otherwise.
	 */
	bool HasMipMaps() const;
	
	/**
	 * @brief Sets the name identifier of the texture.
	 *
	 * @param name The name identifier to set.
	 */
	void SetTextureName(const std::string& name);

	/**
	 * @brief Gets the name identifier of the texture.
	 *
	 * @return The name identifier of the texture.
	 */
	std::string GetTextureName() const;

private:
	GLuint m_uiTextureID;		// OpenGL Texture ID
	GLint m_iWidth;				// Texture dimensions
	GLint m_iHeight;			// Texture dimensions
	GLint m_iChannelsBPP;		// Bytes per pixel
	GLuint64 m_ui64Handle;		// For Bindless rendering (advanced)

	GLenum m_eTextureTarget;	// Texture target (e.g., GL_TEXTURE_2D)
	GLenum m_sourceDataType;	// Data type of the source image data (e.g., GL_UNSIGNED_BYTE)

	bool m_bIsLoaded;			// Flag indicating if the texture is loaded
	bool m_bIsCompressed;		// Flag indicating if the texture is compressed
	bool m_bIsBindless;			// Flag indicating if the texture is bindless
	bool m_bIsResident;			// Flag indicating if the texture is resident in GPU memory
	bool m_bHasMipMaps;			// Flag indicating if the texture has mipmaps

	std::filesystem::path m_textureFilePath;	// File path of the texture
	std::string m_stTextureName;				// Name identifier for the texture
};