#include "../Core/scripting/ntshengn_script.h"
#include <cmath>

using namespace NtshEngn;
struct CameraScript : Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		setCursorVisibility(!m_mouseMiddleMode);

		m_prevMouseX = getWindowWidth() / 2;
		m_prevMouseY = getWindowHeight() / 2;
		setCursorPosition(m_prevMouseX, m_prevMouseY);

		const Transform transform = getEntityComponent<Transform>(entityID);
		Math::vec3 cameraRotation = Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

		m_yaw = std::atan2(cameraRotation.z, cameraRotation.x) * toDeg;
		m_pitch = -std::asin(cameraRotation.y) * toDeg;

		setMaxFPS(60);
	}

	void update(double dt) {
		if (getKeyState(InputKeyboardKey::R) == InputState::Pressed) {
			m_mouseMiddleMode = !m_mouseMiddleMode;
			setCursorVisibility(!m_mouseMiddleMode);
			if (m_mouseMiddleMode) {
				m_prevMouseX = getWindowWidth() / 2;
				m_prevMouseY = getWindowHeight() / 2;
				setCursorPosition(m_prevMouseX, m_prevMouseY);
			}
		}

		Transform& transform = getEntityComponent<Transform>(entityID);
		Math::vec3 cameraRotation = Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

		if (m_mouseMiddleMode) {
			const int mouseX = getCursorPositionX();
			const int mouseY = getCursorPositionY();

			m_prevMouseX = getWindowWidth() / 2;
			m_prevMouseY = getWindowHeight() / 2;
			setCursorPosition(m_prevMouseX, m_prevMouseY);

			const float xOffset = (mouseX - m_prevMouseX) * m_mouseSensitivity;
			const float yOffset = (mouseY - m_prevMouseY) * m_mouseSensitivity;

			m_prevMouseX = mouseX;
			m_prevMouseY = mouseY;

			m_yaw = std::fmod(m_yaw + xOffset, 360.0f);
			m_pitch = std::max(-89.0f, std::min(89.0f, m_pitch + yOffset));

			float yawRad = m_yaw * toRad;
			float pitchRad = m_pitch * toRad;

			cameraRotation.x = std::cos(pitchRad) * std::cos(yawRad);
			cameraRotation.y = -std::sin(pitchRad);
			cameraRotation.z = std::cos(pitchRad) * std::sin(yawRad);
			cameraRotation = Math::normalize(cameraRotation);
		}

		const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);

		Math::vec3 cameraPosition = Math::vec3(transform.position[0], transform.position[1], transform.position[2]);
		if (getKeyState(InputKeyboardKey::W) == InputState::Held) {
			cameraPosition += (cameraRotation * cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::S) == InputState::Held) {
			cameraPosition -= (cameraRotation * cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::A) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
			cameraPosition.x -= (t.x * cameraSpeed);
			cameraPosition.z -= (t.z * cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::D) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
			cameraPosition.x += (t.x * cameraSpeed);
			cameraPosition.z += (t.z * cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::Space) == InputState::Held) {
			cameraPosition.y += cameraSpeed;
		}
		if (getKeyState(InputKeyboardKey::Shift) == InputState::Held) {
			cameraPosition.y -= cameraSpeed;
		}

		transform.position = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
		transform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
	}

	void destroy() {
	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;

	bool m_mouseMiddleMode = false;

	const float m_cameraSpeed = 0.0015f;
	const float m_mouseSensitivity = 0.12f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};