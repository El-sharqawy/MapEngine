#pragma once

#include "TypeVector2.h"
#include <array>
#include <Singleton.h>

enum EKeyState : GLint
{
	KEY_UP,
	KEY_DOWN,
	KEY_PRESSED,    // Down this frame
	KEY_RELEASED    // Up this frame
};

class CInputManager : public CSingleton<CInputManager>
{
public:
	/**
	 * @brief Constructor for the CInputManager class.
	 *
	 * Initializes the input manager with default values.
	 */
	CInputManager() = default;

	/**
	 * @brief Destructor for the CInputManager class.
	 *
	 * Cleans up any resources used by the input manager.
	 */
	~CInputManager() = default;

	/**
	 * @brief Initializes the input manager.
	 *
	 * Sets up necessary data structures and states for input handling.
	 */
	void Initialize();

	/**
	 * @brief Destroys the input manager.
	 *
	 * Cleans up resources and resets states used by the input manager.
	 */
	void Destroy();

	/**
	 * @brief Updates the input manager state.
	 *
	 * This function should be called once per frame to update the state
	 * of keys and mouse buttons.
	 */
	void Update(float deltaTime);

	/**
	 * @brief Handles key input events.
	 *
	 * @param key The key code of the key event.
	 * @param pressed True if the key is pressed, false if released.
	 */
	void OnKey(int32_t key, bool pressed);

	/**
	 * @brief Handles mouse button input events.
	 *
	 * @param button The mouse button code of the event.
	 * @param pressed True if the button is pressed, false if released.
	 */
	void OnMouseButton(int32_t button, bool pressed);

	/**
	 * @brief Handles mouse movement events.
	 *
	 * @param v2MousePos The new mouse position.
	 */
	void OnMouseMove(const Vector2D& v2MousePos);

	/**
	 * @brief Handles mouse scroll events.
	 *
	 * @param fScrollOffset The scroll offset value.
	 */
	void OnMouseScroll(float fScrollOffset);

	/**
	 * brief Checks if a key is currently down.
	 *
	 * @param iKey The key code to check.
	 * @return True if the key is down or was just pressed, false otherwise.
	 */
	bool IsKeyDown(int32_t iKey) const;

	/**
	 * brief Checks if a key was just pressed this frame.
	 *
	 * @param iKey The key code to check.
	 * @return True if the key was just pressed, false otherwise.
	 */
	bool IsKeyPressed(int32_t iKey) const;

	/**
	 * brief Checks if a key was just released this frame.
	 *
	 * @param iKey The key code to check.
	 * @return True if the key was just released, false otherwise.
	 */
	bool IsKeyReleased(int32_t iKey) const;

	/**
	 * brief Checks if a mouse button is currently down.
	 *
	 * @param iKey The mouse button code to check.
	 * @return True if the mouse button is down or was just pressed, false otherwise.
	 */
	bool IsMouseDown(int32_t iKey) const;

	/**
	 * brief Checks if a mouse button was just pressed this frame.
	 *
	 * @param iKey The mouse button code to check.
	 * @return True if the mouse button was just pressed, false otherwise.
	 */
	bool IsMousePressed(int32_t iKey) const;

	/**
	 * brief Checks if a mouse button was just released this frame.
	 *
	 * @param iKey The mouse button code to check.
	 * @return True if the mouse button was just released, false otherwise.
	 */
	bool IsMouseReleased(int32_t iKey) const;

	/**
	 * @brief Retrieves the current mouse position.
	 *
	 * @return The current mouse position as a Vector2D.
	 */
	Vector2D GetMousePosition() const;

	/**
	 * @brief Retrieves the current mouse scroll value.
	 *
	 * @return The current mouse scroll value as a GLfloat.
	 */
	float GetMouseScroll() const;

private:
	// Input
	std::array<EKeyState, 512> m_bKeyBools{}; // Max is 512 keys .. 512 bytes
	std::array<EKeyState, 3> m_bMouseKeys{}; // 0 -> Left, 1 -> Right, 2 -> Scroll

	Vector2D m_v2MousePos{}; // Current mouse position
	float m_fMouseScrollVal = 0.0f; // Current mouse scroll value
};