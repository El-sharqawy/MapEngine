#pragma once 

#include <glad/glad.h>

/**
 * Structure to hold parameters for an orthogonal projection matrix
 *
 * This structure contains the parameters needed to define an orthographic projection matrix, which
 * is used when you want to render objects with no perspective distortion. In this projection, objects
 * appear the same size regardless of their distance from the camera, which is ideal for 2D views,
 * technical drawings, or isometric views.
 *
 * Members:
 *   - Right: The right plane of the orthogonal view frustum.
 *   - Left: The left plane of the orthogonal view frustum.
 *   - Bottom: The bottom plane of the orthogonal view frustum.
 *   - Top: The top plane of the orthogonal view frustum.
 *   - NearZ: The near clipping plane distance.
 *   - FarZ: The far clipping plane distance.
 *   - Width: The width of the view in world-space units.
 *   - Height: The height of the view in world-space units.
 */
typedef struct SOrthoProjInfo
{
	GLfloat Left;    /** The left plane of the orthogonal frustum */
	GLfloat Right;   /** The right plane of the orthogonal frustum */
	GLfloat Bottom;  /** The bottom plane of the orthogonal frustum */
	GLfloat Top;     /** The top plane of the orthogonal frustum */
	GLfloat zNear;   /** The near clipping plane distance */
	GLfloat zFar;    /** The far clipping plane distance */
	GLfloat Width;   /** The width of the orthogonal view in world-space units */
	GLfloat Height;  /** The height of the orthogonal view in world-space units */
} TOrthoProjInfo;
