#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
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

			nml::vec3 position = nml::vec3(transform.position.data());
			nml::vec3 rotation = nml::vec3(transform.rotation.data());
			nml::vec3 scale = nml::vec3(transform.scale.data());
			nml::vec3 cameraRotation = nml::vec3(cameraTransform.rotation.data());
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Up) == NtshEngn::InputState::Held) {
				position.x += (cameraRotation.x * cubeSpeed);
				position.z += (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Down) == NtshEngn::InputState::Held) {
				position.x -= (cameraRotation.x * cubeSpeed);
				position.z -= (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Left) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				position.x -= (t.x * cubeSpeed);
				position.z -= (t.z * cubeSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Right) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
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
				rotationMouseStart = nml::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
			}
			else if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::One) == NtshEngn::InputState::Held) {
				const nml::vec2 rotationMouseEnd = nml::vec2(static_cast<float>(windowModule->getCursorPositionX(windowModule->getMainWindowID())), static_cast<float>(windowModule->getCursorPositionY(windowModule->getMainWindowID())));
				const nml::vec2 rotationMouse = (rotationMouseEnd - rotationMouseStart);
				const nml::vec2 windowSize = nml::vec2(static_cast<float>(windowModule->getWidth(windowModule->getMainWindowID())), static_cast<float>(windowModule->getHeight(windowModule->getMainWindowID())));
				const nml::vec2 rotationMouseNormalized = nml::vec2(rotationMouse.x / windowSize.x, rotationMouse.y / windowSize.y);

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

	nml::vec2 rotationMouseStart;
};