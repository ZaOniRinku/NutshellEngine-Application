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
			const float rootSpeed = m_rootSpeed * static_cast<float>(dt);

			nml::vec3 centerPosition = nml::vec3(transform.position.data());
			nml::vec3 headPosition = nml::vec3(transform.rotation.data());
			nml::vec3 tailPosition = nml::vec3(transform.scale.data());

			tailPosition.x = lerp(tailPosition.x, centerPosition.x, 0.05f);
			tailPosition.z = lerp(tailPosition.z, centerPosition.z, 0.05f);
			centerPosition.x = lerp(centerPosition.x, headPosition.x, 0.05f);
			centerPosition.z = lerp(centerPosition.z, headPosition.z, 0.05f);

			nml::vec3 cameraRotation = nml::vec3(cameraTransform.rotation.data());
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				headPosition.x += (cameraRotation.x * rootSpeed);
				headPosition.z += (cameraRotation.z * rootSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				headPosition.x -= (cameraRotation.x * rootSpeed);
				headPosition.z -= (cameraRotation.z * rootSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				headPosition.x -= (t.x * rootSpeed);
				headPosition.z -= (t.z * rootSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				headPosition.x += (t.x * rootSpeed);
				headPosition.z += (t.z * rootSpeed);
			}

			transform.position = { centerPosition.x, centerPosition.y, centerPosition.z };
			transform.rotation = { headPosition.x, headPosition.y, headPosition.z };
			transform.scale = { tailPosition.x, tailPosition.y, tailPosition.z };

			if (physicsModule) {
				for (NtshEngn::Entity other : physicsModule->m_entities) {
					if (other != entityID) {
						NtshEngn::Transform otherTransform = ecs->getComponent<NtshEngn::Transform>(other);
						nml::vec3 otherPosition = otherTransform.position.data();
						const float distance = (otherPosition - headPosition).length();
						if (distance < 100.0f) {
							m_entitiesToDestroy.push_back(other);
						}
					}
				}
			}

			for (size_t i = 0; i < m_entitiesToDestroy.size(); i++) {
				ecs->destroyEntity(m_entitiesToDestroy[i]);
			}
			m_entitiesToDestroy.clear();
			m_entitiesToDestroy.shrink_to_fit();
		}
	}

	void destroy() {
	}

private:
	float lerp(float a, float b, float t) { return a + t * (b - a); }

private:
	const float m_rootSpeed = 0.5f;

	const NtshEngn::Entity m_camera = 0;

	std::vector<NtshEngn::Entity> m_entitiesToDestroy;
};