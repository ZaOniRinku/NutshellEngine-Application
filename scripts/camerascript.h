#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/module_interfaces/ntshengn_window_module_interface.h"
#include "../Core/src/frame_limiter/ntshengn_frame_limiter.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>

struct CameraScript : public NtshEngn::Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);

			m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
			m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
			windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);

			const NtshEngn::Transform transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			NtshEngn::Math::vec3 cameraRotation = NtshEngn::Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

			m_yaw = std::atan2(cameraRotation.z, cameraRotation.x) * toDeg;
			m_pitch = -std::asin(cameraRotation.y) * toDeg;
		}
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::R) == NtshEngn::InputState::Pressed) {
				m_mouseMiddleMode = !m_mouseMiddleMode;
				windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);
				if (m_mouseMiddleMode) {
					m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
					m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
					windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);
				}
			}

			NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			NtshEngn::Math::vec3 cameraRotation = NtshEngn::Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

			if (m_mouseMiddleMode) {
				const int mouseX = windowModule->getCursorPositionX(windowModule->getMainWindowID());
				const int mouseY = windowModule->getCursorPositionY(windowModule->getMainWindowID());

				m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
				m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
				windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);

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
				cameraRotation = NtshEngn::Math::normalize(cameraRotation);
			}

			const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);

			NtshEngn::Math::vec3 cameraPosition = NtshEngn::Math::vec3(transform.position[0], transform.position[1], transform.position[2]);
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				cameraPosition += (cameraRotation * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				cameraPosition -= (cameraRotation * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x -= (t.x * cameraSpeed);
				cameraPosition.z -= (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x += (t.x * cameraSpeed);
				cameraPosition.z += (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Space) == NtshEngn::InputState::Held) {
				cameraPosition.y += cameraSpeed;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Shift) == NtshEngn::InputState::Held) {
				cameraPosition.y -= cameraSpeed;
			}

			transform.position = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
			transform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
		}
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