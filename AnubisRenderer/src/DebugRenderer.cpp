#include "Stdafx.h"
#include "DebugRenderer.h"
#include "RendererManager.h"
#include "MathUtils.h"
#include "EngineMathMatrix.h"
#include "TypeMatrix4.h"
#include "Camera.h"
#include "CameraManager.h"

/**
 * @brief Constructor - initializes member variables.
 */
CDebugRenderer::CDebugRenderer()
{
	m_bInitialized = false;
	m_bIsDebuggingEnabled = true;

	m_v4DiffuseColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f); // White by default
}

/**
 * @brief Destructor - cleans up all resources.
 */
CDebugRenderer::~CDebugRenderer()
{
	Destroy();
}

/*
* @brief Clears all batched debug lines.
*
* This function removes all the lines that have been batched for rendering.
* It is typically called at the start of a new frame to reset the debug line data.
*/
void CDebugRenderer::Destroy()
{
	Clear();
}

/** 
 * @brief Clears all batched debug lines.
 *
 * This function removes all the lines that have been batched for rendering.
 * It is typically called at the start of a new frame to reset the debug line data.
 */
void CDebugRenderer::Clear()
{
	// Clear all the collected line vertices from the CPU-side map.
	// This resets the state for the next frame's data collection.
	m_vSolidVertices.clear(); // Clear solid box vertices
	m_vSolidIndices.clear(); // Clear solid box indices

	m_vWireframeVertices.clear(); // Clear wireframe box vertices
	m_vWireframeIndices.clear(); // Clear wireframe box indices

	// m_bInitialized = false; // Mark as uninitialized
}

/**
 * @brief Initializes the debug renderer by setting up Rendering Positions.
 */
void CDebugRenderer::Initialize()
{
	m_bInitialized = true;
}

/**
 * @brief Renders all batched debug lines to the screen.
 *
 * This function processes all the lines that have been batched for rendering,
 * grouped by their thickness. It sets up the necessary OpenGL state, uploads
 * the vertex data to the GPU, and issues draw calls for each group of lines.
 * After rendering, it clears the batched lines to prepare for the next frame.
 */
void CDebugRenderer::RenderAll()
{
	// If debugging is disabled, skip rendering.
	if (!m_bInitialized || !m_bIsDebuggingEnabled)
	{
		return;
	}

	if (m_bIsDebuggingEnabled)
	{
		RenderAxisLines();
	}

	// --- Phase 1 & 2: All Line-Based Geometry ---
	// This now includes raw lines, wireframe boxes, grids, and axes.
	if (!m_vWireframeVertices.empty())
	{
		// No more manual offset math! The manager treats this as a fresh upload to the dynamic buffer.
		CRendererManager::Instance().RenderDynamicBatchDebugging(GL_LINES, m_vWireframeVertices.data(), m_vWireframeVertices.size(), m_vWireframeIndices.data(), m_vWireframeIndices.size());
	}

	// --- Phase 3: All Triangle-Based Geometry ---
	// We MUST keep this separate because GL_TRIANGLES is a different state than GL_LINES.
	if (!m_vSolidVertices.empty())
	{
		CRendererManager::Instance().RenderDynamicBatchDebugging(GL_TRIANGLES, m_vSolidVertices.data(), m_vSolidVertices.size(), m_vSolidIndices.data(), m_vSolidIndices.size());
	}

	Clear();
}

/**
 * @brief Enables or disables debug rendering.
 *
 * This function sets a flag that determines whether debug lines should be rendered.
 * When disabled, calls to RenderAll() will not draw any lines.
 *
 * @param bEnabled true to enable debug rendering, false to disable.
 */
void CDebugRenderer::SetDebuggingEnabled(bool bEnabled) 
{
	m_bIsDebuggingEnabled = bEnabled;
}

/**
 * @brief Checks if debug rendering is enabled.
 *
 * @return true if debug rendering is enabled, false otherwise.
 */
bool CDebugRenderer::IsDebuggingEnabled() const
{
	return m_bIsDebuggingEnabled;
}

/**
 * @brief Sets the diffuse color used for rendering debug lines.
 *
 * @param v4Color The new diffuse color to set.
 */
void CDebugRenderer::SetDiffuseColor(const Vector4D& v4Color)
{
	m_v4DiffuseColor = v4Color;
}

/**
 * @brief Sets the diffuse color used for rendering debug lines.
 *
 * @param fR Red component of the color.
 * @param fG Green component of the color.
 * @param fB Blue component of the color.
 * @param fA Alpha (opacity) component of the color.
 */
void CDebugRenderer::SetDiffuseColor(GLfloat fR, GLfloat fG, GLfloat fB, GLfloat fA)
{
	m_v4DiffuseColor = Vector4D(fR, fG, fB, fA);
}

/**
 * @brief Retrieves the current diffuse color used for rendering debug lines.
 *
 * @return Vector4D The current diffuse color.
 */
Vector4D CDebugRenderer::GetDiffuseColor() const
{
	return m_v4DiffuseColor;
}

///////////////////////// LINES RENDERING /////////////////////////
/**
 * @brief Renders the primary axis lines (X, Y, Z) in the 3D space.
 *
 * This function draws three colored lines representing the X, Y, and Z axes.
 * The X axis is drawn in red, the Y axis in green, and the Z axis in blue.
 */
void CDebugRenderer::RenderAxisLines()
{
	// X-Axis (Red)
	SetDiffuseColor(Vector4D(1.0f, 0.0f, 0.0f, 1.0f)); // Red color for X Axis
	RenderLine3D(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(100.0f, 0.0f, 0.0f)); // Start (Origin) - End

	// Y-Axis (Green)
	SetDiffuseColor(Vector4D(0.0f, 1.0f, 0.0f, 1.0f)); // Green color for Y Axis
	RenderLine3D(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 100.0f, 0.0f)); // Start (Origin) - End

	// Z-Axis (Blue)
	SetDiffuseColor(Vector4D(0.0f, 0.0f, 1.0f, 1.0f)); // Blue color for Z Axis
	RenderLine3D(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(0.0f, 0.0f, 100.0f)); // Start (Origin) - End

	SetDiffuseColor(1.0f); // Reset to white
}

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
void CDebugRenderer::RenderLine2D(GLfloat sx, GLfloat sz, GLfloat ex, GLfloat ez, GLfloat y)
{
	RenderLine3D(sx, y, sz, ex, y, ez);
}

/**
 * @brief Renders a 2D line on a specified Y plane with given start and end points.
 *
 * This function adds a line defined by its start and end XZ coordinates
 * at a specific Y height to the batch of lines to be rendered. The line
 * is stored along with its thickness for later rendering in the RenderAll() function.
 *
 * @param v2StartPoint 2D vector representing the start point (X,Z) of the line.
 * @param v2EndPoint   2D vector representing the end point (X,Z) of the line.
 * @param fY          Y coordinate (height) where the line will be rendered.
 * @param fThickness  Thickness of the line (default is 1.0f).
 */
void CDebugRenderer::RenderLine2D(const Vector2D& v2StartPoint, const Vector2D& v2EndPoint, GLfloat fY)
{
	RenderLine3D(v2StartPoint.x, fY, v2StartPoint.y, v2EndPoint.x, fY, v2EndPoint.y);
}

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
void CDebugRenderer::RenderLine3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez)
{
	// 2. Capture the starting index for this line
	// If the vertex buffer has 10 vertices, the next two will be at index 10 and 11.
	GLuint startIndex = static_cast<GLuint>(m_vWireframeVertices.size());

	// Add start vertex (Color choice matters now)
	m_vWireframeVertices.push_back({ Vector3D(sx, sy, sz), m_v4DiffuseColor });

	// Add end vertex
	m_vWireframeVertices.push_back({ Vector3D(ex, ey, ez), m_v4DiffuseColor });

	// 4. Add Indices
	// Every line needs two indices pointing to the two vertices above
	m_vWireframeIndices.push_back(startIndex);
	m_vWireframeIndices.push_back(startIndex + 1);
}

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
void CDebugRenderer::RenderLine3D(const Vector3D& v3StartPoint, const Vector3D& v3EndPoint)
{
	RenderLine3D(v3StartPoint.x, v3StartPoint.y, v3StartPoint.z, v3EndPoint.x, v3EndPoint.y, v3EndPoint.z);
}

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
void CDebugRenderer::RenderCircle2D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep, bool bHorizonal)
{
	GLfloat fTheta = 0.0f;
	GLfloat fDelta = 2.0f * (static_cast<GLfloat>(M_PI)) / static_cast<GLfloat>(iStep);
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
	std::vector<Vector3D> v3Points;
	v3Points.reserve(iStep);

	for (GLint i = 0; i <= iStep; ++i)
	{
		x = fX + fRadius * cosf(fTheta);
		y = fY;
		z = fZ + fRadius * sinf(fTheta);
		if (bHorizonal)
		{
			v3Points.push_back(Vector3D(x, y, z));
		}
		else
		{
			v3Points.push_back(Vector3D(x, z, y));
		}
		fTheta += fDelta;
	}

	for (GLint count = 0; count < iStep - 1; count++)
	{
		RenderLine3D(v3Points[count].x, v3Points[count].y, v3Points[count].z, v3Points[count + 1].x, v3Points[count + 1].y, v3Points[count + 1].z);
	}
	RenderLine3D(v3Points[iStep - 1].x, v3Points[iStep - 1].y, v3Points[iStep - 1].z, v3Points[0].x, v3Points[0].y, v3Points[0].z);
}

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
void CDebugRenderer::RenderCircle2D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep, bool bHorizonal)
{
	RenderCircle2D(v3Position.x, v3Position.y, v3Position.z, fRadius, iStep, bHorizonal);
}

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
void CDebugRenderer::RenderCircle3D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep)
{
	GLfloat fTheta = 0.0f;
	GLfloat fDelta = 2.0f * (static_cast<GLfloat>(M_PI)) / static_cast<GLfloat>(iStep);
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
	std::vector<Vector3D> v3Points;
	v3Points.reserve(iStep);

	// Get the billboard matrix from the current camera
	Matrix4 billBoardMat = CCameraManager::Instance().GetCurrentCamera()->GetBillBoardMatrix();

	// Generate the circle points
	for (GLint count = 0; count <= iStep; ++count)
	{
		SVector3Df point(fRadius * std::cosf(fTheta), fRadius * std::sinf(fTheta), 0.0f);
		v3Points.push_back(SVector3Df(billBoardMat * SVector4Df(point, 1.0f))); // Transform to camera-aligned space
		fTheta += fDelta;
	}

	// Render the circle using connected line segments
	for (GLint count = 0; count < iStep - 1; count++)
	{
		RenderLine3D(fX + v3Points[count].x, fY + v3Points[count].y, fZ + v3Points[count].z,
					fX + v3Points[count + 1].x, fY + v3Points[count + 1].y, fZ + v3Points[count + 1].z);
	}

	// Connect the last point to the first to close the circle
	RenderLine3D(fX + v3Points[iStep - 1].x, fY + v3Points[iStep - 1].y, fZ + v3Points[iStep - 1].z,
				fX + v3Points[0].x, fY + v3Points[0].y, fZ + v3Points[0].z);
}


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
void CDebugRenderer::RenderCircle3D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep)
{
	RenderCircle3D(v3Position.x, v3Position.y, v3Position.z, fRadius, iStep);
}

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
void CDebugRenderer::RenderSolidCircle2D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep)
{
	// --- Setup for Solid Geometry ---
	std::vector<SLinesVertex>& targetVertices = m_vSolidVertices;
	std::vector<GLuint>& targetIndices = m_vSolidIndices;
	const GLuint offset = (GLuint)targetVertices.size();

	// Constants
	const GLfloat fDelta = 2.0f * (static_cast<GLfloat>(M_PI)) / static_cast<GLfloat>(iStep);

	// --- 1. Add Center Vertex (Index: offset + 0) ---
	SVector3Df center(fX, fY, fZ);
	// Note: We use the existing fThickness parameter for consistency, though it's optional for solids
	targetVertices.push_back({ center, m_v4DiffuseColor });

	// --- 2. Generate and Add Edge Vertices ---
	for (GLint count = 0; count <= iStep; ++count)
	{
		GLfloat fTheta = fDelta * (GLfloat)count;

		// Position in the XY plane, relative to the center
		// Note: No billboard transform, circle is fixed in World Space.
		GLfloat x_local = fRadius * std::cosf(fTheta);
		GLfloat y_local = fRadius * std::sinf(fTheta);

		// Final position (relative to the center arguments fX, fY, fZ)
		SVector3Df edgePos(fX + x_local, fY + y_local, fZ); // Z is fixed to fZ

		// Add edge vertex. Index starts at offset + 1
		targetVertices.push_back({ edgePos, m_v4DiffuseColor });
	}

	// --- 3. Generate Indices (Triangle Fan Logic using GL_TRIANGLES) ---
	// Total vertices added: iStep + 2 (Center + iStep + 1 for wrap-around)
	// We generate iStep triangles.

	for (GLint i = 0; i < iStep; ++i)
	{
		// Triangle is formed by: Center (offset + 0), Current Edge (offset + i + 1), Next Edge (offset + i + 2)

		targetIndices.push_back(offset + 0);         // Center
		targetIndices.push_back(offset + i + 1);     // Current edge vertex
		targetIndices.push_back(offset + i + 2);     // Next edge vertex 
		// Note: The last iteration uses the (iStep + 1)th vertex, which is a copy of the 1st
		// edge vertex (angle 2*PI) and closes the loop.
	}
}

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
void CDebugRenderer::RenderSolidCircle2D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep)
{
	RenderSolidCircle2D(v3Position.x, v3Position.y, v3Position.z, fRadius, iStep);
}

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
void CDebugRenderer::RenderSolidCircle3D(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius, GLint iStep)
{
	// --- Setup for Solid Geometry ---
	std::vector<SLinesVertex>& targetVertices = m_vSolidVertices;
	std::vector<GLuint>& targetIndices = m_vSolidIndices;
	const GLuint offset = (GLuint)targetVertices.size();

	// Constants
	const GLfloat fDelta = 2.0f * (static_cast<GLfloat>(M_PI)) / static_cast<GLfloat>(iStep);

	// Get the billboard matrix from the current camera
	Matrix4 billBoardMat = CCameraManager::Instance().GetCurrentCamera()->GetBillBoardMatrix();

	// --- 1. Add Center Vertex (Index: offset + 0) ---
	SVector3Df center(fX, fY, fZ);
	// Note: We use the existing fThickness parameter for consistency, though it's optional for solids
	targetVertices.push_back({ center, m_v4DiffuseColor });

	// --- 2. Generate and Add Edge Vertices ---
	for (GLint count = 0; count <= iStep; ++count)
	{
		GLfloat fTheta = fDelta * (GLfloat)count;

		// Position in the XY plane, relative to the center
		// Note: No billboard transform, circle is fixed in World Space.
		SVector3Df point_local(fRadius * std::cosf(fTheta), fRadius * std::sinf(fTheta), 0.0f);

		// Transform the local point using the billboard matrix
		SVector3Df point_world = SVector3Df(billBoardMat * SVector4Df(point_local, 1.0f));

		// Final position = Circle Center + Transformed Local Point
		SVector3Df edgePos(center.x + point_world.x, center.y + point_world.y, center.z + point_world.z);

		// Add edge vertex. Index starts at offset + 1
		targetVertices.push_back({ edgePos, m_v4DiffuseColor });
	}

	// --- 3. Generate Indices (Triangle Fan Logic using GL_TRIANGLES) ---
	// Total vertices added: iStep + 2 (Center + iStep + 1 for wrap-around)
	// We generate iStep triangles.

	for (GLint i = 0; i < iStep; ++i)
	{
		// Triangle is formed by: Center (offset + 0), Current Edge (offset + i + 1), Next Edge (offset + i + 2)

		targetIndices.push_back(offset + 0);         // Center
		targetIndices.push_back(offset + i + 1);     // Current edge vertex
		targetIndices.push_back(offset + i + 2);     // Next edge vertex 
		// Note: The last iteration uses the (iStep + 1)th vertex, which is a copy of the 1st
		// edge vertex (angle 2*PI) and closes the loop.
	}
}

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
void CDebugRenderer::RenderSolidCircle3D(const Vector3D& v3Position, GLfloat fRadius, GLint iStep)
{
	RenderSolidCircle3D(v3Position.x, v3Position.y, v3Position.z, fRadius, iStep);
}

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
void CDebugRenderer::RenderSquare3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez, const BoxRenderMode& renderMode)
{
	// Determine the target buffers based on the renderMode
	std::vector<SLinesVertex>& targetVertices = (renderMode == WIREFRAME) ? m_vWireframeVertices : m_vSolidVertices;
	std::vector<GLuint>& targetIndices = (renderMode == WIREFRAME) ? m_vWireframeIndices : m_vSolidIndices;

	// Define 4 corners of the square plane.
	// Here, we assume the plane is axis-aligned in XY and uses the z value of the first corner (sz).
	SVector3Df v0(sx, sy, sz); // bottom-left
	SVector3Df v1(ex, sy, sz); // bottom-right
	SVector3Df v2(sx, ey, sz); // top-left
	SVector3Df v3(ex, ey, sz); // top-right

	// Create a vertex array for 4 vertices using a common color (m_v4DiffColor)
	targetVertices.push_back({ v0, m_v4DiffuseColor });
	targetVertices.push_back({ v1, m_v4DiffuseColor });
	targetVertices.push_back({ v2, m_v4DiffuseColor });
	targetVertices.push_back({ v3, m_v4DiffuseColor });

	const GLuint offset = (GLuint)targetVertices.size() - 4; // Use size BEFORE push, or just targetVertices.size() if that's where you start

	// Define indices for the square edges (4 edges, 8 indices total)
	if (renderMode == WIREFRAME)
	{
		// Wireframe mode - define edges
		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 1); // Bottom edge
		targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 3); // Right edge
		targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 2); // Top edge
		targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 0); // Left edge
	}
	else
	{
		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 3);	// Front face (v0,v1,v3,v2)
		targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 0);	// Front face (v0,v1,v3,v2)
	}
}

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
void CDebugRenderer::RenderSquare3D(const Vector3D& v3Start, const Vector3D& v3End, const BoxRenderMode& renderMode)
{
	RenderSquare3D(v3Start.x, v3Start.y, v3Start.z, v3End.x, v3End.y, v3End.z, renderMode);
}

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
void CDebugRenderer::RenderBox3D(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez, const BoxRenderMode& renderMode)
{
	// Determine the target buffers based on the renderMode
	std::vector<SLinesVertex>& targetVertices = (renderMode == WIREFRAME) ? m_vWireframeVertices : m_vSolidVertices;
	std::vector<GLuint>& targetIndices = (renderMode == WIREFRAME) ? m_vWireframeIndices : m_vSolidIndices;

	// Define the 8 corners of the cube
	SVector3Df v0(sx, sy, sz); // 0
	SVector3Df v1(ex, sy, sz); // 1
	SVector3Df v2(sx, ey, sz); // 2
	SVector3Df v3(ex, ey, sz); // 3
	SVector3Df v4(sx, sy, ez); // 4
	SVector3Df v5(ex, sy, ez); // 5
	SVector3Df v6(sx, ey, ez); // 6
	SVector3Df v7(ex, ey, ez); // 7

	// Calculate the base offset for indices
	const GLuint offset = (GLuint)targetVertices.size();

	// Create an array of vertices using a common color (m_v4DiffColor)
	targetVertices.push_back({ v0, m_v4DiffuseColor });
	targetVertices.push_back({ v1, m_v4DiffuseColor });
	targetVertices.push_back({ v2, m_v4DiffuseColor });
	targetVertices.push_back({ v3, m_v4DiffuseColor });
	targetVertices.push_back({ v4, m_v4DiffuseColor });
	targetVertices.push_back({ v5, m_v4DiffuseColor });
	targetVertices.push_back({ v6, m_v4DiffuseColor });
	targetVertices.push_back({ v7, m_v4DiffuseColor });

	if (renderMode == WIREFRAME)
	{
		// Wireframe mode - define edges
		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 1); // Bottom face
		targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 3);
		targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 2);
		targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 0);
		targetIndices.push_back(offset + 4); targetIndices.push_back(offset + 5); // Top face
		targetIndices.push_back(offset + 5); targetIndices.push_back(offset + 7);
		targetIndices.push_back(offset + 7); targetIndices.push_back(offset + 6);
		targetIndices.push_back(offset + 6); targetIndices.push_back(offset + 4);
		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 4); // Side edges
		targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 5);
		targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 6);
		targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 7);
	}
	else
	{
		// Define indices for 12 triangles (36 indices total)
		// Each face of the cube is made up of two triangles.
		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 3);	// Front face (v0,v1,v3,v2)
		targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 0);	// Front face (v0,v1,v3,v2)

		targetIndices.push_back(offset + 4); targetIndices.push_back(offset + 6); targetIndices.push_back(offset + 7);	// Back face (v4,v6,v7,v5)
		targetIndices.push_back(offset + 7); targetIndices.push_back(offset + 5); targetIndices.push_back(offset + 4);	// Back face (v4,v6,v7,v5)

		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 6);	// Left face (v0,v2,v6,v4)
		targetIndices.push_back(offset + 6); targetIndices.push_back(offset + 4); targetIndices.push_back(offset + 0);	// Left face (v0,v2,v6,v4)

		targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 5); targetIndices.push_back(offset + 7);	// Right face (v1,v5,v7,v3)
		targetIndices.push_back(offset + 7); targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 1);	// Right face (v1,v5,v7,v3)

		targetIndices.push_back(offset + 2); targetIndices.push_back(offset + 3); targetIndices.push_back(offset + 7);	// Top face (v2,v3,v7,v6)
		targetIndices.push_back(offset + 7); targetIndices.push_back(offset + 6); targetIndices.push_back(offset + 2);	// Top face (v2,v3,v7,v6)

		targetIndices.push_back(offset + 0); targetIndices.push_back(offset + 4); targetIndices.push_back(offset + 5);	// Bottom face (v0,v4,v5,v1)
		targetIndices.push_back(offset + 5); targetIndices.push_back(offset + 1); targetIndices.push_back(offset + 0);	// Bottom face (v0,v4,v5,v1)
	}
}

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
void CDebugRenderer::RenderBox3D(const Vector3D& v3Start, const Vector3D& v3End, const BoxRenderMode& renderMode)
{
	RenderBox3D(v3Start.x, v3Start.y, v3Start.z, v3End.x, v3End.y, v3End.z, renderMode);
}

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
void CDebugRenderer::RenderSphere3D(GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat fRadius, GLint iSegments, GLint iSlices)
{
	// Target buffers for solid geometry
	std::vector<SLinesVertex>& targetVertices = m_vSolidVertices;
	std::vector<GLuint>& targetIndices = m_vSolidIndices;

	const GLuint baseOffset = (GLuint)targetVertices.size();

	// 1. Generate Vertices (Poles and Rings)
	for (int i = 0; i <= iSegments; ++i) // Iterate through stacks (latitude) - iSegments
	{
		GLfloat phi = static_cast<GLfloat>(i) / static_cast<GLfloat>(iSegments) * static_cast<GLfloat>(M_PI); // angle phi from 0 to PI
		GLfloat sinPhi = sin(phi);
		GLfloat cosPhi = cos(phi);

		for (GLint j = 0; j <= iSlices; ++j) // Iterate through slices (longitude)
		{
			GLfloat theta = static_cast<GLfloat>(j) / static_cast<GLfloat>(iSlices) * (2.0f * static_cast<GLfloat>(M_PI)); // angle theta from 0 to 2*PI
			GLfloat sinTheta = sin(theta);
			GLfloat cosTheta = cos(theta);

			// Calculate 3D coordinates
			GLfloat x = fRadius * cosTheta * sinPhi;
			GLfloat y = fRadius * sinTheta * sinPhi;
			GLfloat z = fRadius * cosPhi;

			SVector3Df position(fCenterX + x, fCenterY + y, fCenterZ + z);

			// Add the vertex to the buffer
			// Assuming default color and thickness (0.0f) for solid
			targetVertices.push_back({ position, m_v4DiffuseColor });
		}
	}

	// 2. Generate Indices (Quads formed by 4 vertices, then split into 2 triangles)
	for (GLint i = 0; i < iSegments; ++i) // Row 'i'
	{
		for (GLint j = 0; j < iSlices; ++j) // Column 'j'
		{
			// Indices of the four corners of a quad:
			// (i, j) ---- (i, j+1)
			//   |            |
			// (i+1, j) -- (i+1, j+1)

			// Calculate indices in the flat array (where slices+1 is the row width)
			GLuint i1 = baseOffset + (i * (iSlices + 1)) + j;
			GLuint i2 = baseOffset + (i * (iSlices + 1)) + j + 1;
			GLuint i3 = baseOffset + ((i + 1) * (iSlices + 1)) + j;
			GLuint i4 = baseOffset + ((i + 1) * (iSlices + 1)) + j + 1;

			// First triangle (i1, i3, i4)
			targetIndices.push_back(i1);
			targetIndices.push_back(i3);
			targetIndices.push_back(i4);

			// Second triangle (i1, i4, i2)
			targetIndices.push_back(i1);
			targetIndices.push_back(i4);
			targetIndices.push_back(i2);
		}
	}
}

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
void CDebugRenderer::RenderSphere3D(const Vector3D& v3Center, GLfloat fRadius, GLint iSegments, GLint iSlices)
{
	RenderSphere3D(v3Center.x, v3Center.y, v3Center.z, fRadius, iSegments, iSlices);
}

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
void CDebugRenderer::RenderWireSphere3D(GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat fRadius, GLint iSegments, GLint iSlices)
{
	// Target buffers for solid geometry
	std::vector<SLinesVertex>& targetVertices = m_vSolidVertices;
	std::vector<GLuint>& targetIndices = m_vSolidIndices;

	const GLuint baseOffset = (GLuint)targetVertices.size();

	// 1. Generate Vertices (Poles and Rings)
	for (int i = 0; i <= iSegments; ++i) // Iterate through stacks (latitude) - iSegments
	{
		GLfloat phi = static_cast<GLfloat>(i) / static_cast<GLfloat>(iSegments) * static_cast<GLfloat>(M_PI); // angle phi from 0 to PI
		GLfloat sinPhi = std::sin(phi);
		GLfloat cosPhi = std::cos(phi);

		SVector3Df prevPos{}; // Store the previous position to draw lines

		for (GLint j = 0; j <= iSlices; ++j) // Iterate through slices (longitude)
		{
			GLfloat theta = static_cast<GLfloat>(j) / static_cast<GLfloat>(iSlices) * (2.0f * static_cast<GLfloat>(M_PI)); // angle theta from 0 to 2*PI
			GLfloat sinTheta = std::sin(theta);
			GLfloat cosTheta = std::cos(theta);

			// Calculate 3D coordinates
			GLfloat x = fRadius * cosTheta * sinPhi;
			GLfloat y = fRadius * sinTheta * sinPhi;
			GLfloat z = fRadius * cosPhi;

			SVector3Df currentPos(fCenterX + x, fCenterY + y, fCenterZ + z);

			if (j > 0)
			{
				// Draw longitude line segments (around the stack/ring)
				RenderLine3D(prevPos.x, prevPos.y, prevPos.z, currentPos.x, currentPos.y, currentPos.z);
			}
			prevPos = currentPos;
		}
	}
}

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
void CDebugRenderer::RenderWireSphere3D(const Vector3D& v3Center, GLfloat fRadius, GLint iSegments, GLint iSlices)
{
	RenderWireSphere3D(v3Center.x, v3Center.y, v3Center.z, fRadius, iSegments, iSlices);
}

/**
 * @brief Draws a crosshair at the center of the screen.
 *
 * This function calculates the position in front of the camera and
 * draws a simple crosshair using two intersecting lines.
 */
void CDebugRenderer::DrawCrosshair()
{
	auto pCamera = CCameraManager::Instance().GetCameraByID(1);
	SVector3Df centerPoint = pCamera->GetPosition() + (pCamera->GetFront() * 2.0f);

	// Get your existing Billboard Matrix
	Matrix4 billMat = pCamera->GetBillBoardMatrix();

	// Size of the crosshair lines
	GLfloat size = 0.05f;

	// Define the 4 points of the cross in LOCAL space (flat on screen)
	SVector3Df left = SVector3Df(-size, 0, 0);
	SVector3Df right = SVector3Df(size, 0, 0);
	SVector3Df top = SVector3Df(0, size, 0);
	SVector3Df bottom = SVector3Df(0, -size, 0);

	// Transform these points by the billboard matrix to face the camera
	// Note: We don't want the translation from the matrix, just rotation
	SVector3Df vL = EngineMath::TransformDirection(billMat, left);
	SVector3Df vR = EngineMath::TransformDirection(billMat, right);
	SVector3Df vT = EngineMath::TransformDirection(billMat, top);
	SVector3Df vB = EngineMath::TransformDirection(billMat, bottom);

	// Render the lines in World Space, but now they are oriented to the lens
	RenderLine3D(centerPoint + vL, centerPoint + vR);
	RenderLine3D(centerPoint + vT, centerPoint + vB);
}
