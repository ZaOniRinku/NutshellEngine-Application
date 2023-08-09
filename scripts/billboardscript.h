#pragma once
#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include "../Core/Common/utils/ntshengn_defines.h"
#include <cmath>

struct BillboardScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(BillboardScript);

	void init() {
		
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

		NtshEngn::Math::vec3 cameraPosition = NtshEngn::Math::vec3(ecs->getComponent<NtshEngn::Transform>(0).position.data());

		NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
		NtshEngn::Math::vec3 newRotation = cameraPosition - NtshEngn::Math::vec3(transform.position.data());
		transform.rotation[1] = std::atan2(newRotation.x, newRotation.z);
	}

	void destroy() {

	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;
};