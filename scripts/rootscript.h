#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>

struct RootScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(RootScript);

	void init() {
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {

			NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(m_camera);

			const float speed = m_speed * static_cast<float>(dt);

			nml::vec3 position = nml::vec3(transform.position.data());
			nml::vec3 cameraRotation = nml::vec3(cameraTransform.rotation.data());
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				position.x += (cameraRotation.x * speed);
				position.z += (cameraRotation.z * speed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				position.x -= (cameraRotation.x * speed);
				position.z -= (cameraRotation.z * speed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				position.x -= (t.x * speed);
				position.z -= (t.z * speed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				position.x += (t.x * speed);
				position.z += (t.z * speed);
			}
			std::cout << nml::to_string(position) << std::endl;

			transform.position = { position.x, position.y, position.z };
			transform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
		}
	}

	void destroy() {
	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;

	const float m_speed = 0.05f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	NtshEngn::Entity m_camera = 0;
};