#pragma once

#include "AnubisTypes.h"
#include "TypeVector4.h"

namespace Anubis
{
	namespace GL
	{
        struct STextureFormat
        {
            GLenum internal;
            GLenum pixel;
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

        /**
         * @brief Checks if the current OpenGL version is greater than or equal to the specified version.
         *
         * @param MajorVer The major version to check (e.g., 4)
         * @param MinorVer The minor version to check (e.g., 5)
         * @return true if current version >= specified version
         */
        bool IsGLVersionHigher(GLint MajorVer, GLint MinorVer);

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