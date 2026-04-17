#pragma once

#include <algorithm>

#include "AnubisTypes.h"
#include "TypeVector4.h"
#include "Vertex.h"
#include "LogManager.h"

constexpr GLsizeiptr MAX_MAIN_VERTICES = 1024 * 1024;	// 1 million vertices for main buffers
constexpr GLsizeiptr MAX_MAIN_INDICES = 3 * 1024 * 1024;	// 3 million indices for main buffers

constexpr GLsizeiptr MAX_DEBUG_VERTICES = 256 * 1024;	// 256k vertices for debug buffers
constexpr GLsizeiptr MAX_DEBUG_INDICES = 512 * 1024;	// 512k indices for debug buffers

constexpr GLsizeiptr MAX_MAIN_VRAM_BYTES = 1024 * 1024 * 1024;	// 1GB for main buffers
constexpr GLsizeiptr MAX_DEBUG_VRAM_BYTES = 256 * 1024 * 1024;	// 256MB for debug buffers

constexpr GLsizeiptr INITIAL_DEBUG_VERTEX_CAPACITY = 1024; // Start with 1k vertices
constexpr GLsizeiptr INITIAL_DEBUG_INDEX_CAPACITY = 2048;  // Start with 2k indices

constexpr GLsizeiptr INITIAL_VERTEX_CAPACITY = 8192; // Start with 8k vertices
constexpr GLsizeiptr INITIAL_INDEX_CAPACITY = 16384;  // Start with 16k indices

namespace Anubis
{
	namespace GL
	{
        struct STextureFormat
        {
            GLenum internal;
            GLenum pixel;
        };

        /**
         * @brief Structure to hold GPU buffer IDs and their capacities.
         *
         * This structure encapsulates the Vertex Buffer Object (VBO) and Element Buffer Object (EBO)
         * along with their respective capacities in bytes. It also includes flags for Direct State Access (DSA)
         * and legacy usage patterns for buffer storage allocation.
         */
        struct SGPUBuffersGroup
        {
            GLuint uiVBO = 0;
            GLuint uiEBO = 0;
            GLsizeiptr iVboCapacity = 0;
            GLsizeiptr iEboCapacity = 0;
            GLbitfield DSAFlags = GL_DYNAMIC_STORAGE_BIT; // For DSA buffer storage allocation using glNamedBufferStorage / glBufferStorage
            GLenum LegacyUsage = GL_STATIC_DRAW; // For legacy buffer data allocation
        };

        // Buffers

        GLuint CreateBuffer();
        void DeleteBuffer(GLuint& uiBufferID);
        // Vertex Arrays
        GLuint CreateVertexArray();
        void DeleteVertexArray(GLuint& uiVertexArrayID);

        // Textures
        GLuint CreateTexture(GLenum eTextureTarget); // can also use CreateTextures(1, eTextureTarget);
        void DeleteTexture(GLuint& uiTextureID);

        bool CreateGLTexture(GLuint& uiTextureID, GLenum eTargetTexture);
        void SafeDeletGLTexture(GLuint& uiTextureID);

        void MakeTextureResident(GLuint uiTextureID, GLuint64& ui64Handle, bool& bIsResident);
        void MakeTextureNonResident(GLuint64& ui64Handle, bool& bIsResident);

        bool UploadTextureDataToGPU(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType);
        bool UploadTextureDataToGPUDSA(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType);
        bool UploadTextureDataToGPULegacy(GLuint& uiTextureID, const SImageData& sImageData, GLenum eTextureTarget, bool bGenMipMaps, GLenum sourceType);

        STextureFormat GetTextureFormats(GLint channels, GLenum* internal, GLenum* pixel, bool bIsFloat);
        void GetGLTextureFormats(GLint channels, GLenum& internal, GLenum& pixel, bool bIsFloat);

        // Modern Code
        bool CreateBuffer(GLuint& uiBufferID);
        bool CreateVertexArray(GLuint& uiVAOID);
        bool CreateBuffers(GLuint* uiBuffersIDs, GLsizei uiCount);
        bool CreateVertexArrays(GLuint* uiVAOsIDs, GLsizei uiCount);

        void SafeDeleteGLBuffer(GLuint& uiBufferID);
        void SafeDeleteGLVertexArray(GLuint& uiVAOID);
        void SafeDeleteGLBuffers(GLuint* uiBuffersIDs, GLsizei uiCount);
        void SafeDeleteGLVertexArrays(GLuint* uiVAOsIDs, GLsizei uiCount);

        // GPU Buffers
        bool CreateGPUBufferGroup(SGPUBuffersGroup& bufferGroup);
        void SafeDeleteGPUBufferGroup(SGPUBuffersGroup& bufferGroup);
		void SetupVertexBufferAttributesLines(GLuint uiVAO);
        void SetupVertexBufferAttributesVertex(GLuint uiVAO);

		/**
		 * @brief Checks if the current OpenGL version is greater than or equal to the specified version.
		 *
		 * @param MajorVer The major version to check (e.g., 4)
		 * @param MinorVer The minor version to check (e.g., 5)
		 * @return true if current version >= specified version
		 */
		bool IsGLVersionHigher(GLint MajorVer, GLint MinorVer);

		template<class T>
		const T& GLmax(const T& a, const T& b)
		{
			return (a < b) ? b : a;
		}

		template<class T>
		const T& GLmin(const T& a, const T& b)
		{
			return (b < a) ? b : a;
		}

		template<class T>
		const T& GLminmax(T min, T value, T max)
		{
			T tv;

			tv = (min > value ? min : value);
			return (max < tv) ? max : tv;
		}

        /**
         * @brief Allocates storage for the vertex and index buffers in the GPU buffer group.
         *
         * This function allocates memory for the vertex buffer object (VBO) and element buffer object (EBO)
         * based on the specified capacities in the provided GPU buffer group. It uses Direct State Access (DSA)
         * methods if available, otherwise falls back to legacy binding methods.
         *
         * Uses (DSA) on modern drivers or (Legacy) as a fallback to ensure cross-version compatibility.
         *
         * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex, SVertex).
         * @param buffersGroup The GPU buffer group to allocate storage for.
         * @return true if the allocation was successful, false otherwise.
         */
        template <typename T>
        bool AllocateBuffersStorage(SGPUBuffersGroup& buffersGroup)
        {
            GLsizeiptr vboBytes = buffersGroup.iVboCapacity * sizeof(T);
            GLsizeiptr eboBytes = buffersGroup.iEboCapacity * sizeof(GLuint);

            if (!buffersGroup.uiVBO || !buffersGroup.uiEBO)
            {
                syserr("Called with invalid VBO ({}) or EBO ({})!", buffersGroup.uiVBO, buffersGroup.uiEBO);
                return (false); // Changed from return;
            }

            // Setup buffer storage
            if (IsGLVersionHigher(4, 5))
            {
                // IMPORTANT: glNamedBufferStorage is IMMUTABLE. 
                // If the buffer was already allocated, we MUST delete and recreate the ID.
                if (vboBytes > 0)
                {
                    CreateGPUBufferGroup(buffersGroup); // Recreate to ensure fresh IDs
                }

                // Allocation (Immutable) - Ensure ID is fresh or deleted/recreated before calling
                glNamedBufferStorage(buffersGroup.uiVBO, vboBytes, nullptr, buffersGroup.DSAFlags);
                glNamedBufferStorage(buffersGroup.uiEBO, eboBytes, nullptr, buffersGroup.DSAFlags);
            }
            else
            {
                // Allocation (Mutable)
                glBindBuffer(GL_ARRAY_BUFFER, buffersGroup.uiVBO);
                glBufferData(GL_ARRAY_BUFFER, vboBytes, nullptr, buffersGroup.LegacyUsage);

                // Bind and Allocate EBO (This also links it to the VAO permanently)
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffersGroup.uiEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboBytes, nullptr, buffersGroup.LegacyUsage);
            }

            return (true);
        }

        /**
         * @brief Sets up the vertex buffer attributes for the specified Vertex Array Object (VAO).
         *
         * This function configures the vertex attribute pointers based on the vertex type T.
         * It supports different vertex structures by using template specialization.
         * It uses Direct State Access (DSA) methods if available, otherwise falls back to legacy binding methods.
         *
         * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex, SVertex).
         * @param uiVAO The Vertex Array Object to set up.
         */
        template <typename T>
        inline void SetupVertexBufferAttributes(GLuint uiVAO)
        {
            if (!uiVAO)
            {
                syserr("called with invalid VAO ID!");
                return;
            }

            // Same Type as SLinesVertex
            if constexpr (std::is_same<T, SLinesVertex>::value)
            {
                SetupVertexBufferAttributesLines(uiVAO);
            }
            else if constexpr (std::is_same<T, SVertex>::value)
            {
                SetupVertexBufferAttributesVertex(uiVAO);
            }
        }

		/**
		 * @brief Links the GPU buffer group to the specified Vertex Array Object (VAO).
		 *
		 * This function associates the vertex buffer object (VBO) and element buffer object (EBO)
		 * from the provided GPU buffer group with the given VAO. It uses Direct State Access (DSA)
		 * methods if available, otherwise falls back to legacy binding methods.
		 *
		 * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex).
		 * @param uiVAO The Vertex Array Object to link the buffers to.
		 * @param buffersGroup The GPU buffer group containing the VBO and EBO to link.
		 */
		template <typename T>
		inline bool LinkBuffersToVAO(GLuint uiVAO, const SGPUBuffersGroup& buffersGroup)
		{
			if (!uiVAO)
			{
				syserr("called with invalid VAO ID %u!", uiVAO);
				return (false);
			}

			// DSA Linking
			if (IsGLVersionHigher(4, 5))
			{
				glVertexArrayVertexBuffer(uiVAO, 0, buffersGroup.uiVBO, 0, sizeof(T));
				glVertexArrayElementBuffer(uiVAO, buffersGroup.uiEBO);
			}
			else
			{
				glBindVertexArray(uiVAO);
				glBindBuffer(GL_ARRAY_BUFFER, buffersGroup.uiVBO);
				// Bind and Allocate EBO (This also links it to the VAO permanently)
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffersGroup.uiEBO);

				// If we changed the VBO, we must re-inform the VAO where the pointers are.
				SetupVertexBufferAttributes<T>(uiVAO);

				// 
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			return (true);
		}

		/**
		 * @brief Updates the dynamic GPU buffer group with new vertex and index data.
		 *
		 * This function efficiently updates the contents of the vertex and index buffers
		 * in the provided GPU buffer group. It uses modern OpenGL techniques if available,
		 * falling back to legacy methods for compatibility with older versions.
		 *
		 * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex).
		 * @param group The GPU buffer group to update.
		 * @param pVertices Pointer to the array of vertex data.
		 * @param vertexCount Number of vertices to upload.
		 * @param pIndices Pointer to the array of index data.
		 * @param indexCount Number of indices to upload.
		 */
		template <typename T>
		inline void UpdateDynamicBufferGroup(const SGPUBuffersGroup& group,
			const T* pVertices, GLsizeiptr vertexCount,
			const GLuint* pIndices, GLsizeiptr indexCount)
		{
			if (!pVertices || !pIndices)
			{
				syserr("Called with null vertex or index data!");
				return;
			}

			// Calculate byte sizes automatically based on the template type T
			GLsizeiptr vboByteSize = vertexCount * sizeof(T);
			GLsizeiptr eboByteSize = indexCount * sizeof(GLuint);

			if (IsGLVersionHigher(4, 5))
			{
				// DSA Path: Modern Invalidation
				glInvalidateBufferData(group.uiVBO);
				glInvalidateBufferData(group.uiEBO);

				glNamedBufferSubData(group.uiVBO, 0, vboByteSize, pVertices);
				glNamedBufferSubData(group.uiEBO, 0, eboByteSize, pIndices);
			}
			else
			{
				// Legacy Path: Manual Orphaning
				glBindBuffer(GL_ARRAY_BUFFER, group.uiVBO);
				// Passing nullptr here tells the driver to flip to a new memory page
				glBufferData(GL_ARRAY_BUFFER, vboByteSize, nullptr, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vboByteSize, pVertices);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.uiEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboByteSize, nullptr, GL_STATIC_DRAW);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, eboByteSize, pIndices);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		template <typename T>
		inline bool ReallocateVault(SGPUBuffersGroup& group, GLuint uiVAO, GLsizeiptr newVboCount, GLsizeiptr newEboCount)
		{
			// 1. Store old state
			GLuint oldVBO = group.uiVBO;
			GLuint oldEBO = group.uiEBO;
			GLsizeiptr oldVboByteSize = group.iVboCapacity * sizeof(T);
			GLsizeiptr oldEboByteSize = group.iEboCapacity * sizeof(GLuint);

			// 2. Generate New Handles
			GLuint newVBO, newEBO;
			if (IsGLVersionHigher(4, 5))
			{
				glCreateBuffers(1, &newVBO);
				glCreateBuffers(1, &newEBO);
			}
			else
			{
				glGenBuffers(1, &newVBO);
				glGenBuffers(1, &newEBO);
			}

			// 3. Allocate Storage
			GLsizeiptr newVboByteSize = newVboCount * sizeof(T);
			GLsizeiptr newEboByteSize = newEboCount * sizeof(GLuint);

			if (IsGLVersionHigher(4, 5))
			{
				// Modern: Immutable Storage
				glNamedBufferStorage(newVBO, newVboByteSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
				glNamedBufferStorage(newEBO, newEboByteSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
			}
			else
			{
				// Legacy: Mutable Storage (using group.LegacyUsage like GL_STATIC_DRAW or GL_DYNAMIC_DRAW)
				glBindBuffer(GL_ARRAY_BUFFER, newVBO);
				glBufferData(GL_ARRAY_BUFFER, newVboByteSize, nullptr, GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, newEboByteSize, nullptr, GL_STATIC_DRAW);
			}

			// 4. The Copy (GPU to GPU)
			if (oldVBO != 0 && oldVboByteSize > 0)
			{
				if (IsGLVersionHigher(4, 5))
				{
					glCopyNamedBufferSubData(oldVBO, newVBO, 0, 0, oldVboByteSize);
				}
				else
				{
					// Legacy Copy: Use temporary read/write targets to avoid messing with VAO bindings
					glBindBuffer(GL_COPY_READ_BUFFER, oldVBO);
					glBindBuffer(GL_COPY_WRITE_BUFFER, newVBO);
					glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldVboByteSize);
				}
			}

			if (oldEBO != 0 && oldEboByteSize > 0)
			{
				if (IsGLVersionHigher(4, 5))
				{
					glCopyNamedBufferSubData(oldEBO, newEBO, 0, 0, oldEboByteSize);
				}
				else
				{
					glBindBuffer(GL_COPY_READ_BUFFER, oldEBO);
					glBindBuffer(GL_COPY_WRITE_BUFFER, newEBO);
					glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldEboByteSize);
				}
			}

			// 5. Cleanup
			if (oldVBO != 0) glDeleteBuffers(1, &oldVBO);
			if (oldEBO != 0) glDeleteBuffers(1, &oldEBO);

			// 6. Update Struct
			group.uiVBO = newVBO;
			group.uiEBO = newEBO;
			group.iVboCapacity = newVboCount;
			group.iEboCapacity = newEboCount;

#if defined(ENABLE_DEBUG_LOG)
			syslog("Vault Expanded to V:%zu E:%zu", newVboCount, newEboCount);
#endif
			return (true);
		}

		/**
		 * @brief Reallocates the GPU buffer group to new vertex and index capacities.
		 *
		 * This function cleans up the existing buffers, creates new buffer IDs,
		 * allocates storage with the updated capacities, and links the new buffers
		 * back to the provided Vertex Array Object (VAO).
		 *
		 * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex).
		 * @param group The GPU buffer group to reallocate.
		 * @param uiVAO The Vertex Array Object associated with the buffer group.
		 * @param newVboCount The new capacity for the vertex buffer.
		 * @param newEboCount The new capacity for the index buffer.
		 * @return true if reallocation was successful, false otherwise.
		 */
		template <typename T>
		inline bool ReallocateBufferGroup(SGPUBuffersGroup& group, GLuint uiVAO, GLsizeiptr newVboCount, GLsizeiptr newEboCount)
		{
			ReallocateVault<T>(group, uiVAO, newVboCount, newEboCount);

			// Since the IDs changed, the VAO's internal state is now pointing to deleted memory
			LinkBuffersToVAO<T>(uiVAO, group);

			return true;
		}

		/**
		 * @brief Ensures the GPU buffer group has enough capacity for the requested vertices and indices.
		 *
		 * If the requested vertex or index count exceeds the current capacity, the buffers are reallocated
		 * to a larger size (typically doubling the current capacity or meeting the requested size).
		 * This function also enforces hard caps on debug buffers to prevent excessive VRAM usage.
		 *
		 * @tparam T The vertex type stored in the buffer (e.g., SLinesVertex).
		 * @param group The GPU buffer group to check and potentially reallocate.
		 * @param uiVAO The Vertex Array Object associated with the buffer group.
		 * @param reqVerts The required number of vertices.
		 * @param reqIndices The required number of indices.
		 */
		template <typename T>
		inline void EnsureBufferCapacity(SGPUBuffersGroup& group, GLuint uiVAO, GLsizeiptr reqVerts, GLsizeiptr reqIndices)
		{
			// Check if we need to grow
			if (reqVerts > group.iVboCapacity || reqIndices > group.iEboCapacity)
			{
				// Calculate new capacity (e.g., current * 2)
				GLsizeiptr nextV = Anubis::GL::GLmax(reqVerts, Anubis::GL::GLmax(INITIAL_DEBUG_VERTEX_CAPACITY, group.iVboCapacity * 2));
				GLsizeiptr nextI = Anubis::GL::GLmax(reqIndices, Anubis::GL::GLmax(INITIAL_DEBUG_INDEX_CAPACITY, group.iEboCapacity * 2));

				/** HARD CAPS For Debugging **/
				if constexpr (std::is_same <T, SLinesVertex>::value)
				{
					// HARD CAP: Don't let debug buffers exceed 128MB
					GLsizeiptr totalBytes = (nextV * sizeof(SLinesVertex)) + (nextI * sizeof(GLuint));

					if (totalBytes > MAX_DEBUG_VRAM_BYTES)
					{
						syserr("Renderer: Debug buffer growth capped!");
						// Clamp the growth to the maximum allowed
						nextV = Anubis::GL::GLmin(nextV, MAX_DEBUG_VRAM_BYTES / (GLsizeiptr)sizeof(SLinesVertex));
						nextI = Anubis::GL::GLmin(nextI, MAX_DEBUG_VRAM_BYTES / (GLsizeiptr)sizeof(GLuint));
					}
				}

				ReallocateBufferGroup<T>(group, uiVAO, nextV, nextI);

				//syslog("Renderer: Reallocated Buffer Group to V:%lld I:%lld", nextV, nextI);
			}
		}

		template <typename T>
		bool InitializeBufferGroup(SGPUBuffersGroup& bufferGroup, GLuint& uiVAO)
		{
			// 1. Setup the Metadata inside the struct
			bufferGroup.iVboCapacity = INITIAL_VERTEX_CAPACITY;
			bufferGroup.iEboCapacity = INITIAL_INDEX_CAPACITY;
			bufferGroup.DSAFlags = GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT;
			bufferGroup.LegacyUsage = GL_STATIC_DRAW;

			// 2. Create the GPU "Names" (IDs)
			// These functions should call glCreate... or glGen..
			CreateVertexArray(uiVAO);
			CreateGPUBufferGroup(bufferGroup);

			// 3. Allocate the actual VRAM
			// This uses the metadata we just set to call glNamedBufferStorage or glBufferData
			AllocateBuffersStorage<T>(bufferGroup);

			// 4. Define the Vertex Layout (What does a vertex look like?)
			// This tells the VAO how to interpret the data (Position, Color, etc.)
			SetupVertexBufferAttributes<T>(uiVAO);

			// 5. Physical Link (Which buffers belong to this VAO?)
			// This tells the VAO which specific VBO/EBO IDs to pull from
			LinkBuffersToVAO<T>(uiVAO, bufferGroup);

			return (true);
		}

        /**
         * @brief Creates a color buffer filled with a specified color.
         *
         * This template function generates a color buffer of the given width and height,
         * filling it with the specified RGBA color. The buffer can be created with either
         * unsigned byte or floating-point data types based on the passed type T.
         *
         * @tparam T The data type of the buffer elements (GLubyte or GLfloat).
         * @param width The width of the color buffer.
         * @param height The height of the color buffer.
         * @param color The RGBA color to fill the buffer with.
         * @return A vector containing the color buffer data.
         */
        template <typename T>
        inline std::vector<T> CreateColorBuffer(GLint width, GLint height, const Vector4D& color)
        {
            std::vector<T> data(width * height * 4);
            for (GLint i = 0; i < width * height; ++i)
            {
                if constexpr (std::is_same_v<T, GLubyte>)
                {
                    data[i * 4 + 0] = static_cast<GLubyte>(color.r * 255.0f);
                    data[i * 4 + 1] = static_cast<GLubyte>(color.g * 255.0f);
                    data[i * 4 + 2] = static_cast<GLubyte>(color.b * 255.0f);
                    data[i * 4 + 3] = static_cast<GLubyte>(color.a * 255.0f);
                }
                else if constexpr (std::is_same_v<T, GLfloat>)
                {
                    data[i * 4 + 0] = static_cast<T>(color.r);
                    data[i * 4 + 1] = static_cast<T>(color.g);
                    data[i * 4 + 2] = static_cast<T>(color.b);
                    data[i * 4 + 3] = static_cast<T>(color.a);
                }
            }

            return (data);
        }
	}
}