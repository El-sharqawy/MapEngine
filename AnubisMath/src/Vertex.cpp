#include "Stdafx.h"
#include "Vertex.h"

/**
 * SVertex Implementations
 */
SVertex::SVertex()
{
	m_v3Position = 0.0f;
	m_v3Normals = 0.0f;   // Default normals to zero
	m_v2TexCoords = 0.0f; // Default UVs to zero
	m_v4Color = 1.0f; // Default to white color
}

SVertex::SVertex(GLfloat fX, GLfloat fY, GLfloat fZ)
{
	m_v3Position.x = fX;
	m_v3Position.y = fY;
	m_v3Position.z = fZ;

	m_v3Normals = 0.0f;   // Default normals to zero
	m_v2TexCoords = 0.0f; // Default UVs to zero
	m_v4Color = 1.0f; // Default to white color
}

SVertex::SVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fU, GLfloat fV)
{
	m_v3Position.x = fX;
	m_v3Position.y = fY;
	m_v3Position.z = fZ;

	m_v3Normals = 0.0f;   // Default normals to zero

	m_v2TexCoords.u = fU;
	m_v2TexCoords.v = fV;

	m_v4Color = 1.0f; // Default to white color
}

SVertex::SVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fU, GLfloat fV, GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
{
	m_v3Position.x = fX;
	m_v3Position.y = fY;
	m_v3Position.z = fZ;

	m_v3Normals = 0.0f;   // Default normals to zero

	m_v2TexCoords.u = fU;
	m_v2TexCoords.v = fV;

	m_v4Color.r = fR;
	m_v4Color.g = fG;
	m_v4Color.b = fB;
	m_v4Color.a = fA;
}

SVertex::SVertex(const Vector3D& v3Position)
{
	m_v3Position = v3Position;

	m_v3Normals = 0.0f;   // Default normals to zero
	m_v2TexCoords = 0.0f; // Default UVs to zero
	m_v4Color = 1.0f; // Default to white color
}

SVertex::SVertex(const Vector3D& v3Position, const Vector2D& v2TexCoords)
{
	m_v3Position = v3Position;

	m_v3Normals = 0.0f;   // Default normals to zero
	m_v2TexCoords = v2TexCoords;
	m_v4Color = 1.0f; // Default to white color
}

SVertex::SVertex(const Vector3D& v3Position, const Vector2D& v2TexCoords, const Vector4D& v4Color)
{
	m_v3Position = v3Position;

	m_v3Normals = 0.0f;   // Default normals to zero
	m_v2TexCoords = v2TexCoords;
	m_v4Color = v4Color;
}

SVertex::SVertex(const Vector3D& v3Position, const Vector4D& v4Color)
{
	m_v3Position = v3Position;
	m_v4Color = v4Color;
	m_v2TexCoords = 0.0f; // Default UVs to zero
	m_v3Normals = 0.0f;   // Default normals to zero
}

/**
 * SLinesVertex Implementations
 */
SLinesVertex::SLinesVertex()
{
	m_v3Position = 0.0f;
	m_v4Color = 1.0f; // Default to white color
}

SLinesVertex::SLinesVertex(GLfloat fX, GLfloat fY, GLfloat fZ)
{
	m_v3Position = Vector3D(fX, fY, fZ);
	m_v4Color = 1.0f; // Default to white color
}

SLinesVertex::SLinesVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
{
	m_v3Position = Vector3D(fX, fY, fZ);
	m_v4Color.r = fR;
	m_v4Color.g = fG;
	m_v4Color.b = fB;
	m_v4Color.a = fA;
}

SLinesVertex::SLinesVertex(const Vector3D& v3Position)
{
	m_v3Position = v3Position;
	m_v4Color = 1.0f; // Default to white color
}

SLinesVertex::SLinesVertex(const Vector3D& v3Position, const Vector4D& v4Color)
{
	m_v3Position = v3Position;
	m_v4Color = v4Color; // Default to white color
}

/**
 * SUIVertex Implementations
 */
SUIVertex::SUIVertex()
{
	m_v2Position = 0.0f;
	m_v2TexCoords = 0.0f;
}

SUIVertex::SUIVertex(GLfloat fX, GLfloat fY)
{
	m_v2Position = Vector2D(fX, fY);
	m_v2TexCoords = 0.0f;
}
SUIVertex::SUIVertex(GLfloat fX, GLfloat fY, GLfloat fU, GLfloat fV)
{
	m_v2Position = Vector2D(fX, fY);
	m_v2TexCoords = Vector2D(fU, fV);
}

SUIVertex::SUIVertex(const Vector2D& v2Position)
{
	m_v2Position = v2Position;
	m_v2TexCoords = 0.0f;
}
SUIVertex::SUIVertex(const Vector2D& v2Position, const Vector2D& v2TexCoords)
{
	m_v2Position = v2Position;
	m_v2TexCoords = v2TexCoords;
}