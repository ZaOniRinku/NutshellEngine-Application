#pragma once
#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>

struct BillboardScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(BillboardScript);

	void init() {
		
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

		nml::vec3 cameraPosition = nml::vec3(ecs->getComponent<NtshEngn::Transform>(0).position.data());

		NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
		nml::vec3 newRotation = cameraPosition - nml::vec3(transform.position.data());
		transform.rotation[1] = std::atan2(newRotation.x, newRotation.z);
	}

	void destroy() {

	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;
};