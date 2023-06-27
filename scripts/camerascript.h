#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>
#include <random>

struct CameraScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {
			windowModule->setCursorVisibility(NTSHENGN_MAIN_WINDOW, !m_mouseMiddleMode);

			m_prevMouseX = windowModule->getWidth(NTSHENGN_MAIN_WINDOW) / 2;
			m_prevMouseY = windowModule->getHeight(NTSHENGN_MAIN_WINDOW) / 2;
			windowModule->setCursorPosition(NTSHENGN_MAIN_WINDOW, m_prevMouseX, m_prevMouseY);

			const NtshEngn::Transform transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			nml::vec3 cameraRotation = nml::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

			m_yaw = std::atan2(cameraRotation.z, cameraRotation.x) * toDeg;
			m_pitch = -std::asin(cameraRotation.y) * toDeg;
		}

		rigidbody.restitution = 1.1f;

		sphereCollidable.collider.center = { 0.0f, 0.0f, 0.0f };
		sphereCollidable.collider.radius = 1.0f;

		aabbCollidable.collider.min = { -0.5f, -0.5f, -0.5f };
		aabbCollidable.collider.max = { -0.5f, -0.5f, -0.5f };

		capsuleCollidable.collider.base = { 0.0f, 0.0f, 0.0f };
		capsuleCollidable.collider.tip = { 0.0f, 1.0f, 0.0f };
	}

	void update(double dt) {
		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::R) == NtshEngn::InputState::Pressed) {
				m_mouseMiddleMode = !m_mouseMiddleMode;
				windowModule->setCursorVisibility(NTSHENGN_MAIN_WINDOW, !m_mouseMiddleMode);
				if (m_mouseMiddleMode) {
					m_prevMouseX = windowModule->getWidth(NTSHENGN_MAIN_WINDOW) / 2;
					m_prevMouseY = windowModule->getHeight(NTSHENGN_MAIN_WINDOW) / 2;
					windowModule->setCursorPosition(NTSHENGN_MAIN_WINDOW, m_prevMouseX, m_prevMouseY);
				}
			}

			NtshEngn::Transform& transform = ecs->getComponent<NtshEngn::Transform>(entityID);
			nml::vec3 cameraRotation = nml::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

			if (m_mouseMiddleMode) {
				const int mouseX = windowModule->getCursorPositionX(NTSHENGN_MAIN_WINDOW);
				const int mouseY = windowModule->getCursorPositionY(NTSHENGN_MAIN_WINDOW);

				m_prevMouseX = windowModule->getWidth(NTSHENGN_MAIN_WINDOW) / 2;
				m_prevMouseY = windowModule->getHeight(NTSHENGN_MAIN_WINDOW) / 2;
				windowModule->setCursorPosition(NTSHENGN_MAIN_WINDOW, m_prevMouseX, m_prevMouseY);

				const float xOffset = (mouseX - m_prevMouseX) * m_mouseSensitivity;
				const float yOffset = (mouseY - m_prevMouseY) * m_mouseSensitivity;

				m_prevMouseX = mouseX;
				m_prevMouseY = mouseY;

				m_yaw = std::fmod(m_yaw + xOffset, 360.0f);
				m_pitch = std::max(-89.0f, std::min(89.0f, m_pitch + yOffset));

				float yawRad = m_yaw * toRad;
				float pitchRad = m_pitch * toRad;

				cameraRotation.x = std::cos(pitchRad) * std::cos(yawRad);
				cameraRotation.y = -std::sin(pitchRad);
				cameraRotation.z = std::cos(pitchRad) * std::sin(yawRad);
				cameraRotation = nml::normalize(cameraRotation);
			}

			const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);

			nml::vec3 cameraPosition = nml::vec3(transform.position[0], transform.position[1], transform.position[2]);
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				cameraPosition += (cameraRotation * cameraSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				cameraPosition -= (cameraRotation * cameraSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x -= (t.x * cameraSpeed);
				cameraPosition.z -= (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x += (t.x * cameraSpeed);
				cameraPosition.z += (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Space) == NtshEngn::InputState::Held) {
				cameraPosition.y += cameraSpeed;
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Shift) == NtshEngn::InputState::Held) {
				cameraPosition.y -= cameraSpeed;
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::B) == NtshEngn::InputState::Pressed) {
				windowModule->setBorderless(NTSHENGN_MAIN_WINDOW, !windowModule->isBorderless(NTSHENGN_MAIN_WINDOW));
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::V) == NtshEngn::InputState::Pressed) {
				windowModule->setResizable(NTSHENGN_MAIN_WINDOW, !windowModule->isResizable(NTSHENGN_MAIN_WINDOW));
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::NumPlus) == NtshEngn::InputState::Pressed) {
				frameLimiter->setMaxFPS(frameLimiter->getMaxFPS() + 15);
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::NumMinus) == NtshEngn::InputState::Pressed) {
				frameLimiter->setMaxFPS(frameLimiter->getMaxFPS() - 15);
			}

			transform.position = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
			transform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };

			frameCounter++;

			timeAcc += dt;
			if (timeAcc > 1000.0) {
				windowModule->setTitle(NTSHENGN_MAIN_WINDOW, std::to_string(frameCounter) + " - Max: " + std::to_string(frameLimiter->getMaxFPS()));
				timeAcc = 0.0;
				frameCounter = 0;
				
				NtshEngn::Entity newEntity = ecs->createEntity();
				
				NtshEngn::Transform& newEntityTransform = ecs->getComponent<NtshEngn::Transform>(newEntity);
				newEntityTransform.position[1] = 10.0f;

				ecs->addComponent(newEntity, rigidbody);

				std::random_device rd;

				std::mt19937 gen(rd());
				std::uniform_int_distribution<> disInt(0, 2);
				std::uniform_real_distribution<> disNorm(0.0, 1.0);
				std::uniform_real_distribution<> dis(-1.0, 1.0);

				int shape = disInt(gen);
				if (shape == 0) {
					sphereCollidable.collider.radius = static_cast<float>(disNorm(gen)) * 5.0f + 1.0f;
					ecs->addComponent(newEntity, sphereCollidable);
				}
				else if (shape == 1) {
					aabbCollidable.collider.min = { -(static_cast<float>(disNorm(gen)) * 2.0f + 0.25f), -(static_cast<float>(disNorm(gen)) * 2.0f + 0.25f), -(static_cast<float>(disNorm(gen)) * 2.0f + 0.25f) };
					aabbCollidable.collider.max = { static_cast<float>(disNorm(gen)) * 2.0f + 0.25f, static_cast<float>(disNorm(gen)) * 2.0f + 0.25f, static_cast<float>(disNorm(gen)) * 2.0f + 0.25f };
					ecs->addComponent(newEntity, aabbCollidable);
				}
				else if (shape == 2) {
					capsuleCollidable.collider.tip = { 0.0f, static_cast<float>(disNorm(gen)) * 5.0f + 0.25f, 0.0f };
					capsuleCollidable.collider.radius = static_cast<float>(disNorm(gen)) * 3.0f + 1.0f;
					ecs->addComponent(newEntity, capsuleCollidable);
				}
				
				NtshEngn::Rigidbody& newEntityRigidbody = ecs->getComponent<NtshEngn::Rigidbody>(newEntity);
				newEntityRigidbody.mass = static_cast<float>(disNorm(gen)) * 1.0f + 0.05f;
				newEntityRigidbody.force = { static_cast<float>(dis(gen)) * 150.0f, static_cast<float>(disNorm(gen)) * 150.0f, static_cast<float>(dis(gen)) * 150.0f };
			}
		}
	}

	void destroy() {
	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;

	bool m_mouseMiddleMode = false;

	const float m_cameraSpeed = 0.015f;
	const float m_mouseSensitivity = 0.12f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	uint32_t frameCounter = 0;
	double timeAcc = 0.0;

	NtshEngn::Rigidbody rigidbody;
	NtshEngn::SphereCollidable sphereCollidable;
	NtshEngn::AABBCollidable aabbCollidable;
	NtshEngn::CapsuleCollidable capsuleCollidable;
};