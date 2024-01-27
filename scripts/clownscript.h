#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include <random>

using namespace NtshEngn;
struct ClownScript : public Script {
	NTSHENGN_SCRIPT(ClownScript);

	void init() {
	}

	void update(double dt) {
		float deltaTime = static_cast<float>(dt);
		NTSHENGN_UNUSED(deltaTime);

		const Transform& transform = getEntityComponent<Transform>(entityID);
		if (transform.position.y < -5.0f) {
			destroyEntity(entityID);
			return;
		}

		const Collidable& collidable = getEntityComponent<Collidable>(entityID);
		const ColliderSphere* collider = static_cast<ColliderSphere*>(collidable.collider.get());
		Rigidbody& rigidbody = getEntityComponent<Rigidbody>(entityID);

		if (!raycast(transform.position + Math::vec3(0.0f, -(collider->radius * transform.scale.x) + 0.001f, 0.0f), Math::vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.001f).empty()) {
			std::uniform_real_distribution<float> distM100To100(-100.0f, 100.0f);
			std::uniform_real_distribution<float> dist0To2PI(0.0f, 2.0f * Math::PI);

			if (aerial) {
				playSoundAtPosition(boingSound, transform.position, 0.5f, 1.0f / transform.scale.x);
				aerial = false;
			}

			rigidbody.force += Math::vec3(distM100To100(rand), 0.0f, distM100To100(rand));
			rigidbody.torque += Math::vec3(dist0To2PI(rand), dist0To2PI(rand), dist0To2PI(rand));
		}
		else {
			aerial = true;
		}
	}

	void destroy() {
	}

public:
	SoundID boingSound;

private:
	std::default_random_engine rand;
	bool aerial = false;
};