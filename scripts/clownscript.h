#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include <random>

using namespace NtshEngn;
struct ClownScript : public Script {
	NTSHENGN_SCRIPT(ClownScript);

	void init() {
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);
	}

	void destroy() {
	}

	void onCollisionEnter(CollisionInfo collisionInfo) {
		const Transform& transform = getEntityComponent<Transform>(entityID);
		Rigidbody& rigidbody = getEntityComponent<Rigidbody>(entityID);

		playSoundAtPosition(boingSound, transform.position, 0.5f, 1.0f / transform.scale.x);
		std::uniform_real_distribution<float> dist0To2PI(0.0f, 2.0f * Math::PI);
		rigidbody.torque += Math::vec3(dist0To2PI(rand), dist0To2PI(rand), dist0To2PI(rand));
	}

public:
	SoundID boingSound;
	int type;
	int color;

private:
	std::default_random_engine rand;
	bool aerial = false;
};