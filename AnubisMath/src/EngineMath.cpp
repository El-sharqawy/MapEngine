#include "Stdafx.h"
#include "EngineMath.h"
#include "PrespectiveProjection.h"
#include "OrthographicProjection.h"
#include <algorithm>

#include "Ray.h"
#include "EngineMathVectors.h"

/**
 * @brief Provides a const pointer to the underlying float array.
 *
 * This allows direct access to the components as a float array.
 *
 * @return A const pointer to the float array.
 */
const GLfloat* EngineMath::value_ptr(const SVector2Df& vec2)
{
	return (&(vec2.x));
}

const GLfloat* EngineMath::value_ptr(const SVector3Df& vec3)
{
	return (&(vec3.x));
}
const GLfloat* EngineMath::value_ptr(const SVector4Df& vec4)
{
	return (&(vec4.x));
}

const GLfloat* EngineMath::value_ptr(const SMatrix2x2& mat2)
{
	return (&(mat2[0].x));
}

const GLfloat* EngineMath::value_ptr(const SMatrix3x3& mat3)
{
	return (&(mat3[0].x));
}

const GLfloat* EngineMath::value_ptr(const SMatrix4x4& mat4)
{
	return (&(mat4[0].x));
}

const GLfloat* EngineMath::value_ptr(const SQuaternion& quat)
{
	return (&(quat.w));
}

/**
 * Provides a pointer to the underlying float array.
 *
 * This allows direct access to the components as a float array.
 *
 * @return A pointer to the float array.
 */
GLfloat* EngineMath::value_ptr(SVector2Df& vec2)
{
	return (&(vec2.x));
}

GLfloat* EngineMath::value_ptr(SVector3Df& vec3)
{
	return (&(vec3.x));
}
GLfloat* EngineMath::value_ptr(SVector4Df& vec4)
{
	return (&(vec4.x));
}

GLfloat* EngineMath::value_ptr(SMatrix2x2& mat2)
{
	return (&(mat2[0].x));
}

GLfloat* EngineMath::value_ptr(SMatrix3x3& mat3)
{
	return (&(mat3[0].x));
}

GLfloat* EngineMath::value_ptr(SMatrix4x4& mat4)
{
	return (&(mat4[0].x));
}

GLfloat* EngineMath::value_ptr(SQuaternion& quat)
{
	return (&(quat.w));
}

/**
 * @brief Converts degrees to radians.
 *
 * @param fDegrees The angle in degrees.
 * @return The angle in radians.
 */
GLfloat EngineMath::ToRadians(const GLfloat fDegrees)
{
	GLfloat fAngleRad = ((fDegrees) * static_cast<GLfloat>(M_PI) / 180.0f);
	return (fAngleRad);
}

/**
 * @brief Converts radians to degrees.
 *
 * @param fRadians The angle in radians.
 * @return The angle in degrees.
 */
GLfloat EngineMath::ToDegrees(const GLfloat fRadians)
{
	GLfloat fAngleDeg = ((fRadians) * 180.0f / static_cast<GLfloat>(M_PI));
	return (fAngleDeg);
}

/**
 * @brief Tests for intersection between a ray and a triangle using the Mِller–Trumbore algorithm.
 *
 * This function checks if the given ray intersects with the triangle defined by the vertices v0, v1, and v2.
 * If an intersection occurs, it calculates the distance 't' from the ray's origin to the intersection point.
 *
 * @param ray The ray to test for intersection.
 * @param v0 The first vertex of the triangle.
 * @param v1 The second vertex of the triangle.
 * @param v2 The third vertex of the triangle.
 * @param t Output parameter that will hold the distance from the ray origin to the intersection point if an intersection occurs.
 * @return True if the ray intersects the triangle; otherwise, false.
 */
bool EngineMath::IntersectRayTriangle(const SRay& ray, const SVector3Df& v0, const SVector3Df& v1, const SVector3Df& v2, GLfloat& outDistance)
{
	const GLfloat EPSILON = 0.0000001f;
	// --- STEP 1: Calculate the "Edges" of the triangle ---
	// These vectors represent two sides of the triangle starting from v0.
	Vector3D edge1 = v1 - v0;
	Vector3D edge2 = v2 - v0;

	// --- STEP 2: Start calculating the Determinant ---
	// We use the Cross Product to find a vector perpendicular to the ray and one edge.

	Vector3D h = EngineMath::Cross(ray.GetDirection(), edge2);

	// The Dot Product here tells us how "parallel" the ray is to the triangle.
	GLfloat det = EngineMath::Dot(edge1, h);

	// If the determinant is near zero, the ray is lying in the plane of the 
	// triangle or is perfectly parallel to it. No intersection.
	if (det > -EPSILON && det < EPSILON)
	{
		return (false);
	}

	// --- STEP 3: Calculate the 'U' Barycentric Coordinate ---
	// This tells us how far the hit point is from v0 along edge1.
	GLfloat invDet = 1.0f / det;
	Vector3D s = ray.GetOrigin() - v0;
	GLfloat u = invDet * EngineMath::Dot(s, h);

	// If U is outside [0, 1], the hit point is outside the triangle's boundaries.
	if (u < 0.0f || u > 1.0f)
	{
		return (false);
	}

	// --- STEP 4: Calculate the 'V' Barycentric Coordinate ---
	// This tells us how far the hit point is from v0 along edge2.
	Vector3D q = EngineMath::Cross(s, edge1);
	GLfloat v = invDet * EngineMath::Dot(ray.GetDirection(), q);

	// If V is negative, or U+V > 1, the point is outside the triangle.
	if (v < 0.0f || u + v > 1.0f)
	{
		return (false);
	}

	// --- STEP 5: Calculate 'T' (The distance along the ray) ---
	// If we reach here, the ray definitely hits the triangle!
	// We just need to know how far away it is.
	GLfloat t = invDet * EngineMath::Dot(edge2, q);

	// Only return true if the hit is in front of the camera (t > 0)
	// and within our ray's maximum travel distance.
	if (t > EPSILON && t < ray.GetMaxDistance())
	{
		outDistance = t;
		return (true);
	}

	return (false);
}

/**
 * @brief Performs linear interpolation between two 3D vectors.
 *
 * This function calculates a point along the line connecting v3Start and v3End
 * based on the interpolation factor t. When t=0, the result is v3Start;
 * when t=1, the result is v3End; values in between yield points along the line.
 *
 * @param v3Start The starting vector.
 * @param v3End The ending vector.
 * @param t The interpolation factor, typically in the range [0, 1].
 * @return The interpolated vector.
 */
SVector3Df EngineMath::Vec3Lerp(const SVector3Df& v3Start, const SVector3Df& v3End, GLfloat t)
{
	return((1.0f - t) * v3Start + t * v3End);
	//return (v3Start + t * (v3End - v3Start));
}

/**
 * @brief Converts world coordinates to map cell and sub-cell indices.
 *
 * This function takes world coordinates (fX, fZ) and computes the corresponding
 * map cell indices (iCellX, iCellZ) and sub-cell indices (iSubCellX, iSubCellZ)
 * and terrain numbers (iTerrainNumX, iTerrainNumZ)
 * based on predefined cell scaling factors.
 *
 * @param fX The world X coordinate.
 * @param fZ The world Z coordinate.
 * @param iCellX Output parameter for the calculated cell X index.
 * @param iCellZ Output parameter for the calculated cell Z index.
 * @param iSubCellX Output parameter for the calculated sub-cell X index.
 * @param iSubCellZ Output parameter for the calculated sub-cell Z index.
 * @param iTerrainNumX Output parameter for the terrain number in the X direction.
 * @param iTerrainNumZ Output parameter for the terrain number in the Z direction.
 */
void EngineMath::ConvertToMapCoordindates(GLfloat fX, GLfloat fZ, GLint* iCellX, GLint* iCellZ, GLint* iSubCellX, GLint* iSubCellZ, GLint* iTerrainNumX, GLint* iTerrainNumZ)
{
	/*GLfloat fTerrainXSize = TERRAIN_XSIZE;
	GLfloat fTerrainZSize = TERRAIN_XSIZE;

	// Calculate terrain grid indices based on predefined terrain tile sizes
	*iTerrainNumX = static_cast<GLint>(fX / fTerrainXSize); // X-axis grid index
	*iTerrainNumZ = static_cast<GLint>(fZ / fTerrainZSize); // Z-axis grid index

	GLfloat fMaxX = TERRAIN_XSIZE;
	GLfloat fMaxZ = TERRAIN_ZSIZE;

	// Negative Values Not Allowed
	while (fX < 0.0f)
	{
		fX += fMaxX;
	}

	while (fZ < 0.0f)
	{
		fZ += fMaxZ;
	}

	// Out of Bounds not Allowed
	while (fX > fMaxX)
	{
		fX -= fMaxX;
	}
	while (fZ > fMaxZ)
	{
		fZ -= fMaxZ;
	}

	GLfloat fScale = 1.0f / static_cast<GLfloat>(CELL_SCALE_METER); // Inverse for slope calculations

	GLfloat fCellX = fX * fScale;
	GLfloat fCellZ = fZ * fScale;

	*iCellX = static_cast<GLint>(fCellX); // fCellX
	*iCellZ = static_cast<GLint>(fCellZ); // fCellZ

	GLfloat fRatioScale = static_cast<GLfloat>(HEIGHT_TILE_XRATIO) * fScale;

	GLfloat fSubCellX = fX * fRatioScale;
	GLfloat fSubCellZ = fZ * fRatioScale;

	*iSubCellX = static_cast<GLint>(fSubCellX);
	*iSubCellZ = static_cast<GLint>(fSubCellZ);

	*iSubCellX = (*iSubCellX) % HEIGHT_TILE_XRATIO;
	*iSubCellZ = (*iSubCellZ) % HEIGHT_TILE_ZRATIO;*/
}
