#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include <random>

using namespace NtshEngn;
struct ObjectScript : public Script {
	NTSHENGN_SCRIPT(ObjectScript);

	void init() {
		m_camera = findEntityByName("Camera");
	}

	void update(double dt) {
		Transform& transform = getEntityComponent<Transform>(entityID);
		const Transform& cameraTransform = getEntityComponent<Transform>(m_camera);
		const float objectSpeed = (m_objectSpeed / 200.0f) * static_cast<float>(dt);

		if (transform.position.y < -100.0f) {
			destroyEntity(entityID);
			return;
		}

		if (getKeyState(InputKeyboardKey::Up) == InputState::Held) {
			transform.position.x += (cameraTransform.rotation.x * objectSpeed);
			transform.position.z += (cameraTransform.rotation.z * objectSpeed);
		}
		if (getKeyState(InputKeyboardKey::Down) == InputState::Held) {
			transform.position.x -= (cameraTransform.rotation.x * objectSpeed);
			transform.position.z -= (cameraTransform.rotation.z * objectSpeed);
		}
		if (getKeyState(InputKeyboardKey::Left) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraTransform.rotation.z, 0.0, cameraTransform.rotation.x));
			transform.position.x -= (t.x * objectSpeed);
			transform.position.z -= (t.z * objectSpeed);
		}
		if (getKeyState(InputKeyboardKey::Right) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraTransform.rotation.z, 0.0, cameraTransform.rotation.x));
			transform.position.x += (t.x * objectSpeed);
			transform.position.z += (t.z * objectSpeed);
		}
		if (getKeyState(InputKeyboardKey::NumPlus) == InputState::Held) {
			transform.position.y += objectSpeed;
		}
		if (getKeyState(InputKeyboardKey::NumMinus) == InputState::Held) {
			transform.position.y -= objectSpeed;
		}

		if (getMouseButtonState(InputMouseButton::Two) == InputState::Pressed) {
			rotationMouseStart = Math::vec2(static_cast<float>(getCursorPositionX()), static_cast<float>(getCursorPositionY()));
		}
		else if (getMouseButtonState(InputMouseButton::Two) == InputState::Held) {
			const Math::vec2 rotationMouseEnd = Math::vec2(static_cast<float>(getCursorPositionX()), static_cast<float>(getCursorPositionY()));
			const Math::vec2 rotationMouse = (rotationMouseEnd - rotationMouseStart);
			const Math::vec2 windowSize = Math::vec2(static_cast<float>(getWindowWidth()), static_cast<float>(getWindowHeight()));
			const Math::vec2 rotationMouseNormalized = Math::vec2(rotationMouse.x / windowSize.x, rotationMouse.y / windowSize.y);

			transform.rotation.x += rotationMouseNormalized.y * static_cast<float>(dt);
			transform.rotation.y += rotationMouseNormalized.x * static_cast<float>(dt);

			rotationMouseStart = rotationMouseEnd;
		}

		if (getKeyState(InputKeyboardKey::Num0) == InputState::Pressed) {
			if (!isAnimationPlaying(entityID, 0)) {
				playAnimation(entityID, 0);
			}
			else {
				pauseAnimation(entityID);
			}
		}
		if (getKeyState(InputKeyboardKey::Num1) == InputState::Pressed) {
			if (!isAnimationPlaying(entityID, 1)) {
				playAnimation(entityID, 1);
			}
			else {
				pauseAnimation(entityID);
			}
		}
		if (getKeyState(InputKeyboardKey::Num2) == InputState::Pressed) {
			if (!isAnimationPlaying(entityID, 2)) {
				playAnimation(entityID, 2);
			}
			else {
				pauseAnimation(entityID);
			}
		}
		if (getKeyState(InputKeyboardKey::Num3) == InputState::Pressed) {
			if (!isAnimationPlaying(entityID, 3)) {
				playAnimation(entityID, 3);
			}
			else {
				pauseAnimation(entityID);
			}
		}
		if (getKeyState(InputKeyboardKey::Num4) == InputState::Pressed) {
			if (!isAnimationPlaying(entityID, 4)) {
				playAnimation(entityID, 4);
			}
			else {
				pauseAnimation(entityID);
			}
		}

		transform.scale += getMouseScrollOffsetY() * objectSpeed;
		/*transform.scale.x = std::max(0.1f, transform.scale.x);
		transform.scale.y = std::max(0.1f, transform.scale.y);
		transform.scale.z = std::max(0.1f, transform.scale.z);*/

		Rigidbody& rigidbody = getEntityComponent<Rigidbody>(entityID);
		//rigidbody.mass = transform.scale.x;

		if (getKeyState(InputKeyboardKey::G) == InputState::Pressed) {
			rigidbody.isStatic = !rigidbody.isStatic;
		}
		if (getKeyState(InputKeyboardKey::F) == InputState::Held) {
			rigidbody.isStatic = false;
			rigidbody.force = cameraTransform.rotation * objectSpeed;
		}
	}

	void destroy() {
	}

private:
	const float m_objectSpeed = 2.0f;

	Entity m_camera;

	Math::vec2 rotationMouseStart;
};