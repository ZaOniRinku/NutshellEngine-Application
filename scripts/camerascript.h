#pragma once
#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/module_interfaces/ntshengn_graphics_module_interface.h"
#include "../Core/Common/module_interfaces/ntshengn_physics_module_interface.h"
#include "../Core/Common/module_interfaces/ntshengn_window_module_interface.h"
#include "../Core/Common/asset_manager/ntshengn_asset_manager.h"
#include "../Core/frame_limiter/ntshengn_frame_limiter.h"
#include "../Core/Common/utils/ntshengn_utils_math.h"
#include <cmath>

using namespace NtshEngn;
struct CameraScript : public Script {

	NTSHENGN_SCRIPT(CameraScript);

	void init() {
		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);

			m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
			m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
			windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);

			const Transform transform = ecs->getComponent<Transform>(entityID);
			Math::vec3 cameraRotation = Math::vec3(transform.rotation[0], transform.rotation[1], transform.rotation[2]);

			m_yaw = std::atan2(cameraRotation.z, cameraRotation.x) * toDeg;
			m_pitch = -std::asin(cameraRotation.y) * toDeg;
		}

		Font* font = assetManager->loadFont("assets/fonts/JetBrainsMono-Regular.ttf", 24.0f);
		m_fontID = graphicsModule->load(*font);
	}

	void update(double dt) {
		const float windowWidth = static_cast<float>(windowModule->getWidth(windowModule->getMainWindowID()));
		const float windowHeight = static_cast<float>(windowModule->getHeight(windowModule->getMainWindowID()));
		graphicsModule->drawUIText(m_fontID, "+", Math::vec2(windowWidth / 2.0f, windowHeight / 2.0f), Math::vec4(1.0f, 0.0f, 0.0f, 0.25f));

		if (windowModule && windowModule->isOpen(windowModule->getMainWindowID())) {
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::R) == InputState::Pressed) {
				m_mouseMiddleMode = !m_mouseMiddleMode;
				windowModule->setCursorVisibility(windowModule->getMainWindowID(), !m_mouseMiddleMode);
				if (m_mouseMiddleMode) {
					m_prevMouseX = windowModule->getWidth(windowModule->getMainWindowID()) / 2;
					m_prevMouseY = windowModule->getHeight(windowModule->getMainWindowID()) / 2;
					windowModule->setCursorPosition(windowModule->getMainWindowID(), m_prevMouseX, m_prevMouseY);
				}
			}

			Transform& transform = ecs->getComponent<Transform>(entityID);

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
				m_pitch = std::max(-89.0f, std::min(89.0f, m_pitch + yOffset));

				float yawRad = m_yaw * toRad;
				float pitchRad = m_pitch * toRad;

				transform.rotation.x = std::cos(pitchRad) * std::cos(yawRad);
				transform.rotation.y = -std::sin(pitchRad);
				transform.rotation.z = std::cos(pitchRad) * std::sin(yawRad);
				transform.rotation = Math::normalize(transform.rotation);
			}

			const float cameraSpeed = m_cameraSpeed * static_cast<float>(dt);
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::W) == InputState::Held) {
				transform.position += (transform.rotation * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::S) == InputState::Held) {
				transform.position -= (transform.rotation * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::A) == InputState::Held) {
				Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
				transform.position.x -= (t.x * cameraSpeed);
				transform.position.z -= (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::D) == InputState::Held) {
				Math::vec3 t = Math::normalize(Math::vec3(-transform.rotation.z, 0.0, transform.rotation.x));
				transform.position.x += (t.x * cameraSpeed);
				transform.position.z += (t.z * cameraSpeed);
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::Space) == InputState::Held) {
				transform.position.y += cameraSpeed;
			}
			if (windowModule->getKeyState(windowModule->getMainWindowID(), InputKeyboardKey::Shift) == InputState::Held) {
				transform.position.y -= cameraSpeed;
			}

			if (windowModule->getMouseButtonState(windowModule->getMainWindowID(), NtshEngn::InputMouseButton::One) == NtshEngn::InputState::Pressed) {
				std::vector<RaycastInformation> raycastInformations = physicsModule->raycast(transform.position, transform.rotation, 0.0f, 10000.0f);
				if (!raycastInformations.empty()) {
					const RaycastInformation& raycastInformation = raycastInformations.front();

					m_hitEntity = raycastInformation.entity;

					const Renderable& renderable = ecs->getComponent<Renderable>(m_hitEntity);

					if (renderable.material->diffuseTexture.image) {
						m_entityDiffuse = graphicsModule->load(*renderable.material->diffuseTexture.image);
						m_entityDiffuseFilter = renderable.material->diffuseTexture.imageSampler.magFilter;
						m_entityDiffuseSize = std::max(static_cast<float>(renderable.material->diffuseTexture.image->width), static_cast<float>(renderable.material->diffuseTexture.image->height));
					}
					else {
						m_entityDiffuse = std::numeric_limits<ImageID>::max();
					}

					if (renderable.material->normalTexture.image) {
						m_entityNormal = graphicsModule->load(*renderable.material->normalTexture.image);
						m_entityNormalFilter = renderable.material->normalTexture.imageSampler.magFilter;
						m_entityNormalSize = std::max(static_cast<float>(renderable.material->normalTexture.image->width), static_cast<float>(renderable.material->normalTexture.image->height));
					}
					else {
						m_entityNormal = std::numeric_limits<ImageID>::max();
					}

					if (renderable.material->metalnessTexture.image) {
						m_entityMetalness = graphicsModule->load(*renderable.material->metalnessTexture.image);
						m_entityMetalnessFilter = renderable.material->metalnessTexture.imageSampler.magFilter;
						m_entityMetalnessSize = std::max(static_cast<float>(renderable.material->metalnessTexture.image->width), static_cast<float>(renderable.material->metalnessTexture.image->height));
					}
					else {
						m_entityMetalness = std::numeric_limits<ImageID>::max();
					}

					if (renderable.material->roughnessTexture.image) {
						m_entityRoughness = graphicsModule->load(*renderable.material->roughnessTexture.image);
						m_entityRoughnessFilter = renderable.material->roughnessTexture.imageSampler.magFilter;
						m_entityRoughnessSize = std::max(static_cast<float>(renderable.material->roughnessTexture.image->width), static_cast<float>(renderable.material->roughnessTexture.image->height));
					}
					else {
						m_entityRoughness = std::numeric_limits<ImageID>::max();
					}

					if (renderable.material->emissiveTexture.image) {
						m_entityEmissive = graphicsModule->load(*renderable.material->emissiveTexture.image);
						m_entityEmissiveFilter = renderable.material->emissiveTexture.imageSampler.magFilter;
						m_entityEmissiveSize = std::max(static_cast<float>(renderable.material->emissiveTexture.image->width), static_cast<float>(renderable.material->emissiveTexture.image->height));
					}
					else {
						m_entityEmissive = std::numeric_limits<ImageID>::max();
					}

					if (renderable.material->occlusionTexture.image) {
						m_entityOcclusion = graphicsModule->load(*renderable.material->occlusionTexture.image);
						m_entityOcclusionFilter = renderable.material->occlusionTexture.imageSampler.magFilter;
						m_entityOcclusionSize = std::max(static_cast<float>(renderable.material->occlusionTexture.image->width), static_cast<float>(renderable.material->occlusionTexture.image->height));
					}
					else {
						m_entityOcclusion = std::numeric_limits<ImageID>::max();
					}

					m_entityEmissiveFactor = renderable.material->emissiveFactor;
					m_entityIOR = renderable.material->indexOfRefraction;
				}
				else {
					m_hitEntity = std::numeric_limits<Entity>::max();
				}
			}
		}

		if (m_hitEntity != std::numeric_limits<Entity>::max()) {
			graphicsModule->drawUIRectangle(Math::vec2(45.0f, windowHeight - 200.0f - 25.0f), Math::vec2(windowWidth - 45.0f - 45.0f, 200.0f), Math::vec4(0.0f, 0.0f, 0.0f, 0.5f));

			graphicsModule->drawUIText(m_fontID, "Entity: " + std::to_string(m_hitEntity), Math::vec2(50.0f, windowHeight - 200.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));

			graphicsModule->drawUIText(m_fontID, "Diffuse", Math::vec2(50.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityDiffuse != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityDiffuse, m_entityDiffuseFilter, Math::vec2(50.0f + 64.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityDiffuseSize, 128.0f / m_entityDiffuseSize), Math::vec4(1.0f));
			}

			graphicsModule->drawUIText(m_fontID, "Normal", Math::vec2(50.0f + 128.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityNormal != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityNormal, m_entityNormalFilter, Math::vec2(50.0f + 192.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityNormalSize, 128.0f / m_entityNormalSize), Math::vec4(1.0f));
			}

			graphicsModule->drawUIText(m_fontID, "Metalness", Math::vec2(50.0f + 256.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityMetalness != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityMetalness, m_entityMetalnessFilter, Math::vec2(50.0f + 320.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityMetalnessSize, 128.0f / m_entityMetalnessSize), Math::vec4(1.0f));
			}

			graphicsModule->drawUIText(m_fontID, "Roughness", Math::vec2(50.0f + 384.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityRoughness != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityRoughness, m_entityRoughnessFilter, Math::vec2(50.0f + 448.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityRoughnessSize, 128.0f / m_entityRoughnessSize), Math::vec4(1.0f));
			}

			graphicsModule->drawUIText(m_fontID, "Emissive", Math::vec2(50.0f + 512.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityEmissive != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityEmissive, m_entityEmissiveFilter, Math::vec2(50.0f + 576.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityEmissiveSize, 128.0f / m_entityEmissiveSize), Math::vec4(1.0f));
			}

			graphicsModule->drawUIText(m_fontID, "Occlusion", Math::vec2(50.0f + 640.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (m_entityOcclusion != std::numeric_limits<ImageID>::max()) {
				graphicsModule->drawUIImage(m_entityOcclusion, m_entityOcclusionFilter, Math::vec2(50.0f + 704.0f, windowHeight - 200.0f + 104.0f), 0.0f, Math::vec2(128.0f / m_entityOcclusionSize, 128.0f / m_entityOcclusionSize), Math::vec4(1.0f));
			}

			/*const Renderable& renderable = ecs->getComponent<Renderable>(m_hitEntity);
			for (size_t i = 0; i < renderable.mesh->indices.size(); i += 3) {
				graphicsModule->drawUILine(Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i]].position), Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i + 1]].position), Math::vec4(1.0f));
				graphicsModule->drawUILine(Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i + 1]].position), Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i + 2]].position), Math::vec4(1.0f));
				graphicsModule->drawUILine(Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i + 2]].position), Math::vec2(renderable.mesh->vertices[renderable.mesh->indices[i]].position), Math::vec4(1.0f));
			}*/

			graphicsModule->drawUIText(m_fontID, "Emissive factor: " + std::to_string(m_entityEmissiveFactor), Math::vec2(50.0f + 768.0f + 50.0f, windowHeight - 200.0f + 30.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			graphicsModule->drawUIText(m_fontID, "IOR: " + std::to_string(m_entityIOR), Math::vec2(50.0f + 768.0f + 50.0f, windowHeight - 200.0f + 50.0f), Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void destroy() {
	}

private:
	const float toRad = 3.1415926535897932384626433832795f / 180.0f;
	const float toDeg = 180.0f / 3.1415926535897932384626433832795f;

	bool m_mouseMiddleMode = false;

	const float m_cameraSpeed = 0.0015f;
	const float m_mouseSensitivity = 0.12f;

	int m_prevMouseX = 0;
	int m_prevMouseY = 0;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	Entity m_hitEntity = std::numeric_limits<Entity>::max();

	ImageID m_entityDiffuse;
	ImageSamplerFilter m_entityDiffuseFilter;
	float m_entityDiffuseSize;

	ImageID m_entityNormal;
	ImageSamplerFilter m_entityNormalFilter;
	float m_entityNormalSize;

	ImageID m_entityMetalness;
	ImageSamplerFilter m_entityMetalnessFilter;
	float m_entityMetalnessSize;

	ImageID m_entityRoughness;
	ImageSamplerFilter m_entityRoughnessFilter;
	float m_entityRoughnessSize;

	ImageID m_entityEmissive;
	ImageSamplerFilter m_entityEmissiveFilter;
	float m_entityEmissiveSize;

	ImageID m_entityOcclusion;
	ImageSamplerFilter m_entityOcclusionFilter;
	float m_entityOcclusionSize;

	float m_entityEmissiveFactor;
	float m_entityIOR;

	FontID m_fontID;
};