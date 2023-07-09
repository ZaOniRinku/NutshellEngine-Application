#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>

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

			transform.position = { position.x, position.y, position.z };
		}
	}

	void destroy() {
	}

private:
	const float m_cubeSpeed = 0.005f;

	NtshEngn::Entity m_camera;
};