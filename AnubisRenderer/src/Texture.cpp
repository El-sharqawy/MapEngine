#include "Texture.h"
#include "CoreUtils.h"
#include "LogManager.h"
#include "GLUtils.h"
#include <stb_image/stb_image.h>
#include <filesystem>

/**
 * @brief Constructor - initializes member variables.
 */
CTexture::CTexture()
{
	m_uiTextureID = 0;						// OpenGL Texture ID
	m_iWidth = 0;							// Texture dimensions
	m_iHeight = 0;							// Texture dimensions
	m_iChannelsBPP = 0;						// Bytes per pixel
	m_ui64Handle = 0;						// For Bindless rendering (advanced)

	m_eTextureTarget = GL_TEXTURE_2D;		// Texture target (e.g., GL_TEXTURE_2D)
	m_sourceDataType = GL_UNSIGNED_BYTE;	// Data type of the source image data (e.g., GL_UNSIGNED_BYTE)

	m_stTextureName.clear();
	m_stTextureName.shrink_to_fit();		// Forces release of memory

	m_bIsLoaded = false;					// Flag indicating if the texture is loaded
	m_bIsCompressed = false;				// Flag indicating if the texture is compressed
	m_bIsBindless = false;					// Flag indicating if the texture is bindless
	m_bIsResident = false;					// Flag indicating if the texture is resident in GPU memory
	m_bHasMipMaps = true;					// Flag indicating if the texture has mipmaps
}

/**
 * @brief Destructor - cleans up all resources.
 */
CTexture::~CTexture()
{
	Destroy();
}

/**
 * @brief Move Constructor - transfers ownership of resources from another CTexture.
 *
 * @param other The CTexture object to move from.
 */
CTexture::CTexture(CTexture&& other) noexcept
{
	// Transfer ownership of resources
	m_uiTextureID = std::exchange(other.m_uiTextureID, 0);
	m_iWidth = std::exchange(other.m_iWidth, 0);
	m_iHeight = std::exchange(other.m_iHeight, 0);
	m_iChannelsBPP = std::exchange(other.m_iChannelsBPP, 0);
	m_ui64Handle = std::exchange(other.m_ui64Handle, 0);
	m_eTextureTarget = std::exchange(other.m_eTextureTarget, GL_TEXTURE_2D);
	m_bIsLoaded = std::exchange(other.m_bIsLoaded, false);
	m_bIsCompressed = std::exchange(other.m_bIsCompressed, false);
	m_bIsBindless = std::exchange(other.m_bIsBindless, false);
	m_bIsResident = std::exchange(other.m_bIsResident, false);
	m_bHasMipMaps = std::exchange(other.m_bHasMipMaps, false);
}

/**
 * @brief Move Assignment Operator - transfers ownership of resources from another CTexture.
 *
 * @param other The CTexture object to move from.
 */
CTexture& CTexture::operator=(CTexture&& other) noexcept
{
	// 1. Self-assignment check
	if (this != &other)
	{
		// 2. Clean up existing resources in 'this' object
		// This ensures we don't leak the old GPU texture ID
		Destroy();

		// 3. Transfer ownership using the same logic as the Move Constructor
		m_uiTextureID = std::exchange(other.m_uiTextureID, 0);
		m_iWidth = std::exchange(other.m_iWidth, 0);
		m_iHeight = std::exchange(other.m_iHeight, 0);
		m_iChannelsBPP = std::exchange(other.m_iChannelsBPP, 0);
		m_ui64Handle = std::exchange(other.m_ui64Handle, 0);
		m_eTextureTarget = std::exchange(other.m_eTextureTarget, GL_TEXTURE_2D);
		m_bIsLoaded = std::exchange(other.m_bIsLoaded, false);
		m_bIsCompressed = std::exchange(other.m_bIsCompressed, false);
		m_bIsBindless = std::exchange(other.m_bIsBindless, false);
		m_bIsResident = std::exchange(other.m_bIsResident, false);
		m_bHasMipMaps = std::exchange(other.m_bHasMipMaps, false);
	}

	return *this;
}

/**
 * @brief Destroys the texture, freeing GPU and CPU resources.
 */
void CTexture::Destroy()
{
	// Perform cleanup in the correct order
	Anubis::GL::MakeTextureNonResident(m_ui64Handle, m_bIsResident);

	// 1. Clean up the GPU resource
	Anubis::GL::SafeDeletGLTexture(m_uiTextureID);

	// 2. Reset all member variables to default states
	m_iWidth = 0;						// Texture dimensions
	m_iHeight = 0;						// Texture dimensions
	m_iChannelsBPP = 0;					// Bytes per pixel
	m_ui64Handle = 0;
	m_eTextureTarget = GL_TEXTURE_2D;	// Texture target (e.g., GL_TEXTURE_2D)

	m_bIsLoaded = false;				// Flag indicating if the texture is loaded
	m_bIsCompressed = false;			// Flag indicating if the texture is compressed
	m_bIsBindless = false;				// Flag indicating if the texture is bindless
	m_bIsResident = false;
	m_bHasMipMaps = false;				// Flag indicating if the texture has mipmaps
}

/**
 * @brief Loads an image file from disk into a simple data structure.
 *
 * This is a utility function that uses the stbi_load library to read
 * an image file. It forces the image to have 4 channels (RGBA).
 *
 * @param filepath The path to the image file.
 * @return An ImageData struct containing the raw pixel data and dimensions.
 */
SImageData CTexture::LoadImageData(const std::filesystem::path& filePath)
{
	// 1. Validation
	if (!std::filesystem::exists(filePath))
	{
		syserr("File not found: {}", filePath.string());
		return SImageData();
	}

	m_textureFilePath = filePath;

	std::string ext = filePath.extension().string();
	if (ext == ".dds" || ext == ".ktx")
	{
		m_bIsCompressed = true;
	}

	// Ensure that images are not flipped vertically upon loading.
	stbi_set_flip_vertically_on_load(false);
	SImageData imageData{};
	imageData.stName = filePath.string();

	// Load the image, forcing 4 color channels (RGBA).
	imageData.pData = stbi_load(filePath.string().c_str(), &imageData.iWidth, &imageData.iHeight, &imageData.iChannels, 0);
	return (imageData);
}

/**
 * @brief Loads texture data from a file.
 *
 * @param filePath The file path to load the texture from.
 * @return true if the texture was loaded successfully, false otherwise.
 */
bool CTexture::LoadTexture(const std::filesystem::path& filePath)
{
	// Convert to absolute path to see EXACTLY what the OS is looking for
	std::filesystem::path absPath = std::filesystem::absolute(filePath);

	if (!std::filesystem::exists(absPath))
	{
		syserr("Absolute path failed: {}", absPath.string());
		return false;
	}

	// 1. Validation
	SImageData imageData = LoadImageData(filePath);
	if (!imageData.IsValid())
	{
		syserr("Current Dir: {}", std::filesystem::current_path().string());
		return (false);
	}

	// 2. Set Metadata
	m_iWidth = imageData.iWidth;
	m_iHeight = imageData.iHeight;
	m_iChannelsBPP = imageData.iChannels;
	m_stTextureName = filePath.stem().string(); // Returns "texture"

	// 3. Create the OpenGL Resource (The ID)
	if (!Anubis::GL::CreateGLTexture(m_uiTextureID, m_eTextureTarget))
	{
		return (false);
	}

	// 4. Upload to GPU
	// This moves the data from RAM (m_pData) to VRAM
	Anubis::GL::UploadTextureDataToGPU(m_uiTextureID, imageData, m_eTextureTarget, m_bHasMipMaps, m_sourceDataType);

	// 5. Cleanup RAM
	// Since the data is now on the GPU, we don't need m_pTextureData in the class
	// We can just let the imageData struct handle the free() naturally.
	m_bIsLoaded = true;

	// Release the image data from CPU RAM
	if (imageData.pData)
	{
		free(imageData.pData);
	}

	// remember that the residency handle must be generated after the mipmaps are generated.
	if (m_bIsBindless)
	{
		Anubis::GL::MakeTextureResident(m_uiTextureID, m_ui64Handle, m_bIsResident);
	}

#if defined(ENABLE_DEBUG_LOGS)
	syslog("Loaded Texture: {} (W:{} H:{} Channels:{}) ID:{} Bindless: {}", filePath.string(), m_iWidth,  m_iHeight, m_iChannelsBPP, m_uiTextureID, m_bIsBindless ? "Yes" : "No");
#endif
	return (true);
}

void CTexture::BindTexture(GLuint uiUnitIndex) const
{
	if (Anubis::GL::IsGLVersionHigher(4, 5))
	{
		// Modern DSA: Just takes the index (0, 1, 2...)
		glBindTextureUnit(uiUnitIndex, m_uiTextureID);
	}
	else
	{
		// Legacy: Needs the GL_TEXTURE0 + index enum
		glActiveTexture(GL_TEXTURE0 + uiUnitIndex);
		glBindTexture(m_eTextureTarget, m_uiTextureID);
	}
}

/**
 * @brief Creates a new OpenGL texture and returns its ID.
 *
 * @return The generated OpenGL texture ID.
 */
GLuint CTexture::GenerateGLTexture()
{
	// Prevent leaks by cleaning up existing ID and re-create
	Anubis::GL::CreateGLTexture(m_uiTextureID, m_eTextureTarget);

	return GLuint(m_uiTextureID);
}

/**
 * @brief Gets the OpenGL texture ID.
 *
 * @return The OpenGL texture ID.
 */
GLuint CTexture::GetTextureID() const
{
	return m_uiTextureID;
}

/**
 * @brief Gets the width of the texture.
 *
 * @return The width of the texture.
 */
GLint CTexture::GetWidth() const
{
	return m_iWidth;
}

/**
 * @brief Gets the height of the texture.
 *
 * @return The height of the texture.
 */
GLint CTexture::GetHeight() const
{
	return m_iHeight;
}

/**
 * @brief Gets the number of channels (bytes per pixel) of the texture.
 *
 * @return The number of channels (bytes per pixel).
 */
GLint CTexture::GetChannelsBPP() const
{
	return m_iChannelsBPP;
}

/**
 * @brief Gets the bindless handle of the texture.
 *
 * @return The 64-bit bindless handle.
 */
GLuint64 CTexture::GetBindlessHandle() const
{
	return m_ui64Handle;
}

/**
 * @brief Checks if the texture is loaded.
 *
 * @return true if the texture is loaded, false otherwise.
 */
bool CTexture::IsLoaded() const
{
	return m_bIsLoaded;
}

/**
 * @brief Checks if the texture is compressed.
 *
 * @return true if the texture is compressed, false otherwise.
 */
bool CTexture::IsCompressed() const
{
	return m_bIsCompressed;
}

/**
 * @brief Checks if the texture is bindless.
 *
 * @return true if the texture is bindless, false otherwise.
 */
bool CTexture::IsBindless() const
{
	return m_bIsBindless;
}

/**
 * @brief Checks if the texture is resident in GPU memory.
 *
 * @return true if the texture is resident, false otherwise.
 */
bool CTexture::IsResident() const
{
	return m_bIsResident;
}

/**
 * @brief Checks if the texture has mipmaps.
 *
 * @return true if the texture has mipmaps, false otherwise.
 */
bool CTexture::HasMipMaps() const
{
	return m_bHasMipMaps;
}

/**
 * @brief Sets the name identifier of the texture.
 *
 * @param name The name identifier to set.
 */
void CTexture::SetTextureName(const std::string& name)
{
	m_stTextureName = name;
}

/**
 * @brief Gets the name identifier of the texture.
 *
 * @return The name identifier of the texture.
 */
std::string CTexture::GetTextureName() const
{
	return (m_stTextureName);
}


bool CTexture::GenerateColoredTexture2D(GLint iWidth, GLint iHeight, const SVector4Df& v4Color, const std::string& debugName)
{
	// 3. Set Metadata
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iChannelsBPP = 4;
	m_stTextureName = debugName; // Returns "texture"

	bool bSuccess = false;

	SImageData imageData{};

	// 1. Prepare color buffer
	if (m_sourceDataType == GL_UNSIGNED_BYTE)
	{
		auto colorData = Anubis::GL::CreateColorBuffer<GLubyte>(iWidth, iHeight, v4Color);
		// 2. Create ImageData struct
		imageData.pData = colorData.data();
		imageData.iWidth = iWidth;
		imageData.iHeight = iHeight;
		imageData.iChannels = m_iChannelsBPP;

		// 5. Upload to GPU
		// This moves the data from RAM (m_pData) to VRAM
		bSuccess = Anubis::GL::UploadTextureDataToGPU(m_uiTextureID, imageData, m_eTextureTarget, m_bHasMipMaps, m_sourceDataType);

		// 6. Cleanup RAM
		// Since the data is now on the GPU, we don't need m_pTextureData in the class
		// We can just let the imageData struct handle the free() naturally.
		// remember that the residency handle must be generated after the mipmaps are generated.
		if (m_bIsBindless)
		{
			Anubis::GL::MakeTextureResident(m_uiTextureID, m_ui64Handle, m_bIsResident);
		}

	}
	else if (m_sourceDataType == GL_FLOAT)
	{
		auto colorData = Anubis::GL::CreateColorBuffer<GLfloat>(iWidth, iHeight, v4Color);
		// 2. Create ImageData struct
		imageData.pData = colorData.data();
		imageData.iWidth = iWidth;
		imageData.iHeight = iHeight;
		imageData.iChannels = m_iChannelsBPP;

		// 5. Upload to GPU
		// This moves the data from RAM (m_pData) to VRAM
		bSuccess = Anubis::GL::UploadTextureDataToGPU(m_uiTextureID, imageData, m_eTextureTarget, m_bHasMipMaps, m_sourceDataType);

		// 6. Cleanup RAM
		// Since the data is now on the GPU, we don't need m_pTextureData in the class
		// We can just let the imageData struct handle the free() naturally.
		// remember that the residency handle must be generated after the mipmaps are generated.
		if (m_bIsBindless)
		{
			Anubis::GL::MakeTextureResident(m_uiTextureID, m_ui64Handle, m_bIsResident);
		}
	}

	m_bIsLoaded = bSuccess;
	return (bSuccess);
}