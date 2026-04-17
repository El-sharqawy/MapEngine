#include "Stdafx.h"
#include "GLUtils.h"
#include "LogManager.h"

GLuint Anubis::GL::CreateBuffer()
{
	GLuint uiBufferID = 0;

	// Create new ID
	if (IsGLVersionHigher(4, 5))
	{
		glCreateBuffers(1, &uiBufferID);
	}
	else
	{
		glGenBuffers(1, &uiBufferID);
	}
	return (uiBufferID);
}

void Anubis::GL::DeleteBuffer(GLuint& uiBufferID)
{
	if (uiBufferID != 0)
	{
		glDeleteBuffers(1, &uiBufferID);
		uiBufferID = 0;
	}
}

GLuint Anubis::GL::CreateVertexArray()
{
	GLuint uiVertexArrayID = 0;

	// Create new ID
	if (IsGLVersionHigher(4, 5))
	{
		glCreateVertexArrays(1, &uiVertexArrayID);
	}
	else
	{
		glGenVertexArrays(1, &uiVertexArrayID);
	}
	return (uiVertexArrayID);
}

void Anubis::GL::DeleteVertexArray(GLuint& uiVertexArrayID)
{
	if (uiVertexArrayID != 0)
	{
		glDeleteVertexArrays(1, &uiVertexArrayID);
		uiVertexArrayID = 0;
	}
}

GLuint Anubis::GL::CreateTexture(GLenum eTextureTarget)
{
	GLuint uiTextureID = 0;

	if (IsGLVersionHigher(4, 5))
	{
		// DSA: No binding required, very efficient
		glCreateTextures(eTextureTarget, 1, &uiTextureID);
	}
	else
	{
		// Legacy: MUST bind to "initialize" the object type in the driver
		glGenTextures(1, &uiTextureID);

		// Use the passed target (eTargetTexture) instead of hardcoded GL_TEXTURE_2D
		glBindTexture(eTextureTarget, uiTextureID);
	}

	return (uiTextureID);
}

void Anubis::GL::DeleteTexture(GLuint& uiTextureID)
{
	if (uiTextureID != 0)
	{
		glDeleteTextures(1, &uiTextureID);
		uiTextureID = 0;
	}
}


/**
 * @brief Creates a single OpenGL buffer and returns its ID.
 *
 * Uses glCreateBuffers (DSA) on modern drivers or glGenBuffers (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiBufferID Reference to store the created buffer ID.
 * @return true if the buffer was created successfully, false otherwise.
 */
bool Anubis::GL::CreateBuffer(GLuint& uiBufferID)
{
	// 1. Prevent leaks by cleaning up existing ID
	Anubis::GL::SafeDeleteGLBuffer(uiBufferID);

	// 2. Create new ID
	if (Anubis::GL::IsGLVersionHigher(4, 5))
	{
		glCreateBuffers(1, &uiBufferID);
	}
	else
	{
		glGenBuffers(1, &uiBufferID);
	}

	if (uiBufferID == 0)
	{
		syserr("Renderer: Failed to generate GL buffer!");
		return (false);
	}
	return (true);
}

/**
 * @brief Creates a single OpenGL Vertex Array Object (VAO) and returns its ID.
 *
 * Uses glCreateVertexArrays (DSA) on modern drivers or glGenVertexArrays (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiVAOID Reference to store the created VAO ID.
 * @return true if the VAO was created successfully, false otherwise.
 */
bool Anubis::GL::CreateVertexArray(GLuint& uiVAOID)
{
	// 1. Prevent leaks by cleaning up existing ID
	SafeDeleteGLVertexArray(uiVAOID);

	// 2. Create new ID
	if (IsGLVersionHigher(4, 5))
	{
		glCreateVertexArrays(1, &uiVAOID);
	}
	else
	{
		glGenVertexArrays(1, &uiVAOID);
	}

	if (uiVAOID == 0)
	{
		syserr("Renderer: Failed to generate VAO!");
		return (false);
	}
	return (true);
}

/**
 * @brief Creates multiple OpenGL buffers and returns their IDs.
 *
 * Uses glCreateBuffers (DSA) on modern drivers or glGenBuffers (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiBuffersIDs Pointer to an array to store the created buffer IDs.
 * @param uiCount Number of buffers to create.
 * @return true if all buffers were created successfully, false otherwise.
 */
bool Anubis::GL::CreateBuffers(GLuint* uiBuffersIDs, GLsizei uiCount)
{
	SafeDeleteGLBuffers(uiBuffersIDs, uiCount);

	if (IsGLVersionHigher(4, 5))
	{
		glCreateBuffers(uiCount, uiBuffersIDs);
	}
	else
	{
		glGenBuffers(uiCount, uiBuffersIDs);
	}

	for (GLsizei i = 0; i < uiCount; ++i)
	{
		if (uiBuffersIDs[i] == 0)
		{
			syserr("Renderer: Failed to create GPU buffer at index %d", i);
			return (false);
		}
	}
	return (true);
}

/**
 * @brief Creates multiple OpenGL Vertex Array Objects (VAOs) and returns their IDs.
 *
 * Uses glCreateVertexArrays (DSA) on modern drivers or glGenVertexArrays (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiVAOsIDs Pointer to an array to store the created VAO IDs.
 * @param uiCount Number of VAOs to create.
 * @return true if all VAOs were created successfully, false otherwise.
 */
bool Anubis::GL::CreateVertexArrays(GLuint* uiVAOsIDs, GLsizei uiCount)
{
	SafeDeleteGLVertexArrays(uiVAOsIDs, uiCount);

	if (IsGLVersionHigher(4, 5))
	{
		glCreateVertexArrays(uiCount, uiVAOsIDs);
	}
	else
	{
		glGenVertexArrays(uiCount, uiVAOsIDs);
	}

	for (GLsizei i = 0; i < uiCount; ++i)
	{
		if (uiVAOsIDs[i] == 0)
		{
			syserr("Renderer: Failed to create GPU Vertex Array at index %d", i);
			return (false);
		}
	}

	return (true);
}

/**
 * @brief Safely deletes an OpenGL buffer and resets its ID.
 *
 * @param uiBufferID Reference to the buffer ID to delete.
 */
void Anubis::GL::SafeDeleteGLBuffer(GLuint& uiBufferID)
{
	if (uiBufferID)
	{
		glDeleteBuffers(1, &uiBufferID);
		uiBufferID = 0;
	}
}

/**
 * @brief Safely deletes an OpenGL Vertex Array Object (VAO) and resets its ID.
 *
 * @param uiVAOID Reference to the VAO ID to delete.
 */
void Anubis::GL::SafeDeleteGLVertexArray(GLuint& uiVAOID)
{
	if (uiVAOID)
	{
		glDeleteVertexArrays(1, &uiVAOID);
		uiVAOID = 0;
	}
}

/**
 * @brief Safely deletes multiple OpenGL buffers and resets their IDs.
 *
 * @param uiBuffersIDs Pointer to the array of buffer IDs to delete.
 * @param uiCount Number of buffers to delete.
 */
void Anubis::GL::SafeDeleteGLBuffers(GLuint* uiBuffersIDs, GLsizei uiCount)
{
	if (uiBuffersIDs == nullptr || uiCount <= 0)
		return;

	// OpenGL spec: "Unused names in buffers are silently ignored, as is the value 0."
	glDeleteBuffers(uiCount, uiBuffersIDs);

	// Always reset to 0 so the rest of your engine knows they are gone
	for (GLsizei i = 0; i < uiCount; ++i)
	{
		uiBuffersIDs[i] = 0;
	}
}

/**
 * @brief Safely deletes multiple OpenGL Vertex Array Objects (VAOs) and resets their IDs.
 *
 * @param uiVAOsIDs Pointer to the array of VAO IDs to delete.
 * @param uiCount Number of VAOs to delete.
 */
void Anubis::GL::SafeDeleteGLVertexArrays(GLuint* uiVAOsIDs, GLsizei uiCount)
{
	if (uiVAOsIDs == nullptr || uiCount <= 0)
		return;

	// OpenGL spec: "Unused names in vertex arrays are silently ignored, as is the value 0."
	glDeleteVertexArrays(uiCount, uiVAOsIDs);

	// Always reset to 0 so the rest of your engine knows they are gone
	for (GLsizei i = 0; i < uiCount; ++i)
	{
		uiVAOsIDs[i] = 0;
	}
}

/**
 * @brief Creates a GPU buffer group with VBO and EBO.
 *
 * This function initializes the Vertex Buffer Object (VBO) and Element Buffer Object (EBO)
 * for the provided GPU buffer group structure. It ensures that any existing buffers are
 * safely deleted before creating new ones.
 *
 * Uses glCreateBuffers (DSA) on modern drivers or glGenBuffers (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param bufferGroup Reference to the GPUBuffersGroup structure to initialize.
 * @return true if the buffer group was created successfully, false otherwise.
 */
bool Anubis::GL::CreateGPUBufferGroup(SGPUBuffersGroup& bufferGroup)
{
	// Prevent Leaks: Clean up if this group was already used
	SafeDeleteGPUBufferGroup(bufferGroup);

	// We pass the address of the first element (&group.uiVBO) 
	// and tell OpenGL to fill 2 slots, Batch Create
	if (IsGLVersionHigher(4, 5))
	{
		glCreateBuffers(2, &bufferGroup.uiVBO);
	}
	else
	{
		glGenBuffers(2, &bufferGroup.uiVBO);
	}

	// If the first one is 0, the driver failed to provide IDs
	if (bufferGroup.uiVBO == 0)
	{
		syserr("Failed to generate Buffer Group!");
		return false;
	}

	return (true);
}

/**
 * @brief Safely deletes the GPU buffer group and resets its IDs.
 *
 * @param bufferGroup Reference to the GPUBuffersGroup structure to delete.
 */
void Anubis::GL::SafeDeleteGPUBufferGroup(SGPUBuffersGroup& bufferGroup)
{
	// glDeleteBuffers safely ignores 0, so we can just check if either is non-zero
	if (bufferGroup.uiVBO != 0) // Checking the first one is usually enough
	{
		glDeleteBuffers(2, &bufferGroup.uiVBO);
		// Clear all IDs in one go
		memset(&bufferGroup.uiVBO, 0, sizeof(GLuint) * 2);
	}
}

/**
 * @brief Sets up the vertex buffer attributes for line vertices.
 *
 * This function configures the vertex attribute pointers for line vertices
 * in the specified Vertex Array Object (VAO).
 *
 * Uses (DSA) on modern drivers or (Legacy) as a fallback to ensure cross-version compatibility.
 *
 * @param uiVAO The Vertex Array Object to set up.
 */
void Anubis::GL::SetupVertexBufferAttributesLines(GLuint uiVAO)
{
	// Define attribute layout
	const GLint iPosition = 0;
	const GLint iColors = 1;

	if (IsGLVersionHigher(4, 5))
	{
		// Attributes
		glEnableVertexArrayAttrib(uiVAO, iPosition); // Position
		glVertexArrayAttribFormat(uiVAO, iPosition, 3, GL_FLOAT, GL_FALSE, offsetof(SLinesVertex, m_v3Position));
		glVertexArrayAttribBinding(uiVAO, iPosition, 0);

		glEnableVertexArrayAttrib(uiVAO, iColors); // Colors
		glVertexArrayAttribFormat(uiVAO, iColors, 4, GL_FLOAT, GL_FALSE, offsetof(SLinesVertex, m_v4Color));
		glVertexArrayAttribBinding(uiVAO, iColors, 0);
	}
	else
	{
		// --- LEGACY PATH ---
		glBindVertexArray(uiVAO);

		// Define Attributes (While m_uiMainVBO is bound to GL_ARRAY_BUFFER)
		glEnableVertexAttribArray(iPosition); // Position
		glVertexAttribPointer(iPosition, 3, GL_FLOAT, GL_FALSE, sizeof(SLinesVertex), (void*)offsetof(SLinesVertex, m_v3Position));

		glEnableVertexAttribArray(iColors); // Colors Coords
		glVertexAttribPointer(iColors, 4, GL_FLOAT, GL_FALSE, sizeof(SLinesVertex), (void*)offsetof(SLinesVertex, m_v4Color));

		glBindVertexArray(0);
	}
}

/**
 * @brief Sets up the vertex buffer attributes for standard vertices.
 *
 * This function configures the vertex attribute pointers for standard vertices
 * in the specified Vertex Array Object (VAO).
 *
 * Uses (DSA) on modern drivers or (Legacy) as a fallback to ensure cross-version compatibility.
 *
 * @param uiVAO The Vertex Array Object to set up.
 */
void Anubis::GL::SetupVertexBufferAttributesVertex(GLuint uiVAO)
{
	// Define attribute layout
	const GLint iPosition = 0;
	const GLint iNormals = 1;
	const GLint iTexCoord = 2;
	const GLint iColors = 3;

	if (IsGLVersionHigher(4, 5))
	{
		// Attributes
		glEnableVertexArrayAttrib(uiVAO, iPosition); // Position
		glVertexArrayAttribFormat(uiVAO, iPosition, 3, GL_FLOAT, GL_FALSE, offsetof(SVertex, m_v3Position));
		glVertexArrayAttribBinding(uiVAO, iPosition, 0);

		glEnableVertexArrayAttrib(uiVAO, iNormals); // Normals
		glVertexArrayAttribFormat(uiVAO, iNormals, 3, GL_FLOAT, GL_FALSE, offsetof(SVertex, m_v3Normals));
		glVertexArrayAttribBinding(uiVAO, iNormals, 0);

		glEnableVertexArrayAttrib(uiVAO, iTexCoord); // Tex Coords
		glVertexArrayAttribFormat(uiVAO, iTexCoord, 2, GL_FLOAT, GL_FALSE, offsetof(SVertex, m_v2TexCoords));
		glVertexArrayAttribBinding(uiVAO, iTexCoord, 0);

		glEnableVertexArrayAttrib(uiVAO, iColors); // Colors
		glVertexArrayAttribFormat(uiVAO, iColors, 4, GL_FLOAT, GL_FALSE, offsetof(SVertex, m_v4Color));
		glVertexArrayAttribBinding(uiVAO, iColors, 0);
	}
	else
	{
		// --- LEGACY PATH ---
		glBindVertexArray(uiVAO);

		// Define Attributes (While m_uiMainVBO is bound to GL_ARRAY_BUFFER)
		glEnableVertexAttribArray(iPosition); // Position
		glVertexAttribPointer(iPosition, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, m_v3Position));

		glEnableVertexAttribArray(iNormals); // Normals
		glVertexAttribPointer(iNormals, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, m_v3Normals));

		glEnableVertexAttribArray(iTexCoord); // Texture Coords
		glVertexAttribPointer(iTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, m_v2TexCoords));

		glEnableVertexAttribArray(iColors); // Colors Coords
		glVertexAttribPointer(iColors, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, m_v4Color));

		glBindVertexArray(0);
	}
}

/**
 * @brief Creates a single OpenGL texture and returns its ID.
 *
 * Uses glCreateTextures (DSA) on modern drivers or glGenTextures (Legacy)
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiTextureID Reference to store the created texture ID.
 * @param eTargetTexture The target texture type (e.g., GL_TEXTURE_2D).
 * @return true if the texture was created successfully, false otherwise.
 */
bool Anubis::GL::CreateGLTexture(GLuint& uiTextureID, GLenum eTargetTexture)
{
	// 1. Prevent leaks by cleaning up existing ID
	SafeDeletGLTexture(uiTextureID);

	// 2. Create new ID
	if (IsGLVersionHigher(4, 5))
	{
		// DSA: No binding required, very efficient
		glCreateTextures(eTargetTexture, 1, &uiTextureID);
	}
	else
	{
		// Legacy: MUST bind to "initialize" the object type in the driver
		glGenTextures(1, &uiTextureID);

		// Use the passed target (eTargetTexture) instead of hardcoded GL_TEXTURE_2D
		glBindTexture(eTargetTexture, uiTextureID);
	}

	// 3. Validation
	if (uiTextureID == 0)
	{
		syserr("Core: Failed to generate GL Texture!");
		return (false);
	}

	return (true);
}

/**
 * @brief Safely deletes an OpenGL texture and resets its ID.
 *
 * @param uiTextureID Reference to the texture ID to delete.
 */
void Anubis::GL::SafeDeletGLTexture(GLuint& uiTextureID)
{
	if (uiTextureID)
	{
		glDeleteTextures(1, &uiTextureID);
		uiTextureID = 0;
	}
}

/**
 * @brief Makes a texture handle resident on the GPU for bindless access.
 * * Generates a 64-bit GPU memory handle for the texture and locks it into
 * GPU memory. This allows shaders to access the texture directly via the
 * handle without using glBindTexture.
 * @param uiTextureID The OpenGL ID of the texture to make resident.
 * @param ui64Handle The 64-bit handle generated by the driver.
 * @param bIsResident Set to true if residency was successfully established.
 */
void Anubis::GL::MakeTextureResident(GLuint uiTextureID, GLuint64& ui64Handle, bool& bIsResident)
{
	if (!GLAD_GL_ARB_bindless_texture)
	{
		ui64Handle = 0;
		bIsResident = false;
		syserr("Bindless Textures not supported on this GPU/Driver!");
		return;
	}

	if (!bIsResident)
	{
		// 2. Get the 64-bit GPU address
		// Important: Once a handle is generated for a texture, it is permanent for the life of that texture ID.
		// You don't actually need to get it again if you just want to toggle residency.
		ui64Handle = glGetTextureHandleARB(uiTextureID);
		if (ui64Handle != 0)
		{
			glMakeTextureHandleResidentARB(ui64Handle);
			bIsResident = true;
		}
	}
}

/**
 * @brief Makes a texture handle non-resident and releases the bindless state.
 * Tells the GPU that the texture handle is no longer needed for direct access.
 * This must be called before deleting the texture or changing its storage.
 * @param ui64Handle The 64-bit handle to release. Set to 0 after call.
 * @param bIsResident  Reset to false after the handle is released.
 */
void Anubis::GL::MakeTextureNonResident(GLuint64& ui64Handle, bool& bIsResident)
{
	if (!GLAD_GL_ARB_bindless_texture)
	{
		ui64Handle = 0;
		bIsResident = false;
		return;
	}

	if (bIsResident && ui64Handle != 0)
	{
		// No need to check glIsTextureHandleResidentARB (it's slow)
		// Just tell the driver this handle is no longer in use
		glMakeTextureHandleNonResidentARB(ui64Handle);

		ui64Handle = 0;
		bIsResident = false;
	}
}

/**
 * @brief Uploads texture data to the GPU.
 *
 * Uses Direct State Access (DSA) on modern drivers or legacy binding methods
 * as a fallback to ensure cross-version compatibility.
 *
 * @param uiTextureID Reference to store the created texture ID.
 * @param sImageData The image data to upload to the GPU.
 * @param eTextureTarget The target texture type (e.g., GL_TEXTURE_2D).
 * @param bGenMipMaps Whether to generate mipmaps for the texture.
 * @param sourceType The data type of the source image data (e.g., GL_UNSIGNED_BYTE).
 * @return true if the texture was created and data uploaded successfully, false otherwise.
 */
bool Anubis::GL::UploadTextureDataToGPU(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType)
{
	if (IsGLVersionHigher(4, 6))
	{
		return (UploadTextureDataToGPUDSA(uiTextureID, sImageData, eTextureTarget, bGenMipMaps, sourceType));
	}
	else
	{
		return (UploadTextureDataToGPULegacy(uiTextureID, sImageData, eTextureTarget, bGenMipMaps, sourceType));
	}
}

/**
 * @brief Uploads texture data to the GPU using legacy binding methods.
 *
 * This function creates a new OpenGL texture using legacy binding methods,
 * uploads the provided image data, and sets texture parameters such as filtering
 * and wrapping. It supports generating mipmaps if requested.
 *
 * @param uiTextureID Reference to store the created texture ID.
 * @param sImageData The image data to upload to the GPU.
 * @param eTextureTarget The target texture type (e.g., GL_TEXTURE_2D).
 * @param bGenMipMaps Whether to generate mipmaps for the texture.
 * @param sourceType The data type of the source image data (e.g., GL_UNSIGNED_BYTE).
 * @return true if the texture was created and data uploaded successfully, false otherwise.
 */
bool Anubis::GL::UploadTextureDataToGPUDSA(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType)
{
	if (!CreateGLTexture(uiTextureID, eTextureTarget))
	{
		syserr("Failed to create texture for DSA upload!");
		return false;
	}

	// Calculate Mip Levels
	GLint iLevels = 1;
	if (bGenMipMaps && sImageData.iWidth > 0)
	{
		iLevels = static_cast<GLint>(std::floor(std::log2(std::max(sImageData.iWidth, sImageData.iHeight)))) + 1;
	}

	bool bIsFloat = (sourceType == GL_FLOAT);

	if (eTextureTarget == GL_TEXTURE_2D)
	{
		// Reset Swizzle to standard
		GLint defaultSwizzle[] = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
		glTextureParameteriv(uiTextureID, GL_TEXTURE_SWIZZLE_RGBA, defaultSwizzle);

		// Determine formats based on channel count
		// 3. Determine Formats
		GLenum internalFormat, pixelFormat;
		bool bIsFloat = (sourceType == GL_FLOAT);

		GetGLTextureFormats(sImageData.iChannels, internalFormat, pixelFormat, bIsFloat);

		// 1. Allocate Immutable Storage
		glTextureStorage2D(uiTextureID, iLevels, internalFormat, sImageData.iWidth, sImageData.iHeight);

		// 2. Upload Data - USE sourceType HERE!
		if (sImageData.pData)
		{
			glTextureSubImage2D(uiTextureID, 0, 0, 0, sImageData.iWidth, sImageData.iHeight, pixelFormat, sourceType, sImageData.pData);
		}

		// 3. Handle Grayscale Swizzle
		if (sImageData.iChannels == 1)
		{
			GLint greySwizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
			glTextureParameteriv(uiTextureID, GL_TEXTURE_SWIZZLE_RGBA, greySwizzle);
		}
	}

	// Parameters
	glTextureParameteri(uiTextureID, GL_TEXTURE_MIN_FILTER, iLevels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTextureParameteri(uiTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(uiTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(uiTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (iLevels > 1 && sImageData.pData)
	{
		glGenerateTextureMipmap(uiTextureID);
	}

	return (true);
}

/**
 * @brief Uploads texture data to the GPU using legacy binding methods.
 *
 * This function creates a new OpenGL texture using legacy binding methods,
 * uploads the provided image data, and sets texture parameters such as filtering
 * and wrapping. It supports generating mipmaps if requested.
 *
 * @param uiTextureID Reference to store the created texture ID.
 * @param sImageData The image data to upload to the GPU.
 * @param eTextureTarget The target texture type (e.g., GL_TEXTURE_2D).
 * @param bGenMipMaps Whether to generate mipmaps for the texture.
 * @param sourceType The data type of the source image data (e.g., GL_UNSIGNED_BYTE).
 * @return true if the texture was created and data uploaded successfully, false otherwise.
 */
bool Anubis::GL::UploadTextureDataToGPULegacy(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType)
{
	// 1. Immutable Storage requires a fresh ID if we are re-loading
	if (!CreateGLTexture(uiTextureID, eTextureTarget))
	{
		syserr("Failed to create texture for Legacy upload!");
		return (false);
	}

	glBindTexture(eTextureTarget, uiTextureID);

	// 2. Calculate Mip Levels (In Legacy, we don't pre-allocate levels, but we need to know if we want them)
	bool bUseMips = bGenMipMaps && sImageData.iWidth > 1;

	// 3. Determine Formats
	GLenum internalFormat, pixelFormat;
	bool bIsFloat = (sourceType == GL_FLOAT);

	GetGLTextureFormats(sImageData.iChannels, internalFormat, pixelFormat, bIsFloat);

	// 4. Upload Data (Mutable Storage)
	// Note: Legacy uses glTexImage2D. The '0' is the base mip level.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(eTextureTarget, 0, internalFormat, sImageData.iWidth, sImageData.iHeight, 0, pixelFormat, sourceType, sImageData.pData);

	// 5. Set Parameters (Using target-based functions)
	glTexParameteri(eTextureTarget, GL_TEXTURE_MIN_FILTER, bUseMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(eTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(eTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(eTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// 6. Handle Swizzle (If 1-channel)
	if (sImageData.iChannels == 1)
	{
		GLint greySwizzle[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
		glTexParameteriv(eTextureTarget, GL_TEXTURE_SWIZZLE_RGBA, greySwizzle);
	}

	// 7. Generate Mipmaps if needed
	if (bUseMips && sImageData.pData)
	{
		glGenerateMipmap(eTextureTarget);
	}

	// 8. Unbind to avoid accidental state changes elsewhere
	glBindTexture(eTextureTarget, 0);

	return (true);
}

Anubis::GL::STextureFormat Anubis::GL::GetTextureFormats(GLint channels, GLenum* internal, GLenum* pixel, bool bIsFloat)
{
	Anubis::GL::STextureFormat texForm = { bIsFloat ? GL_RGBA32F : GL_RGBA8, GL_RGBA };
	switch (channels)
	{
	case 1:
		texForm.internal = bIsFloat ? GL_R32F : GL_R8;
		texForm.pixel = GL_RED;
		break;
	case 2:
		texForm.internal = bIsFloat ? GL_RG32F : GL_RG8;
		texForm.pixel = GL_RG;
		break;
	case 3:
		texForm.internal = bIsFloat ? GL_RGB32F : GL_RGB8;
		texForm.pixel = GL_RGB;  break;
	case 4:
		texForm.internal = bIsFloat ? GL_RGBA32F : GL_RGBA8;
		texForm.pixel = GL_RGBA; break;

	default:
		syslog("Format not implemented: {} channels", channels);
		texForm.internal = bIsFloat ? GL_RGBA32F : GL_RGBA8;
		texForm.pixel = GL_RGBA;
		break;
	}

	return (texForm);
}

/**
 * @brief Maps the number of channels to OpenGL internal and pixel formats.
 *
 * @param channels Number of color channels (1-4).
 * @param internal Reference to store the internal format.
 * @param pixel Reference to store the pixel format.
 */
void Anubis::GL::GetGLTextureFormats(GLint channels, GLenum& internal, GLenum& pixel, bool bIsFloat)
{
	switch (channels)
	{
	case 1:
		internal = bIsFloat ? GL_R32F : GL_R8;
		pixel = GL_RED;
		break;
	case 2:
		internal = bIsFloat ? GL_RG32F : GL_RG8;
		pixel = GL_RG;
		break;
	case 3:
		internal = bIsFloat ? GL_RGB32F : GL_RGB8;
		pixel = GL_RGB;  break;
	case 4:
		internal = bIsFloat ? GL_RGBA32F : GL_RGBA8;
		pixel = GL_RGBA; break;

	default:
		syslog("Format not implemented: {} channels", channels);
		internal = bIsFloat ? GL_RGBA32F : GL_RGBA8;
		pixel = GL_RGBA;
		break;
	}
}

/**
 * @brief Checks if the current OpenGL version is greater than or equal to the specified version.
 *
 * @param MajorVer The major version to check (e.g., 4)
 * @param MinorVer The minor version to check (e.g., 5)
 * @return true if current version >= specified version
 */
bool Anubis::GL::IsGLVersionHigher(GLint MajorVer, GLint MinorVer)
{
	static GLint glMajorVersion = 0;
	static GLint glMinorVersion = 0;

	// Only query once for performance
	if (glMajorVersion == 0)
	{
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
	}

	// Check major version first
	if (glMajorVersion > MajorVer)
	{
		return true;
	}

	// Same major version, check minor
	if (glMajorVersion == MajorVer && glMinorVersion >= MinorVer)
	{
		return true;
	}

	return false;
}

