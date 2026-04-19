#include "InputManager.h"
#include <GLFW/glfw3.h>
#include "../../MapLayer/include/MapManager.h"

/**
 * @brief Initializes the input manager.
 *
 * Sets up necessary data structures and states for input handling.
 */
void CInputManager::Initialize()
{
	m_bKeyBools.fill(EKeyState::KEY_UP);
	m_bMouseKeys.fill(EKeyState::KEY_UP);

	m_v2MousePos = Vector2D(0.0f, 0.0f);

	m_fMouseScrollVal = 0.0f;;
}

/**
 * @brief Destroys the input manager.
 *
 * Cleans up resources and resets states used by the input manager.
 */
void CInputManager::Destroy()
{
	m_bKeyBools.fill(EKeyState::KEY_UP);
	m_bMouseKeys.fill(EKeyState::KEY_UP);
}

/**
 * @brief Updates the input manager state.
 *
 * This function should be called once per frame to update the state
 * of keys and mouse buttons.
 */
void CInputManager::Update(float deltaTime)
{
	// Update key states (Pressed->Down, Released->Up)
	for (auto& key : m_bKeyBools)
	{
		if (key == EKeyState::KEY_PRESSED)
		{
			key = EKeyState::KEY_DOWN;
		}
		else if (key == EKeyState::KEY_RELEASED)
		{
			key = EKeyState::KEY_UP;
		}
	}

	// Update mouse button states similarly
	for (auto& key : m_bMouseKeys)
	{
		if (key == EKeyState::KEY_PRESSED)
		{
			key = EKeyState::KEY_DOWN;
		}
		else if (key == EKeyState::KEY_RELEASED)
		{
			key = EKeyState::KEY_UP;
		}
	}
}

/**
 * @brief Handles key input events.
 *
 * @param key The key code of the key event.
 * @param pressed True if the key is pressed, false if released.
 */
void CInputManager::OnKey(int32_t key, bool pressed)
{
	if (pressed)
	{
		// m_bKeyBools[key] = (m_bKeyBools[key] == EKeyState::KEY_DOWN) ? EKeyState::KEY_DOWN : EKeyState::KEY_PRESSED;

		// new press vs already down
		m_bKeyBools[key] = (m_bKeyBools[key] == EKeyState::KEY_DOWN || m_bKeyBools[key] == EKeyState::KEY_PRESSED)
			? EKeyState::KEY_DOWN
			: EKeyState::KEY_PRESSED;

	}
	else
	{
		m_bKeyBools[key] = EKeyState::KEY_RELEASED;
	}
}

/**
 * @brief Handles mouse button input events.
 *
 * @param button The mouse button code of the event.
 * @param pressed True if the button is pressed, false if released.
 */
void CInputManager::OnMouseButton(int32_t button, bool pressed)
{
	if (pressed)
	{
		m_bMouseKeys[button] = (m_bMouseKeys[button] == EKeyState::KEY_DOWN) ? EKeyState::KEY_DOWN : EKeyState::KEY_PRESSED;

		// --- ADDED LOGIC ---
		// Only trigger UI click on the initial press (not while holding)
		if (button == GLFW_MOUSE_BUTTON_LEFT && m_bMouseKeys[button] == EKeyState::KEY_PRESSED)
		{

		}
	}
	else
	{
		m_bMouseKeys[button] = EKeyState::KEY_RELEASED;
	}
}

/**
 * @brief Handles mouse movement events.
 *
 * @param v2MousePos The new mouse position.
 */
void CInputManager::OnMouseMove(Vector2D v2MousePos)
{
	Vector2D v2Delta = v2MousePos - m_v2LastMousePos;
	m_v2LastMousePos = m_v2MousePos;
	m_v2MousePos = v2MousePos;

	// Only drag if left mouse is held
	if (m_bMouseKeys[0] == EKeyState::KEY_DOWN || m_bMouseKeys[0] == EKeyState::KEY_PRESSED)
	{
		CMapManager::Instance().OnMouseDrag(v2Delta.x, v2Delta.y);
	}
}

/**
 * @brief Handles mouse scroll events.
 *
 * @param fScrollOffset The scroll offset value.
 */
void CInputManager::OnMouseScroll(float fScrollOffset)
{
	m_fMouseScrollVal = fScrollOffset;
	CMapManager::Instance().OnMouseScroll(m_fMouseScrollVal, m_v2MousePos.x, m_v2MousePos.y);
}

/**
 * brief Checks if a key is currently down.
 *
 * @param iKey The key code to check.
 * @return True if the key is down or was just pressed, false otherwise.
 */
bool CInputManager::IsKeyDown(int32_t iKey) const
{
	return (m_bKeyBools[iKey] == EKeyState::KEY_DOWN) || (m_bKeyBools[iKey] == EKeyState::KEY_PRESSED);
}

/**
 * brief Checks if a key was just pressed this frame.
 *
 * @param iKey The key code to check.
 * @return True if the key was just pressed, false otherwise.
 */
bool CInputManager::IsKeyPressed(int32_t iKey) const
{
	return (m_bKeyBools[iKey] == EKeyState::KEY_PRESSED);
}

/**
 * brief Checks if a key was just released this frame.
 *
 * @param iKey The key code to check.
 * @return True if the key was just released, false otherwise.
 */
bool CInputManager::IsKeyReleased(int32_t iKey) const
{
	return (m_bKeyBools[iKey] == EKeyState::KEY_RELEASED);
}

/**
 * brief Checks if a mouse button is currently down.
 *
 * @param iKey The mouse button code to check.
 * @return True if the mouse button is down or was just pressed, false otherwise.
 */
bool CInputManager::IsMouseDown(int32_t iKey) const
{
	return (m_bMouseKeys[iKey] == EKeyState::KEY_DOWN) || (m_bMouseKeys[iKey] == EKeyState::KEY_PRESSED);
}

/**
 * brief Checks if a mouse button was just pressed this frame.
 *
 * @param iKey The mouse button code to check.
 * @return True if the mouse button was just pressed, false otherwise.
 */
bool CInputManager::IsMousePressed(int32_t iKey) const
{
	return (m_bMouseKeys[iKey] == EKeyState::KEY_PRESSED);
}

/**
 * brief Checks if a mouse button was just released this frame.
 *
 * @param iKey The mouse button code to check.
 * @return True if the mouse button was just released, false otherwise.
 */
bool CInputManager::IsMouseReleased(int32_t iKey) const
{
	return (m_bMouseKeys[iKey] == EKeyState::KEY_RELEASED);
}

/**
 * @brief Retrieves the current mouse position.
 *
 * @return The current mouse position as a Vector2D.
 */
Vector2D CInputManager::GetMousePosition() const
{
	return (m_v2MousePos);
}

/**
 * @brief Retrieves the current mouse scroll value.
 *
 * @return The current mouse scroll value as a GLfloat.
 */
float CInputManager::GetMouseScroll() const
{
	return (m_fMouseScrollVal);
}
