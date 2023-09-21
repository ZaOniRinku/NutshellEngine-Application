#include "../Core/scripting/ntshengn_script.h"
#include "../Core/Common/utils/ntshengn_utils_buffer.h"
#include <unordered_map>

using namespace NtshEngn;
struct ServerScript : Script {
	struct Client {
		Math::vec3 position;
	};

	NTSHENGN_SCRIPT(ServerScript);

	void init() {
		m_server = networking->createServerSocket(4750, NetworkType::TCP);

		m_server->setClientConnectCallback([this](ConnectedClientID connectedClientID) {
			Buffer buffer;

			// Send new client its ID
			buffer.write(reinterpret_cast<const std::byte*>(&messageTypeClientID), sizeof(uint16_t));
			buffer.write(reinterpret_cast<const std::byte*>(&connectedClientID), sizeof(ConnectedClientID));
			m_server->sendDataToClient(connectedClientID, buffer.getData(), buffer.getSize());

			// Tell all players a new player connected
			buffer.setCursorPosition(0);
			buffer.write(reinterpret_cast<const std::byte*>(&messageTypeClientConnect), sizeof(uint16_t));
			buffer.write(reinterpret_cast<const std::byte*>(&connectedClientID), sizeof(ConnectedClientID));
			for (const auto& connectedClient : m_server->getConnectedClients()) {
				m_server->sendDataToClient(connectedClient.first, buffer.getData(), buffer.getSize());
			}

			Client newClient;
			newClient.position = { 0.0f, 0.0f, 0.0f };
			m_connectedClients[connectedClientID] = newClient;
		});

		m_server->setClientDisconnectCallback([this](ConnectedClientID connectedClientID) {
			Buffer buffer;
			buffer.write(reinterpret_cast<const std::byte*>(&messageTypeClientDisconnect), sizeof(uint16_t));
			buffer.write(reinterpret_cast<const std::byte*>(&connectedClientID), sizeof(connectedClientID));
			for (const auto& connectedClient : m_server->getConnectedClients()) {
				m_server->sendDataToClient(connectedClient.first, buffer.getData(), buffer.getSize());
			}
			
			m_connectedClients.erase(connectedClientID);
		});

		m_server->setDataReceivedCallback([this](ConnectedClientID connectedClientID, void* data, size_t size) {
			Buffer buffer(static_cast<std::byte*>(data), size);

			uint16_t messageType;
			buffer.read(reinterpret_cast<std::byte*>(&messageType), sizeof(uint16_t));

			if (messageType == messageTypeClientPosition) {
				Math::vec3 entityPosition;
				buffer.read(reinterpret_cast<std::byte*>(entityPosition.data()), 3 * sizeof(float));

				m_connectedClients[connectedClientID].position = entityPosition;
			}
			else {
				std::cout << "Unknown header: " + std::to_string(messageType) << std::endl;
			}
		});
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);
		Buffer buffer;
		for (auto& connectedClient : m_connectedClients) {
			buffer.write(reinterpret_cast<const std::byte*>(&messageTypeClientPosition), sizeof(uint16_t));
			buffer.write(reinterpret_cast<const std::byte*>(&connectedClient.first), sizeof(ConnectedClientID));
			buffer.write(reinterpret_cast<const std::byte*>(connectedClient.second.position.data()), 3 * sizeof(float));
		}

		for (const auto& connectedClient : m_connectedClients) {
			m_server->sendDataToClient(connectedClient.first, buffer.getData(), buffer.getSize());
		}
	}

	void destroy() {
	}

private:
	ServerSocket* m_server;
	std::unordered_map<ConnectedClientID, Client> m_connectedClients;

	const uint16_t messageTypeClientConnect = 0xAE75;
	const uint16_t messageTypeClientDisconnect = 0xF2E6;
	const uint16_t messageTypeClientPosition = 0xDA32;
	const uint16_t messageTypeClientID = 0xCD83;
};