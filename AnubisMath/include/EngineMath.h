#pragma once

/**
 * @file EngineMath.h
 * @brief Core linear algebra and graphics utility functions.
 *
 * This file contains all essential mathematical structures (vectors, matrices, quaternions)
 * and corresponding operations (transformations, normalization, geometry calculations)
 * designed specifically for the engine's 3D graphics and physics systems.
 *
 * All functions are implemented to be highly optimized and concise.
 */

#include <glad/glad.h>

struct SVector2Df;
struct SVector3Df;
struct SVector4Df;
struct SMatrix2x2;
struct SMatrix3x3;
struct SMatrix4x4;
struct SQuaternion;
struct SRay;

/**
 * @brief Provides core linear algebra and graphics utility functions.
 *
 * This namespace contains all essential mathematical structures (vectors, matrices, quaternions)
 * and corresponding operations (transformations, normalization, geometry calculations)
 * designed specifically for the engine's 3D graphics and physics systems.
 *
 * All functions are implemented to be highly optimized and concise.
 */
namespace EngineMath
{
	/**
	 * Provides a const pointer to the underlying float array.
	 *
	 * This allows direct access to the components as a float array.
	 *
	 * @return A const pointer to the float array.
	 */
	const GLfloat* value_ptr(const SVector2Df& vec2);
	const GLfloat* value_ptr(const SVector3Df& vec3);
	const GLfloat* value_ptr(const SVector4Df& vec4);
	const GLfloat* value_ptr(const SMatrix2x2& mat2);
	const GLfloat* value_ptr(const SMatrix3x3& mat3);
	const GLfloat* value_ptr(const SMatrix4x4& mat4);
	const GLfloat* value_ptr(const SQuaternion& quat);

	/**
	 * Provides a pointer to the underlying float array.
	 *
	 * This allows direct access to the components as a float array.
	 *
	 * @return A pointer to the float array.
	 */
	GLfloat* value_ptr(SVector2Df& vec2);
	GLfloat* value_ptr(SVector3Df& vec3);
	GLfloat* value_ptr(SVector4Df& vec4);
	GLfloat* value_ptr(SMatrix2x2& mat2);
	GLfloat* value_ptr(SMatrix3x3& mat3);
	GLfloat* value_ptr(SMatrix4x4& mat4);
	GLfloat* value_ptr(SQuaternion& quat);

	/**
	 * @brief Converts degrees to radians.
	 *
	 * @param fDegrees The angle in degrees.
	 * @return The angle in radians.
	 */
	GLfloat ToRadians(const GLfloat fDegrees);

	/**
	 * @brief Converts radians to degrees.
	 *
	 * @param fRadians The angle in radians.
	 * @return The angle in degrees.
	 */
	GLfloat ToDegrees(const GLfloat fRadians);

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
	 * @param outDistance Output parameter that will hold the distance from the ray origin to the intersection point if an intersection occurs.
	 * @return True if the ray intersects the triangle; otherwise, false.
	 */
	bool IntersectRayTriangle(const SRay& ray, const SVector3Df& v0, const SVector3Df& v1, const SVector3Df& v2, GLfloat& outDistance);

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
	SVector3Df Vec3Lerp(const SVector3Df& v3Start, const SVector3Df& v3End, GLfloat t);

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
	void ConvertToMapCoordindates(GLfloat fX, GLfloat fZ, GLint* iCellX, GLint* iCellZ, GLint* iSubCellX, GLint* iSubCellZ, GLint* iTerrainNumX, GLint* iTerrainNumZ);

	/**
	 * @brief Gets the size of a static array.
	 *
	 * This function calculates the number of elements in a static array type T.
	 *
	 * @tparam T The static array type.
	 * @return The number of elements in the array.
	 */
	template <typename T>
	GLsizeiptr GetArraySize()
	{
		return (sizeof(T) / sizeof(T[0]));
	}

} // namespace EngineMath - General Part