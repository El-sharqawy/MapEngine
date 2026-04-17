#pragma once

#include <glad/glad.h>
#include <singleton.h>
#include <vector>

#include "TypeVector2.h"
#include "TypeVector3.h"
#include "TypeVector4.h"
#include "Vertex.h"

class CShader;

// Modify the signature to include a rendering mode
enum BoxRenderMode 
{
	WIREFRAME,
	SOLID
};

class CDebugRenderer : public CSingleton<CDebugRenderer>
{
public:
	/**
	 * @brief Constructor - initializes member variables.
	 */
	CDebugRenderer();

	/**
	 * @brief Destructor - cleans up all resources.
	 */
	~CDebugRenderer();
	
	/**
	 * @brief Cleans up OpenGL resources used by the debug renderer.
	 *
	 * This function deletes the Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
	 * associated with the debug renderer, as well as the shader program used for rendering.
	 * It should be called when the debug renderer is no longer needed to free up resources.
	 */
	void Destroy();

	/**
	 * @brief Clears all batched debug lines.
	 *
	 * This function removes all the lines that have been batched for rendering.
	 * It is typically called at the start of a new frame to reset the debug line data.
	 */
	void Clear();

	/**
	 * @brief Initializes the debug renderer by setting up OpenGL buffers and shaders.
	 *
	 * This function creates the necessary Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
	 * for rendering debug lines. It also compiles and links the shader program used for rendering.
	 * The function must be called before any rendering functions are used.
	 */
	void Initialize();

	/**
	 * @brief Renders all batched debug lines to the screen.
	 *
	 * This function processes all the lines that have been batched for rendering.
	 * It sets up the necessary OpenGL state, uploads
	 * the vertex data to the GPU, and issues draw calls for each group of lines.
	 * After rendering, it clears the batched lines to prepare for the next frame.
	 */
	void RenderAll();

	/**
	 * @brief Enables or disables debug rendering.
	 *
	 * This function sets a flag that determines whether debug lines should be rendered.
	 * When disabled, calls to RenderAll() will not draw any lines.
	 *
	 * @param bEnabled true to enable debug rendering, false to disable.
	 */
	void SetDebuggingEnabled(bool bEnabled);

	/**
	 * @brief Checks if debug rendering is enabled.
	 *
	 * @return true if debug rendering is enabled, false otherwise.
	 */
	bool IsDebuggingEnabled() const;

	/**
	 * @brief Sets the diffuse color used for rendering debug lines.
	 *
	 * @param v4Color The new diffuse color to set.
	 */
	void SetDiffuseColor(const Vector4D& v4Color);

	/**
	 * @brief Sets the diffuse color used for rendering debug lines.
	 *
	 * @param fR Red component of the color.
	 * @param fG Green component of the color.
	 * @param fB Blue component of the color.
	 * @param fA Alpha (opacity) component of the color.
	 */
	void SetDiffuseColor(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA);

	/**
	 * @brief Retrieves the current diffuse color used for rendering debug lines.
	 *
	 * @return Vector4D The current diffuse color.
	 */
	Vector4D GetDiffuseColor() const;

public:
	/**
	 * @brief Renders the primary axis lines (X, Y, Z) in the 3D space.
	 *
	 * This function draws three colored lines representing the X, Y, and Z axes.
	 * The X axis is drawn in red, the Y axis in green, and the Z axis in blue.
	 */
	void RenderAxisLines();

	/**
	 * @brief Renders a 2D line on a specified Y plane with given start and end points.
	 *
	 * This function adds a line defined by its start and end XZ coordinates
	 * at a specific Y height to the batch of lines to be rendered. The line
	 * is stored for later rendering in the RenderAll() function.
	 *
	 * @param sx        X coordinate of the start point.
	 * @param sz        Z coordinate of the start point.
	 * @param ex        X coordinate of the end point.
	 * @param ez        Z coordinate of the end point.
	 * @param y         Y coordinate (height) where the line will be rendered.
	 */
	void RenderLine2D(GLfloat sx, GLfloat sz, GLfloat ex, GLfloat ez, GLfloat y);

	/**
	 * @brief Renders a 2D line on a specified Y plane with given start and end points.
	 *
	 * This function adds a line defined by its start and end XZ coordinates
	 * at a specific Y height to the batch of lines to be rendered. The line
	 * is stored for later rendering in the RenderAll() function.
	 *
	 * @param v2StartPoint 2D vector representing the start point (X,Z) of the line.
	 * @param v2EndPoint   2D vector representing the end point (X,Z) of the line.
	 * @param fY          Y coordinate (height) where the line will be rendered.
	 */
	void RenderLine2D(const Vector2D& v2StartPoint, const Vector2D& v2EndPoint, GLfloat fY);

	/**
	 * @brief Renders a 3D line between two points with given start and end points.
	 *
	 * This function adds a line defined by its start and end coordinates to the
	 * batch of lines to be rendered. The line is stored
	 * for later rendering in the RenderAll() function.
	 *
	 * @param sx        X coordinate of the start point.
	 * @param sy        Y coordinate of the start point.
	 * @param sz        Z coordinate of the start point.
	 * @param ex        X coordinate of the end point.
	 * @param ey        Y coordinate of the end point.
	 * @param ez        Z coordinate of the end point.
	 */
	void RenderLine3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez);

	/**
	 * @brief Renders a 3D line between two points with given start and end points.
	 *
	 * This function adds a line defined by its start and end coordinates to the
	 * batch of lines to be rendered. The line is stored
	 * for later rendering in the RenderAll() function.
	 *
	 * @param v3StartPoint 3D vector representing the start point of the line.
	 * @param v3EndPoint   3D vector representing the end point of the line.
	 */
	void RenderLine3D(const Vector3D& v3StartPoint, const Vector3D& v3EndPoint);

	/**
	 * @brief Renders a circle in 2D space on a specified plane.
	 *
	 * This function draws a circle centered at (fX, fY, fZ) with a given radius.
	 * The circle can be rendered on either the horizontal (XZ) plane or vertical (XY) plane
	 * based on the bHorizonal parameter. The number of segments used to approximate
	 * the circle can be specified via iStep.
	 *
	 * @param fX         X coordinate of the circle center.
	 * @param fY         Y coordinate of the circle center.
	 * @param fZ         Z coordinate of the circle center.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 * @param bHorizonal true to render on horizontal (XZ) plane, false for vertical (XY) plane.
	 */
	void RenderCircle2D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep, bool bHorizonal = false);

	/**
	 * @brief Renders a circle in 2D space on a specified plane.
	 *
	 * This function draws a circle centered at the given position with a specified radius.
	 * The circle can be rendered on either the horizontal (XZ) plane or vertical (XY) plane
	 * based on the bHorizonal parameter. The number of segments used to approximate
	 * the circle can be specified via iStep.
	 *
	 * @param v3Position Center position of the circle.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 * @param bHorizonal true to render on horizontal (XZ) plane, false for vertical (XY) plane.
	 */
	void RenderCircle2D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep, bool bHorizonal = false);

	/**
	 * @brief Renders a circle in 3D space facing the camera.
	 *
	 * This function draws a circle centered at (fX, fY, fZ) with a given radius.
	 * The circle is oriented to always face the camera using billboarding technique.
	 * The number of segments used to approximate the circle can be specified via iStep.
	 *
	 * @param fX         X coordinate of the circle center.
	 * @param fY         Y coordinate of the circle center.
	 * @param fZ         Z coordinate of the circle center.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderCircle3D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a circle in 3D space facing the camera.
	 *
	 * This function draws a circle centered at the given position with a specified radius.
	 * The circle is oriented to always face the camera using billboarding technique.
	 * The number of segments used to approximate the circle can be specified via iStep,
	 *
	 * @param v3Position Center position of the circle.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderCircle3D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a solid circle in 2D space on a specified plane.
	 *
	 * This function draws a solid circle centered at (fX, fY, fZ) with a given radius.
	 * The circle is filled and can be approximated using the specified number of segments (iStep).
	 *
	 * @param fX         X coordinate of the circle center.
	 * @param fY         Y coordinate of the circle center.
	 * @param fZ         Z coordinate of the circle center.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderSolidCircle2D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a solid circle in 2D space on a specified plane.
	 *
	 * This function draws a solid circle centered at the given position with a specified radius.
	 * The circle is filled and can be approximated using the specified number of segments (iStep).
	 *
	 * @param v3Position Center position of the circle.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderSolidCircle2D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a solid circle in 3D space facing the camera.
	 *
	 * This function draws a solid circle centered at (fX, fY, fZ) with a given radius.
	 * The circle is filled and oriented to always face the camera using billboarding technique.
	 * The number of segments used to approximate the circle can be specified via iStep.
	 *
	 * @param fX         X coordinate of the circle center.
	 * @param fY         Y coordinate of the circle center.
	 * @param fZ         Z coordinate of the circle center.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderSolidCircle3D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a solid circle in 3D space facing the camera.
	 *
	 * This function draws a solid circle centered at the given position with a specified radius.
	 * The circle is filled and oriented to always face the camera using billboarding technique.
	 * The number of segments used to approximate the circle can be specified via iStep.
	 *
	 * @param v3Position Center position of the circle.
	 * @param fRadius    Radius of the circle.
	 * @param iStep      Number of segments to approximate the circle.
	 */
	void RenderSolidCircle3D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep);

	/**
	 * @brief Renders a 3D square between two points with specified start and end points.
	 *
	 * This function adds a square defined by its start and end coordinates to the
	 * batch of lines to be rendered. The square is stored
	 * for later rendering in the RenderAll() function.
	 *
	 * @param sx        X coordinate of the start point.
	 * @param sy        Y coordinate of the start point.
	 * @param sz        Z coordinate of the start point.
	 * @param ex        X coordinate of the end point.
	 * @param ey        Y coordinate of the end point.
	 * @param ez        Z coordinate of the end point.
	 * @param renderMode The rendering mode for the box (WIREFRAME or SOLID).
	 */
	void RenderSquare3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez, const BoxRenderMode& renderMode = BoxRenderMode::SOLID);

	/**
	 * @brief Renders a 3D square between two points with specified start and end points..
	 *
	 * This function adds a square defined by its start and end coordinates to the
	 * batch of lines to be rendered. The square is stored
	 * for later rendering in the RenderAll() function.
	 *
	 * @param v3Start 3D vector representing the start point of the square.
	 * @param v3End   3D vector representing the end point of the square.
	 * @param renderMode The rendering mode for the box (WIREFRAME or SOLID).
	 */
	void RenderSquare3D(const Vector3D& v3Start, const Vector3D& v3End, const BoxRenderMode& renderMode = BoxRenderMode::SOLID);

	/**
	 * @brief Renders a 3D box defined by two corner points with specified start and end points..
	 *
	 * This function adds the edges of a box defined by its start and end corner coordinates
	 * to the batch of lines to be rendered. The box is represented by its 12 edges,
	 * and each edge is stored for later rendering in the RenderAll() function.
	 *
	 * @param sx        X coordinate of the start corner.
	 * @param sy        Y coordinate of the start corner.
	 * @param sz        Z coordinate of the start corner.
	 * @param ex        X coordinate of the end corner.
	 * @param ey        Y coordinate of the end corner.
	 * @param ez        Z coordinate of the end corner.
	 * @param renderMode The rendering mode for the box (WIREFRAME or SOLID).
	 */
	void RenderBox3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez, const BoxRenderMode& renderMode = BoxRenderMode::SOLID);

	/**
	 * @brief Renders a 3D box defined by two corner points with specified start and end points..
	 *
	 * This function adds the edges of a box defined by its start and end corner coordinates
	 * to the batch of lines to be rendered. The box is represented by its 12 edges,
	 * and each edge is stored for later rendering in the RenderAll() function.
	 *
	 * @param v3Start 3D vector representing the start corner of the box.
	 * @param v3End   3D vector representing the end corner of the box.
	 * @param renderMode The rendering mode for the box (WIREFRAME or SOLID).
	 */
	void RenderBox3D(const Vector3D& v3Start, const Vector3D& v3End, const BoxRenderMode& renderMode = BoxRenderMode::SOLID);

	/**
	 * @brief Renders a 3D sphere at a specified center with given radius.
	 *
	 * This function draws a sphere centered at (fCenterX, fCenterY, fCenterZ) with a specified radius.
	 * The sphere is approximated using the specified number of segments and slices.
	 *
	 * @param fCenterX   X coordinate of the sphere center.
	 * @param fCenterY   Y coordinate of the sphere center.
	 * @param fCenterZ   Z coordinate of the sphere center.
	 * @param fRadius    Radius of the sphere.
	 * @param iSegments  Latitude, This determines the vertical resolution (from pole to pole). 128 is an extremely high number and provides excellent vertical smoothness.
	 * @param iSlices    Longitude, This determines the horizontal resolution (around the equator). 128 is an extremely high number and provides excellent horizontal smoothness.
	 */
	void RenderSphere3D(GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat fRadius, GLint iSegments, GLint iSlices);

	/**
	 * @brief Renders a 3D sphere at a specified center with given radius.
	 *
	 * This function draws a sphere centered at the given position with a specified radius.
	 * The sphere is approximated using the specified number of segments and slices.
	 *
	 * @param v3Center   Center position of the sphere.
	 * @param fRadius    Radius of the sphere.
	 * @param iSegments  Latitude, This determines the vertical resolution (from pole to pole). 128 is an extremely high number and provides excellent vertical smoothness.
	 * @param iSlices    Longitude, This determines the horizontal resolution (around the equator). 128 is an extremely high number and provides excellent horizontal smoothness.
	 */
	void RenderSphere3D(const Vector3D& v3Center, GLfloat fRadius, GLint iSegments, GLint iSlices);

	/**
	 * @brief Renders a wireframe 3D sphere at a specified center with given radius.
	 *
	 * This function draws a wireframe sphere centered at (fCenterX, fCenterY, fCenterZ) with a specified radius.
	 * The sphere is approximated using the specified number of segments and slices.
	 *
	 * @param fCenterX   X coordinate of the sphere center.
	 * @param fCenterY   Y coordinate of the sphere center.
	 * @param fCenterZ   Z coordinate of the sphere center.
	 * @param fRadius    Radius of the sphere.
	 * @param iSegments  Latitude, This determines the vertical resolution (from pole to pole). 128 is an extremely high number and provides excellent vertical smoothness.
	 * @param iSlices    Longitude, This determines the horizontal resolution (around the equator). 128 is an extremely high number and provides excellent horizontal smoothness.
	 */
	void RenderWireSphere3D(GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat fRadius, GLint iSegments, GLint iSlices);

	/**
	 * @brief Renders a wireframe 3D sphere at a specified center with given radius.
	 *
	 * This function draws a wireframe sphere centered at the given position with a specified radius.
	 * The sphere is approximated using the specified number of segments and slices.
	 *
	 * @param v3Center   Center position of the sphere.
	 * @param fRadius    Radius of the sphere.
	 * @param iSegments  Latitude, This determines the vertical resolution (from pole to pole). 128 is an extremely high number and provides excellent vertical smoothness.
	 * @param iSlices    Longitude, This determines the horizontal resolution (around the equator). 128 is an extremely high number and provides excellent horizontal smoothness.
	 */
	void RenderWireSphere3D(const Vector3D& v3Center, GLfloat fRadius, GLint iSegments, GLint iSlices);

	/**
	 * @brief Draws a crosshair at the center of the screen.
	 *
	 * This function calculates the position in front of the camera and
	 * draws a simple crosshair using two intersecting lines.
	 */
	void DrawCrosshair();

private:
	// Initialization Flag
	bool m_bInitialized;
	
	// Debugging Enabled Flag
	bool m_bIsDebuggingEnabled;

	// Default Diffuse Color for Debug Lines
	Vector4D m_v4DiffuseColor;

	// Wireframe Box Batch (Drawn using GL_LINES)
	std::vector<SLinesVertex> m_vWireframeVertices;
	std::vector<GLuint> m_vWireframeIndices;

	// Solid Box Batch (Drawn using GL_TRIANGLES)
	std::vector<SLinesVertex> m_vSolidVertices;
	std::vector<GLuint> m_vSolidIndices;
};