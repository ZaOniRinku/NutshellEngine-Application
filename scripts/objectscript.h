#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/module_interfaces/ntshengn_window_module_interface.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>
#include <algorithm>
#include <random>

struct ObjectScript : public NtshEngn::Script {
	NTSHENGN_SCRIPT(ObjectScript);

	void init() {
		m_camera = ecs->findEntityByName("Camera");
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			const NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(m_camera);
			const float objectSpeed = m_objectSpeed * static_cast<float>(dt);

			if (transform.position.y < -100.0f) {
				ecs->destroyEntity(entityID);
				return;
			}

			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Up) == NtshEngn::InputState::Held) {
				transform.position.x += (cameraTransform.rotation.x * objectSpeed);
				transform.position.z += (cameraTransform.rotation.z * objectSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Down) == NtshEngn::InputState::Held) {
				transform.position.x -= (cameraTransform.rotation.x * objectSpeed);
				transform.position.z -= (cameraTransform.rotation.z * objectSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Left) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraTransform.rotation.z, 0.0, cameraTransform.rotation.x));
				transform.position.x -= (t.x * objectSpeed);
				transform.position.z -= (t.z * objectSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Right) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraTransform.rotation.z, 0.0, cameraTransform.rotation.x));
				transform.position.x += (t.x * objectSpeed);
				transform.position.z += (t.z * objectSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::NumPlus) == NtshEngn::InputState::Held) {
				transform.position.y += objectSpeed;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::NumMinus) == NtshEngn::InputState::Held) {
				transform.position.y -= objectSpeed;
			}

			if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::Two) == NtshEngn::InputState::Pressed) {
				rotationMouseStart = NtshEngn::Math::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
			}
			else if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::Two) == NtshEngn::InputState::Held) {
				const NtshEngn::Math::vec2 rotationMouseEnd = NtshEngn::Math::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
				const NtshEngn::Math::vec2 rotationMouse = (rotationMouseEnd - rotationMouseStart);
				const NtshEngn::Math::vec2 windowSize = NtshEngn::Math::vec2(static_cast<float>(windowModule->getWidth(windowModule->getMainWindowID())), static_cast<float>(windowModule->getHeight(windowModule->getMainWindowID())));
				const NtshEngn::Math::vec2 rotationMouseNormalized = NtshEngn::Math::vec2(rotationMouse.x / windowSize.x, rotationMouse.y / windowSize.y);

				transform.rotation.x += rotationMouseNormalized.y * static_cast<float>(dt);
				transform.rotation.y += rotationMouseNormalized.x * static_cast<float>(dt);

				rotationMouseStart = rotationMouseEnd;
			}

			transform.scale += windowModule->getMouseScrollOffsetY(windowModule->getMainWindowID()) * objectSpeed;

			NtshEngn::Rigidbody& rigidbody = ecs->getComponent<NtshEngn::Rigidbody>(entityID);

			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::G) == NtshEngn::InputState::Pressed) {
				rigidbody.isStatic = !rigidbody.isStatic;
			}
			if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::One) == NtshEngn::InputState::Pressed) {
				rigidbody.isStatic = false;
				/*std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> dis(-1.0f, 1.0f);*/
				rigidbody.force = cameraTransform.rotation * 10.0f;
			}
		}
	}

	void destroy() {
	}

private:
	const float m_objectSpeed = 0.005f;

	NtshEngn::Entity m_camera;

	NtshEngn::Math::vec2 rotationMouseStart;
};