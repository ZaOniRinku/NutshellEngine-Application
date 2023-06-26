#pragma once
#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>

struct FootballScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(FootballScript);

	void init() {

	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

		const nml::vec3 cameraPosition = nml::vec3(ecs->getComponent<NtshEngn::Transform>(0).position.data());

		NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
		nml::vec3 newRotation = cameraPosition - nml::vec3(transform.position.data());
		transform.rotation[1] = std::atan2(newRotation.x, newRotation.z);

		if (!moved) {
			const nml::vec3 footballPosition = nml::vec3(ecs->getComponent<NtshEngn::Transform>(entityID).position.data());

			const float distance = (cameraPosition - footballPosition).length();
			if (distance < 0.5f) {
				moved = true;

				if (!audioModule->isPlaying(3)) {
					audioModule->play(3);
				}

				NtshEngn::Transform& footballTransform = ecs->getComponent<NtshEngn::Transform>(entityID);
				footballTransform.position[0] = 4.9f;
				footballTransform.position[2] = 4.0f;

				NtshEngn::Transform& windowTransform = ecs->getComponent<NtshEngn::Transform>(m_windowEntity);
				windowTransform.position[1] = 1000.0f;

				NtshEngn::Transform& windowBrokenTransform = ecs->getComponent<NtshEngn::Transform>(m_windowBrokenEntity);
				windowBrokenTransform.position[1] = -0.5f;
			}
		}
	}

	void destroy() {

	}

private:
	std::array<nml::vec3, 2> getMeshMinMax(const NtshEngn::Mesh& mesh) {
		nml::vec3 min = nml::vec3(std::numeric_limits<float>::max());
		nml::vec3 max = nml::vec3(std::numeric_limits<float>::lowest());
		for (const NtshEngn::Vertex& vertex : mesh.vertices) {
			if (vertex.position[0] < min.x) {
				min.x = vertex.position[0];
			}
			if (vertex.position[0] > max.x) {
				max.x = vertex.position[0];
			}

			if (vertex.position[1] < min.y) {
				min.y = vertex.position[1];
			}
			if (vertex.position[1] > max.y) {
				max.y = vertex.position[1];
			}

			if (vertex.position[2] < min.z) {
				min.z = vertex.position[2];
			}
			if (vertex.position[2] > max.z) {
				max.z = vertex.position[2];
			}
		}


		return { min, max };
	}

private:
	const NtshEngn::Entity m_windowEntity = 58;
	const NtshEngn::Entity m_windowBrokenEntity = 31;

	bool moved = false;
};