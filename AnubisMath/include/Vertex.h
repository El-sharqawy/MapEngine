#pragma once

#include <glad/glad.h>
#include "TypeVector2.h"
#include "TypeVector3.h"
#include "TypeVector4.h"

/**
 * @brief Vertex structure (PNT) for rendering.
 *
 * This structure defines the layout of a vertex used in rendering,
 * including position, texture coordinates, and normals.
 */
struct SVertex
{
	Vector3D m_v3Position;		// World position
	Vector3D m_v3Normals;		// Normal
	Vector2D m_v2TexCoords;		// UVs (For Texturing)
	Vector4D m_v4Color;			// Color

	SVertex();
	SVertex(GLfloat fX, GLfloat fY, GLfloat fZ);
	SVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fU, GLfloat fV);
	SVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fU, GLfloat fV, GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA);
	SVertex(const Vector3D& v3Position);
	SVertex(const Vector3D& v3Position, const Vector2D& v2TexCoords);
	SVertex(const Vector3D& v3Position, const Vector2D& v2TexCoords, const Vector4D& v4Color);
	SVertex(const Vector3D& v3Position, const Vector4D& v4Color); // No UVs
};

using Vertex = SVertex;

/**
 * @brief Vertex structure for rendering lines.
 *
 * This structure defines the layout of a vertex used specifically for line rendering,
 * including position and color.
 */
struct SLinesVertex
{
	Vector3D m_v3Position;		// World position
	Vector4D m_v4Color;			// Color

	SLinesVertex();
	SLinesVertex(GLfloat fX, GLfloat fY, GLfloat fZ);
	SLinesVertex(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA);
	SLinesVertex(const Vector3D& v3Position);
	SLinesVertex(const Vector3D& v3Position, const Vector4D& v4Color);
};

using LinesVertex = SLinesVertex;

struct SUIVertex
{
	Vector2D m_v2Position;
	Vector2D m_v2TexCoords;
	Vector4D m_v4Color;			// Color

	SUIVertex();
	SUIVertex(GLfloat fX, GLfloat fY);
	SUIVertex(GLfloat fX, GLfloat fY, GLfloat fU, GLfloat fV);
	SUIVertex(const Vector2D& v2Position);
	SUIVertex(const Vector2D& v2Position, const Vector2D& v2TexCoords);
};

using UIVertex = SUIVertex;