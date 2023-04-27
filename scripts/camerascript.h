#pragma once
#include "../external/Core/external/Common/resources/ntshengn_resources_scripting.h"
#include "../external/nml/include/nml.h"
#include <cmath>
#include <array>

struct CameraScript : NtshEngn::Script {
	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {
			windowModule->setCursorVisibility(NTSHENGN_MAIN_WINDOW, !m_mouseMiddleMode);

			m_prevMouseX = windowModule->getWidth(NTSHENGN_MAIN_WINDOW) / 2;
			m_prevMouseY = windowModule->getHeight(NTSHENGN_MAIN_WINDOW) / 2;
			windowModule->setCursorPosition(NTSHENGN_MAIN_WINDOW, m_prevMouseX, m_prevMouseY);

			m_yaw = -90.0f;
		}
	}

	void update(double dt) {
		NtshEngn::Transform& cameraTransform = ecs->getComponent<NtshEngn::Transform>(entityID);
		if (cameraTransform.position[1] == 1999.5f) {
			if (dt > ((1.0 / 60.0) * 1000.0)) {
				dt = 0.0; // OOB fix
			}
		}

		if (windowModule && windowModule->isOpen(NTSHENGN_MAIN_WINDOW)) {
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::Escape) == NtshEngn::InputState::Pressed ||
				windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::R) == NtshEngn::InputState::Pressed) {
				m_mouseMiddleMode = !m_mouseMiddleMode;
				windowModule->setCursorVisibility(NTSHENGN_MAIN_WINDOW, !m_mouseMiddleMode);
				if (m_mouseMiddleMode) {
					m_prevMouseX = windowModule->getWidth(NTSHENGN_MAIN_WINDOW) / 2;
					m_prevMouseY = windowModule->getHeight(NTSHENGN_MAIN_WINDOW) / 2;
					windowModule->setCursorPosition(NTSHENGN_MAIN_WINDOW, m_prevMouseX, m_prevMouseY);
				}
			}

			nml::vec3 cameraRotation = nml::vec3(cameraTransform.rotation.data());

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
				m_pitch = std::max(-35.0f, std::min(35.0f, m_pitch + yOffset));

				float yawRad = m_yaw * toRad;
				float pitchRad = m_pitch * toRad;

				cameraRotation.x = std::cos(pitchRad) * std::cos(yawRad);
				cameraRotation.y = -std::sin(pitchRad);
				cameraRotation.z = std::cos(pitchRad) * std::sin(yawRad);
				cameraRotation = nml::normalize(cameraRotation);
			}

			const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);

			bool footstepSound = false;
			nml::vec3 cameraPosition = nml::vec3(cameraTransform.position.data());

			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::W) == NtshEngn::InputState::Held) {
				cameraPosition.x += (cameraRotation.x * cameraSpeed);
				cameraPosition.z += (cameraRotation.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::S) == NtshEngn::InputState::Held) {
				cameraPosition.x -= (cameraRotation.x * cameraSpeed);
				cameraPosition.z -= (cameraRotation.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::A) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x -= (t.x * cameraSpeed);
				cameraPosition.z -= (t.z * cameraSpeed);
				footstepSound = true;
			}
			if (windowModule->getKeyState(NTSHENGN_MAIN_WINDOW, NtshEngn::InputKeyboardKey::D) == NtshEngn::InputState::Held) {
				nml::vec3 t = nml::normalize(nml::vec3(-cameraRotation.z, 0.0, cameraRotation.x));
				cameraPosition.x += (t.x * cameraSpeed);
				cameraPosition.z += (t.z * cameraSpeed);
				footstepSound = true;
			}

			bool collide = false;
			if (physicsModule) {
				if (ecs->hasComponent<NtshEngn::SphereCollidable>(entityID)) {
					NtshEngn::ColliderSphere cameraCollider = ecs->getComponent<NtshEngn::SphereCollidable>(entityID).collider;
					cameraCollider.center[0] += cameraPosition.x;
					cameraCollider.center[1] += cameraPosition.y;
					cameraCollider.center[2] += cameraPosition.z;
					cameraCollider.radius *= std::max(cameraTransform.scale[0], std::max(cameraTransform.scale[1], cameraTransform.scale[2]));

					for (const NtshEngn::Entity entity : physicsModule->m_entities) {
						if (entity != entityID) {
							if (ecs->hasComponent<NtshEngn::AABBCollidable>(entity)) {
								const NtshEngn::Transform& otherTransform = ecs->getComponent<NtshEngn::Transform>(entity);
								const nml::vec3 otherPosition = nml::vec3(otherTransform.position[0], otherTransform.position[1], otherTransform.position[2]);
								const nml::vec3 otherRotation = nml::vec3(otherTransform.rotation[0], otherTransform.rotation[1], otherTransform.rotation[2]);
								const nml::vec3 otherScale = nml::vec3(otherTransform.scale[0], otherTransform.scale[1], otherTransform.scale[2]);

								NtshEngn::ColliderAABB otherCollider = ecs->getComponent<NtshEngn::AABBCollidable>(entity).collider;
								const nml::vec3 otherAABBMin = nml::vec3(otherCollider.min[0], otherCollider.min[1], otherCollider.min[2]);
								const nml::vec3 otherAABBMax = nml::vec3(otherCollider.max[0], otherCollider.max[1], otherCollider.max[2]);

								nml::mat4 otherModelMatrix = nml::translate(otherPosition) *
									nml::rotate(otherRotation.x, nml::vec3(1.0f, 0.0f, 0.0f)) *
									nml::rotate(otherRotation.y, nml::vec3(0.0f, 1.0f, 0.0f)) *
									nml::rotate(otherRotation.z, nml::vec3(0.0f, 0.0f, 1.0f)) *
									nml::scale(otherScale);

								std::array<nml::vec3, 2> otherAABBMinAndMax = transformAABB(otherAABBMin, otherAABBMax, otherModelMatrix);
								otherCollider.min = { otherAABBMinAndMax[0].x, otherAABBMinAndMax[0].y, otherAABBMinAndMax[0].z };
								otherCollider.max = { otherAABBMinAndMax[1].x, otherAABBMinAndMax[1].y, otherAABBMinAndMax[1].z };

								collide = physicsModule->intersect(&cameraCollider, &otherCollider);

								if (collide) {
									break;
								}
							}
						}
					}
				}
			}

			m_footstepSoundCooldown += dt;
			if (!collide) {
				cameraTransform.position = { cameraPosition.x, cameraPosition.y, cameraPosition.z };

				if (footstepSound && (m_footstepSoundCooldown > 650.0) && audioModule && !audioModule->isPlaying(m_footstepSound)) {
					audioModule->play(m_footstepSound);
					m_footstepSoundCooldown = 0;
				}
			}

			cameraTransform.rotation = { cameraRotation.x, cameraRotation.y, cameraRotation.z };
		}
	}

	void destroy() {
	}

private:
	std::array<nml::vec3, 2> transformAABB(const nml::vec3& aabbMin, const nml::vec3 aabbMax, const nml::mat4& transform) {
		std::array<nml::vec4, 8> corners = {
						transform * nml::vec4(aabbMin.x, aabbMin.y, aabbMin.z, 1.0f),
						transform * nml::vec4(aabbMax.x, aabbMin.y, aabbMin.z, 1.0f),
						transform * nml::vec4(aabbMin.x, aabbMax.y, aabbMin.z, 1.0f),
						transform * nml::vec4(aabbMax.x, aabbMax.y, aabbMin.z, 1.0f),
						transform * nml::vec4(aabbMin.x, aabbMin.y, aabbMax.z, 1.0f),
						transform * nml::vec4(aabbMax.x, aabbMin.y, aabbMax.z, 1.0f),
						transform * nml::vec4(aabbMin.x, aabbMax.y, aabbMax.z, 1.0f),
						transform * nml::vec4(aabbMax.x, aabbMax.y, aabbMax.z, 1.0f)
		};

		nml::vec3 min = nml::vec3(std::numeric_limits<float>::max());
		nml::vec3 max = nml::vec3(std::numeric_limits<float>::lowest());
		for (const nml::vec4& corner : corners) {
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
	const float m_cameraSpeed = 0.0015f;
	const float m_mouseSensitivity = 0.12f;
	int m_prevMouseX = 0;
	int m_prevMouseY = 0;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	NtshEngn::SoundId m_footstepSound = 0;
	double m_footstepSoundCooldown = 0.0;
};