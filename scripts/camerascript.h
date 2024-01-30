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
		float deltaTime = static_cast<float>(dt / 1.0);

		if (getKeyState(InputKeyboardKey::Escape) == InputState::Pressed) {
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

		Math::vec3 direction = Math::vec3(0.0f, 0.0f, 0.0f);
		if (getKeyState(InputKeyboardKey::W) == InputState::Held) {
			const Math::vec3 r = Math::normalize(Math::vec3(transform.rotation.x, 0.0f, transform.rotation.z));
			direction.x += r.x;
			direction.z += r.z;
		}
		if (getKeyState(InputKeyboardKey::S) == InputState::Held) {
			const Math::vec3 r = Math::normalize(Math::vec3(transform.rotation.x, 0.0f, transform.rotation.z));
			direction.x -= r.x;
			direction.z -= r.z;
		}
		if (getKeyState(InputKeyboardKey::A) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
			direction.x -= t.x;
			direction.z -= t.z;
		}
		if (getKeyState(InputKeyboardKey::D) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
			direction.x += t.x;
			direction.z += t.z;
		}
		if (Math::dot(direction, direction) > 0.0f) {
			direction = Math::normalize(direction) * (m_cameraSpeed * deltaTime);
			rigidbody.linearVelocity.x = direction.x;
			rigidbody.linearVelocity.z = direction.z;
		}
		if ((getKeyState(InputKeyboardKey::W) == InputState::None) && (getKeyState(InputKeyboardKey::A) == InputState::None) && (getKeyState(InputKeyboardKey::S) == InputState::None) && (getKeyState(InputKeyboardKey::D) == InputState::None)) {
			rigidbody.linearVelocity = Math::vec3(0.0f, rigidbody.linearVelocity.y, 0.0f);
		}

		if (m_onTheGround) {
			if (getKeyState(InputKeyboardKey::Space) == InputState::Pressed) {
				rigidbody.linearVelocity.y += m_jumpSpeed * deltaTime;
			}
		}
	}

	void destroy() {
	}

	void onCollisionEnter(CollisionInfo collisionInfo) {
		if (Math::dot(collisionInfo.relativePoints[0], Math::vec3(0.0f, -1.0f, 0.0f)) > 0.0f) {
			m_onTheGround = true;
		}
	}

	void onCollisionStill(CollisionInfo collisionInfo) {
		if (Math::dot(collisionInfo.relativePoints[0], Math::vec3(0.0f, -1.0f, 0.0f)) > 0.0f) {
			m_onTheGround = true;
		}
	}

	void onCollisionExit(CollisionInfo collisionInfo) {
		if (Math::dot(collisionInfo.relativePoints[0], Math::vec3(0.0f, -1.0f, 0.0f)) > 0.0f) {
			m_onTheGround = false;
		}
	}

private:
	bool m_mouseMiddleMode = true;

	const float m_cameraSpeed = 1.0f;
	const float m_jumpSpeed = 0.5f;
	const float m_mouseSensitivity = 0.12f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	bool m_onTheGround = false;
};