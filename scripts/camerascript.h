#pragma once
#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/module_interfaces/ntshengn_window_module_interface.h"
#include "../Core/Common/module_interfaces/ntshengn_audio_module_interface.h"
#include "../Core/Common/asset_manager/ntshengn_asset_manager.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>
#include <array>

struct CameraScript : public NtshEngn::Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);

			m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
			m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
			windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);

			m_yaw = -90.0f;
		}

		if (audioModule) {
			NtshEngn::Sound* footstep = assetManager->loadSound("assets/sounds/footstep.ntsd");
			m_footstepSound = audioModule->load(*footstep);
			audioModule->setGain(m_footstepSound, 0.8f);
		}
	}

	void update(double dt) {
		NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(entityID);
		if (cameraTransform.position[1] == 1999.5f) {
			if (dt > ((1.0 / 60.0) * 1000.0)) {
				dt = ((1.0 / 60.0) * 1000.0); // OOB fix
			}
		}

		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::Escape) == NtshEngn::InputState::Pressed ||
				windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::R) == NtshEngn::InputState::Pressed) {
				m_mouseMiddleMode = !m_mouseMiddleMode;
				windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);
				if (m_mouseMiddleMode) {
					m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
					m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
					windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);
				}
			}

			NtshEngn::Math::vec3 cameraRotation = NtshEngn::Math::vec3(cameraTransform.rotation.data());

			if (m_mouseMiddleMode) {
				const int mouseX = windowModule->getCursorPositionX(windowModule->getMainWindowID());
				const int mouseY = windowModule->getCursorPositionY(windowModule->getMainWindowID());

				m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
				m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
				windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);

				const float xOffset = (mouseX - m_prevMouseX) * m_mouseSensitivity;
				const float yOffset = (mouseY - m_prevMouseY) * m_mouseSensitivity;

				m_prevMouseX = mouseX;
				m_prevMouseY = mouseY;

				m_yaw = std::fmod(m_yaw + xOffset, 360.0f);
				m_pitch = std::max(-35.0f, std::min(35.0f, m_pitch + yOffset));

				float yawRad = m_yaw * toRad;
				float pitchRad = m_pitch * toRad;

				cameraRotation.x = std::cos(pitchRad) * std::cos(yawRad);
				cameraRotation.y = -std::sin(pitchRad);
				cameraRotation.z = std::cos(pitchRad) * std::sin(yawRad);
				cameraRotation = NtshEngn::Math::normalize(cameraRotation);
			}

			const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);

			bool footstepSound = false;

			NtshEngn::Rigidbody& rigidbody = ecs->getComponent<NtshEngn::Rigidbody>(entityID);

			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				rigidbody.force[0] += (cameraRotation.x * cameraSpeed);
				rigidbody.force[2] += (cameraRotation.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				rigidbody.force[0] -= (cameraRotation.x * cameraSpeed);
				rigidbody.force[2] -= (cameraRotation.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				rigidbody.force[0] -= (t.x * cameraSpeed);
				rigidbody.force[2] -= (t.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				NtshEngn::Math::vec3 t = NtshEngn::Math::normalize(NtshEngn::Math::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				rigidbody.force[0] += (t.x * cameraSpeed);
				rigidbody.force[2] += (t.z * cameraSpeed);
				footstepSound = true;
			}

			m_footstepSoundCooldown += dt;

			if (footstepSound && (m_footstepSoundCooldown > 650.0) && audioModule && !audioModule->isPlaying(m_footstepSound)) {
				audioModule->play(m_footstepSound);
				m_footstepSoundCooldown = 0;
			}

			cameraTransform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
		}
	}

	void destroy() {
	}

private:
	std::array<NtshEngn::Math::vec3, 2> transformAABB(const NtshEngn::Math::vec3& aabbMin, const NtshEngn::Math::vec3 aabbMax, const NtshEngn::Math::mat4& transform) {
		std::array<NtshEngn::Math::vec4, 8> corners = {
						transform * NtshEngn::Math::vec4(aabbMin.x, aabbMin.y, aabbMin.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMax.x, aabbMin.y, aabbMin.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMin.x, aabbMax.y, aabbMin.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMax.x, aabbMax.y, aabbMin.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMin.x, aabbMin.y, aabbMax.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMax.x, aabbMin.y, aabbMax.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMin.x, aabbMax.y, aabbMax.z, 1.0f),
						transform * NtshEngn::Math::vec4(aabbMax.x, aabbMax.y, aabbMax.z, 1.0f)
		};

		NtshEngn::Math::vec3 min = NtshEngn::Math::vec3(std::numeric_limits<float>::max());
		NtshEngn::Math::vec3 max = NtshEngn::Math::vec3(std::numeric_limits<float>::lowest());
		for (const NtshEngn::Math::vec4& corner : corners) {
			if (corner.x < min.x) {
				min.x = corner.x;
			}
			if (corner.x > max.x) {
				max.x = corner.x;
			}

			if (corner.y < min.y) {
				min.y = corner.y;
			}
			if (corner.y > max.y) {
				max.y = corner.y;
			}

			if (corner.z < min.z) {
				min.z = corner.z;
			}
			if (corner.z > max.z) {
				max.z = corner.z;
			}
		}

		return { min, max };
	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	bool m_mouseMiddleMode = true;
	const float m_cameraSpeed = 1.0f;
	const float m_mouseSensitivity = 0.12f;
	int m_prevMouseX = 0;
	int m_prevMouseY = 0;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	NtshEngn::SoundID m_footstepSound;
	double m_footstepSoundCooldown = 0.0;
};