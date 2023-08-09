#pragma once
#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>

struct FootballScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(FootballScript);

	void init() {

	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

		const NtshEngn::Math::vec3 cameraPosition = NtshEngn::Math::vec3(ecs->getComponent<NtshEngn::Transform>(0).position.data());

		NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
		NtshEngn::Math::vec3 newRotation = cameraPosition - NtshEngn::Math::vec3(transform.position.data());
		transform.rotation[1] = std::atan2(newRotation.x, newRotation.z);

		if (!moved) {
			const NtshEngn::Math::vec3 footballPosition = NtshEngn::Math::vec3(ecs->getComponent<NtshEngn::Transform>(entityID).position.data());

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
	const NtshEngn::Entity m_windowEntity = 58;
	const NtshEngn::Entity m_windowBrokenEntity = 31;

	bool moved = false;
};