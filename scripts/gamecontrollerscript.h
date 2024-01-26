#pragma once
#include "../Core/scripting/ntshengn_script.h"
#include "clownscript.h"
#include <random>

using namespace NtshEngn;
struct GameControllerScript : public Script {
	NTSHENGN_SCRIPT(GameControllerScript);

	Entity createClown(Math::vec3 position, float scale) {
		Entity newClown = createEntity();

		Transform& clownTransform = getEntityComponent<Transform>(newClown);
		clownTransform.position = position;
		clownTransform.scale = Math::vec3(scale);

		Renderable clownRenderable;
		clownRenderable.model = clownModel;
		clownRenderable.modelPrimitiveIndex = 0;
		addEntityComponent(newClown, clownRenderable);

		Collidable clownCollidable;
		clownCollidable.collider = std::make_unique<ColliderSphere>();
		ColliderSphere* collidableSphere = static_cast<ColliderSphere*>(clownCollidable.collider.get());
		collidableSphere->radius = 1.0f;
		addEntityComponent(newClown, clownCollidable);

		Rigidbody clownRigidbody;
		clownRigidbody.restitution = 1.0f;
		addEntityComponent(newClown, clownRigidbody);

		Scriptable clownScriptable;
		clownScriptable.script = std::make_unique<ClownScript>();
		ClownScript* clownScript = static_cast<ClownScript*>(clownScriptable.script.get());
		clownScript->boingSound = clownBoingSound;
		addEntityComponent(newClown, clownScriptable);

		return newClown;
	}

	void init() {
		clownModel = loadModel("assets/models/clown.gltf");
		clownBoingSound = getSoundID(*loadSound("assets/sounds/clownboing.wav"));
	}

	void update(double dt) {
		time += dt;
		if (time >= 1000.0) {
			std::uniform_real_distribution<float> distM10To10(-10.0f, 10.0f);
			std::uniform_real_distribution<float> dist0P5To2P5(0.5f, 2.5f);
			createClown(Math::vec3(distM10To10(rand), 2.0f, distM10To10(rand)), dist0P5To2P5(rand));

			time -= 1000.0;
		}
	}

	void destroy() {
	}

private:
	Model* clownModel;
	SoundID clownBoingSound;
	double time = 0.0;
	std::default_random_engine rand;
};