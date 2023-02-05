#include "external/Core/src/ntshengn_core.h"
#include "scripts/camerascript.h"
#include "scripts/rootscript.h"
#include <random>

void scene(NtshEngn::Core& core) {
	NtshEngn::ECS* ecs = core.getECS();

	// Create the camera
	NtshEngn::Entity camera = ecs->createEntity();

	NtshEngn::Camera cameraCamera;
	cameraCamera.fov = 45.0f;
	cameraCamera.nearPlane = 0.03f;
	cameraCamera.farPlane = 100.0f;
	ecs->addComponent(camera, cameraCamera);

	NtshEngn::Scriptable cameraScriptable;
	cameraScriptable.script = std::make_unique<CameraScript>();
	ecs->addComponent(camera, cameraScriptable);

	// Create the root
	NtshEngn::Entity root = ecs->createEntity();

	NtshEngn::Transform& rootTransform = ecs->getComponent<NtshEngn::Transform>(root);
	rootTransform.position = { 0.0f, 0.0f, 0.0f }; // Center
	rootTransform.rotation = { -100.0f, 0.0f, 0.0f }; // Head
	rootTransform.scale = { 100.0f, 0.0f, 0.0f }; // Tail

	NtshEngn::Scriptable rootScriptable;
	rootScriptable.script = std::make_unique<RootScript>();
	ecs->addComponent(root, rootScriptable);

	NtshEngn::Renderable rootRenderable;
	ecs->addComponent(root, rootRenderable);

	NtshEngn::SphereCollidable rootCollidable;
	rootCollidable.collider.center = rootTransform.rotation;
	rootCollidable.collider.radius = 10.0f;
	ecs->addComponent(root, rootCollidable);

	// Lights
	std::vector<nml::vec2> lightPositions;
	std::vector<nml::vec3> lightColors;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> pos(-10000.0f, 10000.0f);
	std::uniform_real_distribution<float> col(0.0f, 1.0f);
	for (size_t i = 0; i < 25; i++) {
		lightPositions.push_back({ pos(gen), pos(gen) });
		lightColors.push_back({ col(gen), col(gen), col(gen) });
	}

	NtshEngn::AABBCollidable lightAABB;
	lightAABB.collider.min = { -10.0f, -10.0f, -10.0f };
	lightAABB.collider.max = { 10.0f, 10.0f, 10.0f };

	for (size_t i = 0; i < lightPositions.size(); i++) {
		NtshEngn::Entity light = ecs->createEntity();

		NtshEngn::Transform& lightTransform = ecs->getComponent<NtshEngn::Transform>(light);
		lightTransform.position[0] = lightPositions[i].x;
		lightTransform.position[1] = 10.0f;
		lightTransform.position[2] = lightPositions[i].y;
		lightTransform.rotation[0] = lightColors[i].x;
		lightTransform.rotation[1] = lightColors[i].y;
		lightTransform.rotation[2] = lightColors[i].z;

		ecs->addComponent(light, lightAABB);
	}
}

int main() {
	NtshEngn::Core core;

	// Initialize
	core.init();
	core.getWindowModule()->setTitle(NTSHENGN_MAIN_WINDOW, "The Origin of Light");

	// Scene
	scene(core);

	// Loop
	core.update();

	// Destroy
	core.destroy();

	return 0;
}