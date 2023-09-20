#include "../Core/scripting/ntshengn_script.h"
#include <cmath>

using namespace NtshEngn;
struct CubeScript : Script {
	NTSHENGN_SCRIPT(CubeScript);

	void init() {
		m_camera = findEntityByName("camera");
	}

	void update(double dt) {
		Transform& transform = getEntityComponent<Transform>(entityID);
		Transform& cameraTransform = getEntityComponent<Transform>(m_camera);
		const float cubeSpeed = m_cubeSpeed * static_cast<float>(dt);

		Math::vec3 position = Math::vec3(transform.position.data());
		Math::vec3 cameraRotation = Math::vec3(cameraTransform.rotation.data());
		if (getKeyState(InputKeyboardKey::Up) == InputState::Held) {
			position.x += (cameraRotation.x * cubeSpeed);
			position.z += (cameraRotation.z * cubeSpeed);
		}
		if (getKeyState(InputKeyboardKey::Down) == InputState::Held) {
			position.x -= (cameraRotation.x * cubeSpeed);
			position.z -= (cameraRotation.z * cubeSpeed);
		}
		if (getKeyState(InputKeyboardKey::Left) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
			position.x -= (t.x * cubeSpeed);
			position.z -= (t.z * cubeSpeed);
		}
		if (getKeyState(InputKeyboardKey::Right) == InputState::Held) {
			Math::vec3 t = Math::normalize(Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
			position.x += (t.x * cubeSpeed);
			position.z += (t.z * cubeSpeed);
		}

		transform.position = { position.x, position.y, position.z };
	}

	void destroy() {
	}

private:
	const float m_cubeSpeed = 0.005f;

	Entity m_camera;
};