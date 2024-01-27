#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include <cmath>

using namespace NtshEngn;
struct CameraScript : public Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		setCursorVisibility(!m_mouseMiddleMode);

		m_prevMouseX = getWindowWidth() / 2;
		m_prevMouseY = getWindowHeight() / 2;
		setCursorPosition(m_prevMouseX, m_prevMouseY);

		const Transform transform = getEntityComponent<Transform>(entityID);
		Math::vec3 cameraRotation = Math::normalize(Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]));

		m_yaw = Math::toDeg(std::atan2(cameraRotation.z, cameraRotation.x));
		m_pitch = Math::toDeg(-std::asin(cameraRotation.y));

		setSoundListenerEntity(entityID);
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

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

			float yawRad = Math::toRad(m_yaw);
			float pitchRad = Math::toRad(m_pitch);

			transform.rotation.x = std::cos(pitchRad) * std::cos(yawRad);
			transform.rotation.y = -std::sin(pitchRad);
			transform.rotation.z = std::cos(pitchRad) * std::sin(yawRad);
			transform.rotation = Math::normalize(transform.rotation);
		}

		Rigidbody& rigidbody = getEntityComponent<Rigidbody>(entityID);

		if (getKeyState(InputKeyboardKey::W) == InputState::Held) {
			rigidbody.force += (transform.rotation * m_cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::S) == InputState::Held) {
			rigidbody.force -= (transform.rotation * m_cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::A) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
			rigidbody.force.x -= (t.x * m_cameraSpeed);
			rigidbody.force.z -= (t.z * m_cameraSpeed);
		}
		if (getKeyState(InputKeyboardKey::D) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
			rigidbody.force.x += (t.x * m_cameraSpeed);
			rigidbody.force.z += (t.z * m_cameraSpeed);
		}

		Collidable& collidable = getEntityComponent<Collidable>(entityID);
		ColliderSphere* collider = static_cast<ColliderSphere*>(collidable.collider.get());
		if (getKeyState(InputKeyboardKey::Space) == InputState::Held) {
			if (!raycast(transform.position + Math::vec3(0.0f, -(collider->radius * transform.scale.x) + 0.001f, 0.0f), Math::vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.001f).empty()) {
					rigidbody.force.y += m_cameraSpeed * 100.0f;
			}
		}
	}

	void destroy() {
	}

private:
	bool m_mouseMiddleMode = true;

	const float m_cameraSpeed = 500.0f;
	const float m_mouseSensitivity = 0.12f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
};