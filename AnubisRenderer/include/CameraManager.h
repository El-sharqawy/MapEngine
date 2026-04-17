#pragma once

#include <Singleton.h>
#include <unordered_map>
#include <memory>

class CCamera;

class CCameraManager : public CSingleton<CCameraManager>
{
public:
	/**
	 * @brief constructor for the camera manager.
	 */
	CCameraManager();

	/**
	 * @brief destructor for the camera management system.
	 *
	 * Automatically destroys all cameras managed by this instance through the
	 * unique_ptr container. No manual cleanup required due to RAII.
	 */
	~CCameraManager() = default;

	/**
	 * @brief Bootstraps the camera system by creating default viewports.
	 *
	 * This function handles the initial setup of the camera manager by adding
	 * both a perspective and an orthographic camera to the internal registry.
	 * It ensures the application starts with a valid perspective view active.
	 */
	void Initialize();

	/**
	 * @brief Clears all managed cameras and resets internal state.
	 *
	 * This function removes all camera instances from the manager,
	 * releasing their resources and resetting pointers to the current
	 * and previous cameras. It prepares the manager for a fresh state.
	 */
	void Destroy();

	/**
	 * @brief Creates a new CCamera instance and adds it to the map with a unique ID.
	 * * This allows multiple cameras of the same functional type (e.g., multiple perspective views).
	 * * @param camType The functional type (e.g., PERSPECTIVE, ORTHO). This is now stored in the CCamera object.
	 * * @return uint32_t The unique ID assigned to the newly created camera instance.
	 */
	uint32_t AddCamera(uint8_t camType);

	/**
	 * @brief Remove and destroy the camera instance of a given ID.
	 * * Handles the case where the camera being removed is the currently active one.
	 * * @param camID The unique uint32_t identifier of the camera instance to remove.
	 */
	void RemoveCamera(uint32_t camID);

	/**
	 * @brief Sets the currently active camera based on the provided unique ID.
	 * * Safely updates m_pCurrentCamera and preserves the previously active camera in m_pPreviousCamera.
	 * * @param camID The unique uint32_t identifier of the camera instance to activate.
	 */
	void SetCurrentCamera(uint32_t camID);

	/**
	 * @brief Retrieves the raw pointer to the currently active CCamera object.
	 * @return CCamera* A non-owning raw pointer to the active camera.
	 */
	CCamera* GetCurrentCamera() const;

	/**
	 * @brief Updates all managed cameras by calling their Update method.
	 */
	void UpdateCameras();

	CCamera* GetCameraByID(uint32_t camID) const;

private:
	std::unordered_map<uint32_t, std::unique_ptr<CCamera>> m_mCameraMap;

	CCamera* m_pCurrentCamera; // non-owning
	CCamera* m_pPreviousCamera; // non-owning
	uint32_t m_uiNextCameraID;
};