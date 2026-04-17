#include "CameraManager.h"
#include "Camera.h"
#include "LogManager.h"

/**
 * @brief constructor for the camera manager.
 *
 * Initializes the current and previous camera pointers to a safe state and
 * creates and activates the default perspective camera.
 */
CCameraManager::CCameraManager()
{
	// Start IDs at 0, or some appropriate value.
	m_uiNextCameraID = 1;

	/* Initialize camera pointers to a known null state. */
	m_pCurrentCamera = nullptr;
	m_pPreviousCamera = nullptr;
}

void CCameraManager::Destroy()
{
	/* Clear the camera map, destroying all unique_ptr-managed cameras. */
	m_mCameraMap.clear();

	/* Reset camera pointers to null for safety. */
	m_pCurrentCamera = nullptr;
	m_pPreviousCamera = nullptr;
}

/**
 * @brief Creates a new CCamera instance and adds it to the map with a unique ID.
 * * This allows multiple cameras of the same functional type (e.g., multiple perspective views).
 * * @param camType The functional type (e.g., PERSPECTIVE, ORTHO). This is now stored in the CCamera object.
 * * @return uint32_t The unique ID assigned to the newly created camera instance.
 */
uint32_t CCameraManager::AddCamera(uint8_t camType)
{
	// Get the unique ID for this new camera instance *before* incrementing the tracker.
	uint32_t newCamID = m_uiNextCameraID;

	// Increment the tracker for the next call.
	m_uiNextCameraID++;

	/* Do not create a camera if this ID already exists in the map. */
	if (m_mCameraMap.count(newCamID))
	{
		syslog("Camera type {} already exists. Skipping addition.", newCamID);
		return (newCamID);
	}

	/* Insert a new camera owned by a unique_ptr into the map. */
	auto [it, inserted] = m_mCameraMap.emplace(newCamID, std::make_unique<CCamera>());

	if (!inserted)
	{
		syslog("Failed to insert camera ID {}.", newCamID);
	}

	/* Initialize the camera with the specified type. */
	it->second->Initialize(camType);

	return (newCamID);
}

/**
 * @brief Remove and destroy the camera instance of a given ID.
 * * Handles the case where the camera being removed is the currently active one.
 * * @param camID The unique uint32_t identifier of the camera instance to remove.
 */
void CCameraManager::RemoveCamera(uint32_t camID)
{
	/* Look up the camera in the map. */
	std::unordered_map<uint32_t, std::unique_ptr<CCamera>>::iterator it = m_mCameraMap.find(camID);

	/* If no camera is found for this type, there is nothing to remove. */
	if (it == m_mCameraMap.end())
		return;

	/* Get a raw pointer to the camera for pointer comparison logic. */
	CCamera* cam = it->second.get();

	/* If the removed camera is currently active, update the active camera. */
	if (m_pCurrentCamera == cam)
	{
		m_pCurrentCamera = nullptr;

		/* Switch to the previous camera if it is valid and different. */
		if (m_pPreviousCamera && m_pPreviousCamera != cam)
		{
			m_pCurrentCamera = m_pPreviousCamera;
			syslog("Removed camera type {} was active. Switched to previous camera.", camID);
		}
		else
		{
			syslog("Removed camera type {} was active. No previous camera to switch to.", camID);
		}

		/* Clear the previous camera pointer after the switch decision. */
		m_pPreviousCamera = nullptr;
	}
	else if (m_pPreviousCamera == cam)
	{
		/* If the removed camera was only the previous camera, clear that pointer. */
		m_pPreviousCamera = nullptr;
	}

	/* Finally erase the camera from the map, destroying the unique_ptr. */
	m_mCameraMap.erase(it);
}

/**
 * @brief Sets the currently active camera based on the provided unique ID.
 * * Safely updates m_pCurrentCamera and preserves the previously active camera in m_pPreviousCamera.
 * * @param camID The unique uint32_t identifier of the camera instance to activate.
 */
void CCameraManager::SetCurrentCamera(uint32_t camID)
{
	/* Find the requested camera in the map without creating a new entry. */
	std::unordered_map<uint32_t, std::unique_ptr<CCamera>>::iterator it = m_mCameraMap.find(camID);

	/* If the camera type is not present, log an error and abort the change. */
	if (it == m_mCameraMap.end())
	{
		syslog("ERROR: Attempted to set non-existent camera type {} as current.", camID);
		return;
	}

	/* Get a non-owning pointer to the new current camera. */
	CCamera* pNewCamera = it->second.get();

	/* Only update the previous camera if we are actually switching cameras. */
	if (m_pCurrentCamera != pNewCamera)
	{
		/* Preserve the currently active camera as the previous one for quick switching/fallback */
		m_pPreviousCamera = m_pCurrentCamera;
	}

	/* Set the new current camera and log the change. */
	m_pCurrentCamera = pNewCamera;

#if defined(ENABLE_DEBUG_LOGS)
	syslog("Current camera set to ID {}.", camID);
#endif
}

/**
 * @brief Retrieves the raw pointer to the currently active CCamera object.
 * * @return CCamera* A non-owning raw pointer to the active camera.
 */
CCamera* CCameraManager::GetCurrentCamera() const
{
	// Assert ensures we never try to return a nullptr (i.e., that a camera is always active).
	assert(m_pCurrentCamera);
	return (m_pCurrentCamera);
}

/**
 * @brief Bootstraps the camera system by creating default viewports.
 *
 * This function handles the initial setup of the camera manager by adding
 * both a perspective and an orthographic camera to the internal registry.
 * It ensures the application starts with a valid perspective view active.
 */
void CCameraManager::Initialize()
{
	/* Add the mandatory initial camera (Perspective View) and get its unique ID. */
	uint32_t initialCameraID = AddCamera(CAMERA_PERSPECTIVE);

	/* Set the initial camera as the active camera for the application. */
	SetCurrentCamera(initialCameraID);

	AddCamera(CAMERA_ORTHOGRAPHIC);
	AddCamera(CAMERA_PERSPECTIVE);
}

/**
 * @brief Updates all managed cameras by calling their Update method.
 */
void CCameraManager::UpdateCameras()
{
	for (auto& [id, cameraPtr] : m_mCameraMap)
	{
		cameraPtr->Update();
	}
}

CCamera* CCameraManager::GetCameraByID(uint32_t camID) const
{
	/* Look up the camera in the map. */
	auto it = m_mCameraMap.find(camID);
	/* If no camera is found for this type, return nullptr. */
	if (it == m_mCameraMap.end())
	{
		return nullptr;
	}
	/* Return a non-owning raw pointer to the requested camera. */
	return it->second.get();
}
