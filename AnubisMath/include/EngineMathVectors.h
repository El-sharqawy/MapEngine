#pragma once

#include <glad/glad.h>

struct SVector2Df;
struct SVector3Df;
struct SVector4Df;

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
	//////////////////////////////////////////////////////////////////
	/////////////////////////////VECTOR2//////////////////////////////
	//////////////////////////////////////////////////////////////////

	/**
	 * GLfloat Calculates the squared length (magnitude) of the SVector2Df object.
	 *
	 * @param vec The SVector2Df object to calculate it's squared length.
	 * @return The squared length of the vector.
	 */
	GLfloat LengthSquared(const SVector2Df& v3Vec);

	/**
	 * GLfloat Calculates the length (magnitude) of the SVector2Df object.
	 *
	 * @param vec The SVector2Df object to calculate it's length.
	 * @return The length of the vector.
	 */
	GLfloat Length(const SVector2Df& v2Vec);

	/**
	 * GLfloat Normalizes the SVector2Df object, making its length 1.
	 *
	 * @param vec The SVector2Df object to normalize.
	 * @return A reference to this modified SVector2Df object.
	 */
	SVector2Df Normalize(const SVector2Df& vec);

	/**
	 * GLfloat Calculates the dot product of two SVector2Df objects.
	 *
	 * @param vec1 The SVector2Df object to calculate the dot product with.
	 * @param vec2 The SVector2Df object to calculate the dot product with.
	 * @return The dot product of the two vectors.
	 */
	GLfloat Dot(const SVector2Df& vec1, const SVector2Df& vec2);

	/**
	 * @brief Calculates the Euclidean distance between two SVector2Df objects.
	 *
	 * @param vec1 The SVector2Df object to calculate the distance from.
	 * @param vec2 The SVector2Df object to calculate the distance to.
	 * @return The distance between the two vectors.
	 */
	GLfloat Distance(const SVector2Df& vec1, const SVector2Df& vec2);

	//////////////////////////////////////////////////////////////////
	/////////////////////////////VECTOR3//////////////////////////////
	//////////////////////////////////////////////////////////////////

	/**
	 * GLfloat Calculates the squared length (magnitude) of the SVector3Df object.
	 *
	 * @param vec The SVector3Df object to calculate it's squared length.
	 * @return The squared length of the vector.
	 */
	GLfloat LengthSquared(const SVector3Df& v3Vec);

	/**
	 * GLfloat Calculates the length (magnitude) of the SVector3Df object.
	 *
	 * @param vec The SVector3Df object to calculate it's length.
	 * @return The length of the vector.
	 */
	GLfloat Length(const SVector3Df& v3Vec);

	/**
	 * GLfloat Normalizes the SVector3Df object, making its length 1.
	 *
	 * @param vec The SVector3Df object to normalize.
	 * @return the modified SVector3Df object.
	 */
	SVector3Df Normalize(const SVector3Df& vec);

	/**
	 * GLfloat Calculates the dot product of two SVector3Df objects.
	 *
	 * @param vec1 The SVector3Df object to calculate the dot product with.
	 * @param vec2 The SVector3Df object to calculate the dot product with.
	 * @return The dot product of the two vectors.
	 */
	GLfloat Dot(const SVector3Df& vec1, const SVector3Df& vec2);

	/**
	 * @brief Calculates the cross product of two SVector3Df objects.
	 *
	 * @param vec1 The first SVector3Df object to calculate the cross product with.
	 * @param vec2 The second SVector3Df object to calculate the cross product with.
	 * @return The cross product of the two vectors.
	 */
	SVector3Df Cross(const SVector3Df& vec1, const SVector3Df& vec2);

	/**
	 * @brief Calculates the Euclidean distance between two SVector3Df objects.
	 *
	 * @param vec1 The SVector3Df object to calculate the distance from.
	 * @param vec2 The SVector3Df object to calculate the distance to.
	 * @return The distance between the two vectors.
	 */
	GLfloat Distance(const SVector3Df& vec1, const SVector3Df& vec2);

	/**
	 * @brief Calculates the squared Euclidean distance between two SVector3Df objects.
	 *
	 * @param vec1 The SVector3Df object to calculate the distance from.
	 * @param vec2 The SVector3Df object to calculate the distance to.
	 * @return The squared distance between the two vectors.
	 */
	GLfloat DistanceSquared(const SVector3Df& vec1, const SVector3Df& vec2);

	/**
	 * GLfloat Calculates the Angle between two SVector3Df objects.
	 *
	 * @param vec1 The first SVector3Df object to calculate the angle0 with.
	 * @param vec2 The second SVector3Df object to calculate the angle0 with.
	 * @return The angle between the two vectors.
	 */
	GLfloat Angle(const SVector3Df& vec1, const SVector3Df& vec2);

	/**
	 * Rotates the vector using quaternion by giving angle and vector.
	 *
	 * @param fAngle The Angle of Rotation.
	 * @param vec The vector to rotate around, could represent an Axis
	 * @return The rotated vector
	 */
	SVector3Df Rotate(const SVector3Df& vec, const GLfloat fAngle, const SVector3Df& v3Axis);

	//////////////////////////////////////////////////////////////////
	/////////////////////////////VECTOR4//////////////////////////////
	//////////////////////////////////////////////////////////////////
	/**
	 * GLfloat Calculates the squared length (magnitude) of the SVector4Df object.
	 *
	 * @param v4Vec The SVector4Df object to calculate it's squared length.
	 * @return The squared length of the vector.
	 */
	GLfloat LengthSquared(const SVector4Df& v4Vec);

	/**
	 * GLfloat Calculates the length (magnitude) of the SVector4Df object.
	 *
	 * @param v4Vec The SVector4Df object to calculate it's length.
	 * @return The length of the vector.
	 */
	GLfloat Length(const SVector4Df& v4Vec);

	/**
	 * GLfloat Normalizes the SVector4Df object, making its length 1.
	 *
	 * @param vec The SVector4Df object to normalize.
	 * @return the modified SVector4Df object.
	 */
	SVector4Df Normalize(const SVector4Df& vec);

	/**
	 * GLfloat Calculates the dot product of two SVector4Df objects.
	 *
	 * @param vec1 The SVector4Df object to calculate the dot product with.
	 * @param vec2 The SVector4Df object to calculate the dot product with.
	 * @return The dot product of the two vectors.
	 */
	GLfloat Dot(const SVector4Df& vec1, const SVector4Df& vec2);

	/**
	 * @brief Calculates the Euclidean distance between two SVector4Df objects.
	 *
	 * @param vec1 The SVector4Df object to calculate the distance from.
	 * @param vec2 The SVector4Df object to calculate the distance to.
	 * @return The distance between the two vectors.
	 */
	GLfloat Distance(const SVector4Df& vec1, const SVector4Df& vec2);

} // namespace EngineMath - Vector Part