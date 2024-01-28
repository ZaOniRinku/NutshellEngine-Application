#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include <cmath>

using namespace NtshEngn;
struct SignScript : public Script {
	NTSHENGN_SCRIPT(SignScript);

	void init() {
		camera = findEntityByName("Camera");
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);

		const Transform& cameraTransform = getEntityComponent<Transform>(camera);
		Transform& transform = getEntityComponent<Transform>(entityID);
		const Math::vec3 newRotation = cameraTransform.position - transform.position;
		transform.rotation[1] = std::atan2(newRotation.x, newRotation.z);
	}

	void destroy() {
	}

private:
	Entity camera;
};