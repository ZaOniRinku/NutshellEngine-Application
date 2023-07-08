#include "Core/src/ntshengn_core.h"
#include "scripts/camerascript.h"
#include "scripts/clientscript.h"
#include "scripts/serverscript.h"

void scene(NtshEngn::Core& core) {
	using namespace NtshEngn;

	ECS* ecs = core.getECS();
	//AssetManager* assetManager = core.getAssetManager();

	//const float toRad = 3.1415926535897932384626433832795f / 180.0f;

	// Create the camera
	Entity camera = ecs->createEntity();

	Transform& cameraTransform = ecs->getComponent<Transform>(camera);
	cameraTransform.position = { 0.0f, 0.0f, -2.0f };
	cameraTransform.rotation = { 0.0f, 0.0f, 1.0f };

	Camera cameraCamera;
	cameraCamera.fov = 45.0f;
	cameraCamera.nearPlane = 0.03f;
	cameraCamera.farPlane = 100.0f;
	ecs->addComponent(camera, cameraCamera);

	Scriptable cameraScriptable;
	cameraScriptable.script = std::make_unique<CameraScript>();
	ecs->addComponent(camera, cameraScriptable);

	// Create a light
	Entity light = ecs->createEntity();

	Transform& lightTransform = ecs->getComponent<Transform>(light);
	lightTransform.rotation = { 1.0f, -1.0f, 0.0f };

	Light lightLight;
	lightLight.type = LightType::Directional;
	lightLight.color = { 1.0f, 1.0f, 1.0f };
	ecs->addComponent(light, lightLight);

	if (core.getGraphicsModule()) {
		// Client
		Entity client = ecs->createEntity();

		Scriptable clientScriptable;
		clientScriptable.script = std::make_unique<ClientScript>();
		ecs->addComponent(client, clientScriptable);
	}
	else {
		// Server
		Entity server = ecs->createEntity();

		Scriptable serverScriptable;
		serverScriptable.script = std::make_unique<ServerScript>();
		ecs->addComponent(server, serverScriptable);
	}
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