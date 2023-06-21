#include "external/Core/src/ntshengn_core.h"
#include "scripts/camerascript.h"

void scene(NtshEngn::Core& core) {
	NtshEngn::ECS* ecs = core.getECS();

	// Create the camera
	NtshEngn::Entity camera = ecs->createEntity();

	NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(camera);
	cameraTransform.position = { 0.0f, 2.0f, 5.0f };
	cameraTransform.rotation = { 0.0f, 0.0f, -1.0f };

	NtshEngn::Camera cameraCamera;
	cameraCamera.fov = 45.0f;
	cameraCamera.nearPlane = 0.03f;
	cameraCamera.farPlane = 100.0f;
	ecs->addComponent(camera, cameraCamera);

	NtshEngn::Scriptable cameraScriptable;
	cameraScriptable.script = std::make_unique<CameraScript>();
	ecs->addComponent(camera, cameraScriptable);
}

int main() {
	NtshEngn::Core core;

	// Initialize
	core.init();

	core.getWindowModule()->setTitle(NTSHENGN_MAIN_WINDOW, "NutshellShaderEditor");
	core.setMaxFPS(60);

	// Scene
	scene(core);

	// Loop
	core.update();

	// Destroy
	core.destroy();

	return 0;
}