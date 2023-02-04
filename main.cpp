#include "external/Core/src/ntshengn_core.h"
#include "scripts/camerascript.h"
#include "scripts/rootscript.h"

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
	rootTransform.position = { 0.0f, 0.0f, 0.0f };
	rootTransform.rotation = { 0.0f, 0.0f, 0.0f };

	NtshEngn::Scriptable rootScriptable;
	rootScriptable.script = std::make_unique<RootScript>();
	ecs->addComponent(root, rootScriptable);
}

int main() {
	NtshEngn::Core core;

	// Initialize
	core.init();

	// Scene
	scene(core);

	// Loop
	core.update();

	// Destroy
	core.destroy();

	return 0;
}