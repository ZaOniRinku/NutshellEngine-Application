#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>
#include <algorithm>
#include <random>

struct CubeScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(CubeScript);

	void init() {
		m_camera = ecs->findEntityByName("camera");
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(m_camera);
			const float cubeSpeed = m_cubeSpeed * static_cast<float>(dt);

			NtshEngn::Math::vec3 position = NtshEngn::Math::vec3(transform.position.data());
			if (position.y < -100.0f) {
				ecs->destroyEntity(entityID);
				return;
			}
			NtshEngn::Math::vec3 rotation = NtshEngn::Math::vec3(transform.rotation.data());
			NtshEngn::Math::vec3 scale = NtshEngn::Math::vec3(transform.scale.data());
			NtshEngn::Math::vec3 cameraRotation = NtshEngn::Math::vec3(cameraTransform.rotation.data());
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Up) == NtshEngn::InputState::Held) {
				position.x += (cameraRotation.x * cubeSpeed);
				position.z += (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Down) == NtshEngn::InputState::Held) {
				position.x -= (cameraRotation.x * cubeSpeed);
				position.z -= (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Left) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				position.x -= (t.x * cubeSpeed);
				position.z -= (t.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Right) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				position.x += (t.x * cubeSpeed);
				position.z += (t.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::NumPlus) == NtshEngn::InputState::Held) {
				position.y += cubeSpeed;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::NumMinus) == NtshEngn::InputState::Held) {
				position.y -= cubeSpeed;
			}

			if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::One) == NtshEngn::InputState::Pressed) {
				rotationMouseStart = NtshEngn::Math::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
			}
			else if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::One) == NtshEngn::InputState::Held) {
				const NtshEngn::Math::vec2 rotationMouseEnd = NtshEngn::Math::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
				const NtshEngn::Math::vec2 rotationMouse = (rotationMouseEnd - rotationMouseStart);
				const NtshEngn::Math::vec2 windowSize = NtshEngn::Math::vec2(static_cast<float>(windowModule->getWidth(windowModule->getMainWindowID())), static_cast<float>(windowModule->getHeight(windowModule->getMainWindowID())));
				const NtshEngn::Math::vec2 rotationMouseNormalized = NtshEngn::Math::vec2(rotationMouse.x / windowSize.x, rotationMouse.y / windowSize.y);

				rotation.x += rotationMouseNormalized.y * static_cast<float>(dt);
				rotation.y += rotationMouseNormalized.x * static_cast<float>(dt);

				rotationMouseStart = rotationMouseEnd;
			}

			scale += windowModule->getMouseScrollOffsetY(windowModule->getMainWindowID()) * cubeSpeed;

			transform.position = { position.x, position.y, position.z };
			transform.rotation = { rotation.x, rotation.y, rotation.z };
			transform.scale = { scale.x, scale.y, scale.z };

			NtshEngn::Rigidbody& rigidbody = ecs->getComponent<NtshEngn::Rigidbody>(entityID);

			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::G) == NtshEngn::InputState::Pressed) {
				rigidbody.isStatic = !rigidbody.isStatic;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::F) == NtshEngn::InputState::Pressed) {
				rigidbody.isStatic = false;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
				rigidbody.force = { dis(gen) * 500.0f, dis(gen) * 500.0f, dis(gen) * 500.0f };
			}
		}
	}

	void destroy() {
	}

private:
	const float m_cubeSpeed = 0.005f;

	NtshEngn::Entity m_camera;

	NtshEngn::Math::vec2 rotationMouseStart;
};