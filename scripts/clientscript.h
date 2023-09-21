#include "../Core/scripting/ntshengn_script.h"
#include "../Core/Common/utils/ntshengn_utils_buffer.h"

using namespace NtshEngn;
struct ClientScript : Script {
	NTSHENGN_SCRIPT(ClientScript);

	void init() {
		m_cubeMesh = assetManager->createModel();
		m_cubeMesh->primitives.resize(1);
		m_cubeMesh->primitives[0].mesh.vertices = {
			{ {0.25f, 0.25f, -0.25f}, {0.0f, 0.25f, 0.0f}, {1.0f, 0.0f} },
			{ {-0.25f, 0.25f, -0.25f}, {0.0f, 0.25f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.25f, 0.25f, 0.25f}, {0.0f, 0.25f, 0.0f}, {0.0f, 1.0f} },
			{ {0.25f, 0.25f, 0.25f}, {0.0f, 0.25f, 0.0f}, {0.0f, 0.0f} },
			{ {0.25f, -0.25f, 0.25f}, {0.0f, 0.0f, 0.25f}, {1.0f, 0.0f} },
			{ {0.25f, 0.25f, 0.25f}, {0.0f, 0.0f, 0.25f}, {1.0f, 1.0f} },
			{ {-0.25f, 0.25f, 0.25f}, {0.0f, 0.0f, 0.25f}, {0.0f, 1.0f} },
			{ {-0.25f, -0.25f, 0.25f}, {0.0f, 0.0f, 0.25f}, {0.0f, 0.0f} },
			{ {-0.25f, -0.25f, 0.25f}, {-0.25f, 0.0f, 0.0f}, {1.0f, 0.0f} },
			{ {-0.25f, 0.25f, 0.25f}, {-0.25f, 0.0f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.25f, 0.25f, -0.25f}, {-0.25f, 0.0f, 0.0f}, {0.0f, 1.0f} },
			{ {-0.25f, -0.25f, -0.25f}, {-0.25f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {-0.25f, -0.25f, -0.25f}, {0.0f, -0.25f, 0.0f}, {1.0f, 0.0f} },
			{ {0.25f, -0.25f, -0.25f}, {0.0f, -0.25f, 0.0f}, {1.0f, 1.0f} },
			{ {0.25f, -0.25f, 0.25f}, {0.0f, -0.25f, 0.0f}, {0.0f, 1.0f} },
			{ {-0.25f, -0.25f, 0.25f}, {0.0f, -0.25f, 0.0f}, {0.0f, 0.0f} },
			{ {0.25f, -0.25f, -0.25f}, {0.25f, 0.0f, 0.0f}, {1.0f, 0.0f} },
			{ {0.25f, 0.25f, -0.25f}, {0.25f, 0.0f, 0.0f}, {1.0f, 1.0f} },
			{ {0.25f, 0.25f, 0.25f}, {0.25f, 0.0f, 0.0f}, {0.0f, 1.0f} },
			{ {0.25f, -0.25f, 0.25f}, {0.25f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {-0.25f, -0.25f, -0.25f}, {0.0f, 0.0f, -0.25f}, {1.0f, 0.0f} },
			{ {-0.25f, 0.25f, -0.25f}, {0.0f, 0.0f, -0.25f}, {1.0f, 1.0f} },
			{ {0.25f, 0.25f, -0.25f}, {0.0f, 0.0f, -0.25f}, {0.0f, 1.0f} },
			{ {0.25f, -0.25f, -0.25f}, {0.0f, 0.0f, -0.25f}, {0.0f, 0.0f} }
		};
		m_cubeMesh->primitives[0].mesh.indices = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
		};

		m_cubeTexture = assetManager->createImage();
		m_cubeTexture->width = 1;
		m_cubeTexture->height = 1;
		m_cubeTexture->format = ImageFormat::R8G8B8A8;
		m_cubeTexture->colorSpace = ImageColorSpace::SRGB;
		m_cubeTexture->data = { 255, 0, 0, 255 };

		m_cubeMesh->primitives[0].material.diffuseTexture.image = m_cubeTexture;
		m_cubeMesh->primitives[0].material.diffuseTexture.imageSampler.magFilter = ImageSamplerFilter::Nearest;
		m_cubeMesh->primitives[0].material.diffuseTexture.imageSampler.minFilter = ImageSamplerFilter::Nearest;
		m_cubeMesh->primitives[0].material.diffuseTexture.imageSampler.mipmapFilter = ImageSamplerFilter::Nearest;
		m_cubeMesh->primitives[0].material.diffuseTexture.imageSampler.anisotropyLevel = 8.0f;

		m_newRenderable.model = m_cubeMesh;
		m_newRenderable.modelPrimitiveIndex = 0;

		std::string serverIPAddress;
		std::cout << "Please enter the server\'s IP address: ";
		std::cin >> serverIPAddress;

		m_client = networking->createClientSocket(NetworkType::TCP);
		m_client->connectToServer(serverIPAddress, 4750);

		m_client->setDataReceivedCallback([this](void* data, size_t size) {
			Buffer buffer(static_cast<std::byte*>(data), size);

			uint16_t messageType;
			while (buffer.read(reinterpret_cast<std::byte*>(&messageType), sizeof(uint16_t)) != 0) {
				if (messageType == messageTypeClientConnect) {
					ConnectedClientID clientID;
					buffer.read(reinterpret_cast<std::byte*>(&clientID), sizeof(ConnectedClientID));

					if (clientID != m_id) {
						Entity newEntity = ecs->createEntity();
						ecs->addComponent(newEntity, m_newRenderable);

						m_connectedClients[clientID] = newEntity;

						std::cout << "Client connected: " + std::to_string(clientID) << std::endl;
					}
				}
				else if (messageType == messageTypeClientDisconnect) {
					ConnectedClientID clientID;
					buffer.read(reinterpret_cast<std::byte*>(&clientID), sizeof(ConnectedClientID));

					ecs->destroyEntity(m_connectedClients[clientID]);

					m_connectedClients.erase(clientID);

					std::cout << "Client disconnected: " + std::to_string(clientID) << std::endl;
				}
				else if (messageType == messageTypeClientPosition) {
					ConnectedClientID clientID;
					buffer.read(reinterpret_cast<std::byte*>(&clientID), sizeof(ConnectedClientID));

					Math::vec3 entityPosition;
					buffer.read(reinterpret_cast<std::byte*>(entityPosition.data()), 3 * sizeof(float));
					if (clientID != m_id) {
						if (m_connectedClients.find(clientID) == m_connectedClients.end()) {
							Entity newEntity = ecs->createEntity();
							ecs->addComponent(newEntity, m_newRenderable);

							m_connectedClients[clientID] = newEntity;
						}

						Transform& entityTransform = ecs->getComponent<Transform>(m_connectedClients[clientID]);
						entityTransform.position = entityPosition;
					}
				}
				else if (messageType == messageTypeClientID) {
					ConnectedClientID clientID;
					buffer.read(reinterpret_cast<std::byte*>(&clientID), sizeof(ConnectedClientID));

					m_id = clientID;
					std::cout << "I am number " + std::to_string(m_id) << std::endl;
				}
				else {
					std::cout << "Unknown header: " + std::to_string(messageType) << std::endl;
				}
			}
		});
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);
		Buffer buffer;
		buffer.write(reinterpret_cast<const std::byte*>(&messageTypeClientPosition), sizeof(uint16_t));

		Transform& entityTransform = ecs->getComponent<Transform>(0);
		buffer.write(reinterpret_cast<const std::byte*>(entityTransform.position.data()), 3 * sizeof(float));

		m_client->sendDataToServer(buffer.getData(), buffer.getSize());
	}

	void destroy() {
	}

private:
	ClientSocket* m_client;

	const uint16_t messageTypeClientConnect = 0xAE75;
	const uint16_t messageTypeClientDisconnect = 0xF2E6;
	const uint16_t messageTypeClientPosition = 0xDA32;
	const uint16_t messageTypeClientID = 0xCD83;

	std::unordered_map<ConnectedClientID, Entity> m_connectedClients;

	ConnectedClientID m_id;

	Model* m_cubeMesh;
	Image* m_cubeTexture;
	Renderable m_newRenderable;
};