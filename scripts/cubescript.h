#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>

struct CubeScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(CubeScript);

	void init() {
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {
			NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(m_camera);
			const float cubeSpeed = m_cubeSpeed * static_cast<float>(dt);

			NtshEngn::Rigidbody& rigidbody = ecs->getComponent<NtshEngn::Rigidbody>(entityID);

			nml::vec3 cameraRotation = nml::vec3(cameraTransform.rotation.data());
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Up) == NtshEngn::InputState::Held) {
				rigidbody.force[0] += (cameraRotation.x * cubeSpeed);
				rigidbody.force[2] += (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Down) == NtshEngn::InputState::Held) {
				rigidbody.force[0] -= (cameraRotation.x * cubeSpeed);
				rigidbody.force[2] -= (cameraRotation.z * cubeSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Left) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				rigidbody.force[0] -= (t.x * cubeSpeed);
				rigidbody.force[2] -= (t.z * cubeSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Right) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				rigidbody.force[0] += (t.x * cubeSpeed);
				rigidbody.force[2] += (t.z * cubeSpeed);
			}
		}
	}

	void destroy() {
	}

private:
	const float m_cubeSpeed = 10.0f;

	NtshEngn::Entity m_camera = 0;
};