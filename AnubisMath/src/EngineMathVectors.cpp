#include "Stdafx.h"
#include "EngineMathVectors.h"

//////////////////////////////////////////////////////////////////
/////////////////////////////VECTOR2//////////////////////////////
//////////////////////////////////////////////////////////////////

/**
 * GLfloat Calculates the squared length (magnitude) of the SVector2Df object.
 *
 * @param vec The SVector2Df object to calculate it's squared length.
 * @return The squared length of the vector.
 */
GLfloat EngineMath::LengthSquared(const SVector2Df& v3Vec)
{
	return (v3Vec.x * v3Vec.x + v3Vec.y * v3Vec.y);
}

/**
 * GLfloat Calculates the length (magnitude) of the SVector2Df object.
 *
 * @param vec The SVector2Df object to calculate it's length.
 * @return The length of the vector.
 */
GLfloat EngineMath::Length(const SVector2Df& v2Vec)
{
	return std::sqrt(LengthSquared(v2Vec)); // Calculates the length (magnitude) of the vector
}

/**
 * GLfloat Normalizes the SVector2Df object, making its length 1.
 *
 * @param vec The SVector2Df object to normalize.
 * @return the modified SVector2Df object.
 */
SVector2Df EngineMath::Normalize(const SVector2Df& vec)
{
	SVector2Df normalizedVec = vec;
	GLfloat fLenSq = EngineMath::LengthSquared(vec);
	if (fLenSq > VECTOR_EPSILON)
	{
		GLfloat fInvLen = 1.0f / std::sqrt(fLenSq);
		normalizedVec.x *= fInvLen;
		normalizedVec.y *= fInvLen;
	}
	else
	{
		return SVector2Df(0.0f, 0.0f);
	}
	return (normalizedVec);
}

/**
 * GLfloat Calculates the dot product of two SVector2Df objects.
 *
 * @param vec1 The SVector2Df object to calculate the dot product with.
 * @param vec2 The SVector2Df object to calculate the dot product with.
 * @return The dot product of the two vectors.
 */
GLfloat EngineMath::Dot(const SVector2Df& vec1, const SVector2Df& vec2)
{
	GLfloat dRet = vec1.x * vec2.x + vec1.y * vec2.y;
	return (dRet); // Calculates the dot product of two vector objects
}

/**
 * @brief Calculates the Euclidean distance between two SVector2Df objects.
 *
 * @param vec1 The SVector2Df object to calculate the distance from.
 * @param vec2 The SVector2Df object to calculate the distance to.
 * @return The distance between the two vectors.
 */
GLfloat EngineMath::Distance(const SVector2Df& vec1, const SVector2Df& vec2)
{
	GLfloat delta_x = vec1.x - vec2.x;
	GLfloat delta_y = vec1.y - vec2.y;

	GLfloat distance = std::sqrt(delta_x * delta_x + delta_y * delta_y);
	return (distance);
}

//////////////////////////////////////////////////////////////////
/////////////////////////////VECTOR3//////////////////////////////
//////////////////////////////////////////////////////////////////

/**
 * GLfloat Calculates the squared length (magnitude) of the SVector3Df object.
 *
 * @param vec The SVector3Df object to calculate it's squared length.
 * @return The squared length of the vector.
 */
GLfloat EngineMath::LengthSquared(const SVector3Df& v3Vec)
{
    return (v3Vec.x * v3Vec.x + v3Vec.y * v3Vec.y + v3Vec.z * v3Vec.z);
}

/**
 * GLfloat Calculates the length (magnitude) of the SVector3Df object.
 *
 * @param vec The SVector3Df object to calculate it's length.
 * @return The length of the vector.
 */
GLfloat EngineMath::Length(const SVector3Df& v3Vec)
{
	return std::sqrt(LengthSquared(v3Vec)); // Calculates the length (magnitude) of the vector
}

/**
 * GLfloat Normalizes the SVector3Df object, making its length 1.
 *
 * @param vec The SVector3Df object to normalize.
 * @return the modified SVector3Df object.
 */
SVector3Df EngineMath::Normalize(const SVector3Df& vec)
{
	SVector3Df normalizedVec = vec;
	GLfloat fLenSq = EngineMath::LengthSquared(vec);
	if (fLenSq > VECTOR_EPSILON)
	{
		GLfloat fInvLen = 1.0f / std::sqrt(fLenSq);
		normalizedVec.x *= fInvLen;
		normalizedVec.y *= fInvLen;
		normalizedVec.z *= fInvLen;
	}
	else
	{
		return SVector3Df(0.0f, 0.0f, 0.0f);
	}
	return (normalizedVec);
}

/**
 * GLfloat Calculates the dot product of two SVector3Df objects.
 *
 * @param vec1 The SVector3Df object to calculate the dot product with.
 * @param vec2 The SVector3Df object to calculate the dot product with.
 * @return The dot product of the two vectors.
 */
GLfloat EngineMath::Dot(const SVector3Df& vec1, const SVector3Df& vec2)
{
	GLfloat dRet = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	return (dRet); // Calculates the dot product of two vector objects
}

/**
 * @brief Calculates the cross product of two SVector3Df objects.
 *
 * @param vec1 The first SVector3Df object to calculate the cross product with.
 * @param vec2 The second SVector3Df object to calculate the cross product with.
 * @return The cross product of the two vectors.
 */
SVector3Df EngineMath::Cross(const SVector3Df& vec1, const SVector3Df& vec2)
{
	// Cross Product formula:
	// R.x = (vec1.y * vec2.z) - (vec1.z * vec2.y)
	// R.y = (vec1.z * vec2.x) - (vec1.x * vec2.z)
	// R.z = (vec1.x * vec2.y) - (vec1.y * vec2.x)
	GLfloat fX = vec1.y * vec2.z - vec1.z * vec2.y;
	GLfloat fY = vec1.z * vec2.x - vec1.x * vec2.z;
	GLfloat fZ = vec1.x * vec2.y - vec1.y * vec2.x;

	return (SVector3Df(fX, fY, fZ));
}

/**
 * @brief Calculates the Euclidean distance between two SVector3Df objects.
 *
 * @param vec1 The SVector3Df object to calculate the distance from.
 * @param vec2 The SVector3Df object to calculate the distance to.
 * @return The distance between the two vectors.
 */
GLfloat EngineMath::Distance(const SVector3Df& vec1, const SVector3Df& vec2)
{
	GLfloat delta_x = vec1.x - vec2.x;
	GLfloat delta_y = vec1.y - vec2.y;
	GLfloat delta_z = vec1.z - vec2.z;

	GLfloat distance = std::sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
	return (distance);
}

/**
 * @brief Calculates the squared Euclidean distance between two SVector3Df objects.
 *
 * @param vec1 The SVector3Df object to calculate the distance from.
 * @param vec2 The SVector3Df object to calculate the distance to.
 * @return The squared distance between the two vectors.
 */
GLfloat EngineMath::DistanceSquared(const SVector3Df& vec1, const SVector3Df& vec2)
{
	Vector3D diff = vec1 - vec2;
	// Dot product of a vector with itself is the square of its magnitude
	return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
}

/**
 * GLfloat Calculates the Angle between two SVector3Df objects.
 *
 * @param vec1 The first SVector3Df object to calculate the angle0 with.
 * @param vec2 The second SVector3Df object to calculate the angle0 with.
 * @return The angle between the two vectors.
 */
GLfloat EngineMath::Angle(const SVector3Df& vec1, const SVector3Df& vec2)
{
	// 1. Calculate squared lengths first
	GLfloat lenSq1 = LengthSquared(vec1);
	GLfloat lenSq2 = LengthSquared(vec2);

	// 2. Use Epsilon check on the product of squared lengths
	// This is safer and catches cases where vectors are nearly zero
	if (lenSq1 < VECTOR_EPSILON || lenSq2 < VECTOR_EPSILON)
	{
		return (0.0f); // Angle is undefined for zero-length vectors
	}

	// 3. Calculate cosTheta
	// Mathematically: cos(theta) = (A . B) / (|A| * |B|)
	GLfloat dot = Dot(vec1, vec2);
	GLfloat cosTheta = dot / std::sqrt(lenSq1 * lenSq2);

	// 4. Clamp to avoid domain errors in acos due to float precision
	// If cosTheta is 1.0000001, acos returns NaN
	cosTheta = std::fmax(-1.0f, std::fmin(1.0f, cosTheta));

	return std::acos(cosTheta); // Returns result in radians
}

/**
 * Rotates the vector using quaternion by giving angle and vector.
 *
 * @param fAngle The Angle of Rotation.
 * @param vec The vector to rotate around, could represent an Axis
 * @return The rotated vector
 */
SVector3Df EngineMath::Rotate(const SVector3Df& vec, const GLfloat fAngle, const SVector3Df& v3Axis)
{
    // 1. Normalize the rotation axis.
    SVector3Df normalizedAxis = Normalize(v3Axis);

    // 2. Create the rotation quaternion 'q' from the axis and angle.
    // Formula: q = [ cos(angle/2), sin(angle/2) * axis ]
    GLfloat halfAngle = fAngle / 2.0f;
    GLfloat sinHalf = std::sin(halfAngle);
    GLfloat cosHalf = std::cos(halfAngle);

    SQuaternion q;
    q.w = cosHalf;
    q.x = normalizedAxis.x * sinHalf;
    q.y = normalizedAxis.y * sinHalf;
    q.z = normalizedAxis.z * sinHalf;

    // Note: If you have an SQuaternion::FromAxisAngle(axis, angle) method, use it here:
    // SQuaternion q = SQuaternion::FromAxisAngle(v3Axis, fAngle);


    // 3. Create the inverse/conjugate quaternion 'q_inverse'.
    // For a unit quaternion, q_inverse = q_conjugate.
    SQuaternion q_inverse = q.Conjugate();


    // 4. Create a pure quaternion 'p' representing the vector vec.
    // p = [ 0, vec.x, vec.y, vec.z ]
    SQuaternion p(0.0f, vec.x, vec.y, vec.z);


    // 5. Apply the rotation using the sandwich product: v' = q * p * q_inverse
    // This requires two quaternion multiplications.
    SQuaternion qp = q * p;
    SQuaternion rotatedQuat = qp * q_inverse;


    // 6. Extract the resulting vector from the rotated quaternion.
    // The rotated vector is in the vector part (x, y, z) of the result.
    return SVector3Df(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
}

//////////////////////////////////////////////////////////////////
/////////////////////////////VECTOR4//////////////////////////////
//////////////////////////////////////////////////////////////////
/**
 * GLfloat Calculates the squared length (magnitude) of the SVector4Df object.
 *
 * @param v4Vec The SVector4Df object to calculate it's squared length.
 * @return The squared length of the vector.
 */
GLfloat EngineMath::LengthSquared(const SVector4Df& v4Vec)
{
	return (v4Vec.x * v4Vec.x + v4Vec.y * v4Vec.y + v4Vec.z * v4Vec.z + v4Vec.w * v4Vec.w);
}

/**
 * GLfloat Calculates the length (magnitude) of the SVector4Df object.
 *
 * @param vec The SVector4Df object to calculate it's length.
 * @return The length of the vector.
 */
GLfloat EngineMath::Length(const SVector4Df& v4Vec)
{
	return std::sqrt(LengthSquared(v4Vec));;
}

/**
 * GLfloat Normalizes the SVector4Df object, making its length 1.
 *
 * @param vec The SVector4Df object to normalize.
 * @return the modified SVector4Df object.
 */
SVector4Df EngineMath::Normalize(const SVector4Df& vec)
{
	SVector4Df normalizedVec = vec;
	GLfloat fLenSq = EngineMath::LengthSquared(vec);
	if (fLenSq > VECTOR_EPSILON)
	{
		GLfloat fInvLen = 1.0f / std::sqrt(fLenSq);
		normalizedVec.x *= fInvLen;
		normalizedVec.y *= fInvLen;
		normalizedVec.z *= fInvLen;
		normalizedVec.w *= fInvLen;
	}
	else
	{
		return SVector4Df(0.0f, 0.0f, 0.0f, 0.0f);
	}
	return (normalizedVec);
}

/**
 * GLfloat Calculates the dot product of two SVector4Df objects.
 *
 * @param vec1 The SVector4Df object to calculate the dot product with.
 * @param vec2 The SVector4Df object to calculate the dot product with.
 * @return The dot product of the two vectors.
 */
GLfloat EngineMath::Dot(const SVector4Df& vec1, const SVector4Df& vec2)
{
	GLfloat dRet = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
	return (dRet); // Calculates the dot product of two vector objects
}

/**
 * @brief Calculates the Euclidean distance between two SVector4Df objects.
 *
 * @param vec1 The SVector4Df object to calculate the distance from.
 * @param vec2 The SVector4Df object to calculate the distance to.
 * @return The distance between the two vectors.
 */
GLfloat EngineMath::Distance(const SVector4Df& vec1, const SVector4Df& vec2)
{
	GLfloat delta_x = vec1.x - vec2.x;
	GLfloat delta_y = vec1.y - vec2.y;
	GLfloat delta_z = vec1.z - vec2.z;
	GLfloat delta_w = vec1.w - vec2.w;

	GLfloat distance = std::sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z + delta_w * delta_w);
	return (distance);
}