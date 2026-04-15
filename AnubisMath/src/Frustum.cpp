#include "Stdafx.h"
#include "Frustum.h"
#include "TypeMatrix4.h"
#include "TypeVector4.h"
#include "EngineMathMatrix.h"

/**
 * @brief Initializes the frustum corners from the inverse view-projection matrix.
 *
 * This function calculates the eight corners of the frustum in world space
 * by transforming the normalized device coordinates (NDC) corners using the
 * provided inverse view-projection matrix. It performs a perspective divide
 * to convert from homogeneous coordinates to 3D coordinates.
 *
 * @param matInvViewProj The inverse view-projection matrix.
 */
void SFrustum::InitializeFromMatrix(const Matrix4& matInvViewProj)
{
	// Points in Clip Space (NDC)
	// Format: Matrix * Vector for Column-Major
	v4NearTopLeft = matInvViewProj * SVector4Df(-1.0f, 1.0f, -1.0f, 1.0f);
	v4NearBottomLeft = matInvViewProj * SVector4Df(-1.0f, -1.0f, -1.0f, 1.0f);
	v4NearTopRight = matInvViewProj * SVector4Df(1.0f, 1.0f, -1.0f, 1.0f);
	v4NearBottomRight = matInvViewProj * SVector4Df(1.0f, -1.0f, -1.0f, 1.0f);

	v4FarTopLeft = matInvViewProj * SVector4Df(-1.0f, 1.0f, 1.0f, 1.0f);
	v4FarBottomLeft = matInvViewProj * SVector4Df(-1.0f, -1.0f, 1.0f, 1.0f);
	v4FarTopRight = matInvViewProj * SVector4Df(1.0f, 1.0f, 1.0f, 1.0f);
	v4FarBottomRight = matInvViewProj * SVector4Df(1.0f, -1.0f, 1.0f, 1.0f);

	// Perform perspective divide to convert from homogeneous coordinates to 3D coordinates (Crucial for World Space)
	v4NearTopLeft /= v4NearTopLeft.w;
	v4NearBottomLeft /= v4NearBottomLeft.w;
	v4NearTopRight /= v4NearTopRight.w;
	v4NearBottomRight /= v4NearBottomRight.w;

	v4FarTopLeft /= v4FarTopLeft.w;
	v4FarBottomLeft /= v4FarBottomLeft.w;
	v4FarTopRight /= v4FarTopRight.w;
	v4FarBottomRight /= v4FarBottomRight.w;
}

/**
 * @brief Transforms the frustum corners using the inverse view-projection matrix.
 *
 * This function applies the provided inverse view-projection matrix to each
 * corner of the frustum, transforming them from clip space to world space.
 * It also performs a perspective divide to convert from homogeneous coordinates
 * to 3D coordinates.
 *
 * @param matInvViewProj The inverse view-projection matrix.
 */
void SFrustum::Transform(const Matrix4& matInvViewProj)
{
	// 1. Multiply by Inverse Matrix
	v4NearTopLeft = matInvViewProj * v4NearTopLeft;
	v4NearBottomLeft = matInvViewProj * v4NearBottomLeft;
	v4NearTopRight = matInvViewProj * v4NearTopRight;
	v4NearBottomRight = matInvViewProj * v4NearBottomRight;

	v4FarTopLeft = matInvViewProj * v4FarTopLeft;
	v4FarBottomLeft = matInvViewProj * v4FarBottomLeft;
	v4FarTopRight = matInvViewProj * v4FarTopRight;
	v4FarBottomRight = matInvViewProj * v4FarBottomRight;

	// 2. Perspective Divide (CRITICAL for OpenGL)
	// This converts from 4D Homogeneous space to 3D World Space
	v4NearTopLeft /= v4NearTopLeft.w;
	v4NearBottomLeft /= v4NearBottomLeft.w;
	v4NearTopRight /= v4NearTopRight.w;
	v4NearBottomRight /= v4NearBottomRight.w;

	v4FarTopLeft /= v4FarTopLeft.w;
	v4FarBottomLeft /= v4FarBottomLeft.w;
	v4FarTopRight /= v4FarTopRight.w;
	v4FarBottomRight /= v4FarBottomRight.w;
}

/**
 * @brief Constructs a frustum culling object from the view-projection matrix.
 *
 * This constructor initializes the frustum clipping planes by calling
 * the Update method with the provided view-projection matrix.
 *
 * @param matViewProj The combined view-projection matrix.
 */
SFrustumCulling::SFrustumCulling(const Matrix4& matViewProj)
{
	Update(matViewProj);
}

/**
 * @brief Updates the frustum clipping planes from the view-projection matrix.
 *
 * This function recalculates the six clipping planes (left, right, top,
 * bottom, near, far) of the frustum using the provided view-projection matrix.
 * It utilizes the EngineMath::CalculateClipPlanes function to perform the
 * extraction of the planes.
 *
 * @param matViewProj The combined view-projection matrix.
 */
void SFrustumCulling::Update(const Matrix4& matViewProj)
{
	EngineMath::CalculateClipPlanes(matViewProj,
		v4LeftClipPlane,
		v4RightClipPlane,
		v4TopClipPlane,
		v4BottomClipPlane,
		v4NearClipPlane,
		v4FarClipPlane);
}

/**
 * @brief Checks if a 3D point is inside the view frustum.
 *
 * This function tests whether the given 3D point lies within the six
 * clipping planes of the view frustum by converting it to homogeneous
 * coordinates and performing dot product tests.
 *
 * @param v3Point The 3D point to test.
 * @return true if the point is inside the frustum; false otherwise.
 */
bool SFrustumCulling::IsPointInsideViewFrustum(const SVector3Df& v3Point) const
{
	SVector4Df v4Point(v3Point, 1.0f);

	// A negative epsilon creates a tiny "safety buffer" so objects 
	// don't flicker right at the edge of the screen.
	const GLfloat fEpsilon = -0.0001f;

	// Check Near & Far first since it's the most common to cull objects
	// 1. Near (Z-Axis) - Culls everything behind the camera
	if (v4NearClipPlane.dot(v4Point) < fEpsilon)
	{
		return false;
	}

	// 2. Left/Right (X-Axis) - Culls during horizontal rotation
	if (v4LeftClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	if (v4RightClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	// 3. Top/Bottom (Y-Axis) - Culls during vertical looking
	if (v4TopClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	if (v4BottomClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	// 4. Far (Z-Axis) - Culls based on view distance
	if (v4FarClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	return (true);
}

/**
 * @brief Checks if a 4D point is inside the view frustum.
 *
 * This function tests whether the given 4D point (in homogeneous coordinates)
 * lies within the six clipping planes of the view frustum. It uses a small
 * negative epsilon to create a safety buffer, preventing flickering at the edges.
 *
 * @param v4Point The 4D point to test.
 * @return true if the point is inside the frustum; false otherwise.
 */
bool SFrustumCulling::IsPointInsideViewFrustum(const SVector4Df& v4Point) const
{
	// A negative epsilon creates a tiny "safety buffer" so objects 
	// don't flicker right at the edge of the screen.
	const GLfloat fEpsilon = -0.0001f;

	// Check Near & Far first since it's the most common to cull objects
	// 1. Near (Z-Axis) - Culls everything behind the camera
	if (v4NearClipPlane.dot(v4Point) < fEpsilon)
	{
		return false;
	}

	// 2. Left/Right (X-Axis) - Culls during horizontal rotation
	if (v4LeftClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	if (v4RightClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	// 3. Top/Bottom (Y-Axis) - Culls during vertical looking
	if (v4TopClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	if (v4BottomClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	// 4. Far (Z-Axis) - Culls based on view distance
	if (v4FarClipPlane.dot(v4Point) < fEpsilon)
	{
		return (false);
	}

	return (true);
}
