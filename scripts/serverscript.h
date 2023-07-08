#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include "../Core/Common/resources/ntshengn_resources_scripting.h"
#include <array>
#include <unordered_map>

struct ServerScript : NtshEngn::Script {
	struct Client {
		std::array<float, 3> position;
	};

	NTSHENGN_SCRIPT(ServerScript);

	void init() {
		m_server = networking->createServerSocket(4750, NtshEngn::NetworkType::UDP);

		m_server->setClientConnectCallback([this](NtshEngn::ConnectedClientID connectedClientID) {
			NTSHENGN_UNUSED(connectedClientID);
			for (const auto& connectedClient : m_server->getConnectedClients()) {
				NTSHENGN_UNUSED(connectedClient);
			}
		});
	}

	void update(double dt) {
		NTSHENGN_UNUSED(dt);
		for (const auto& connectedClient : m_server->getConnectedClients()) {
			NTSHENGN_UNUSED(connectedClient);
		}
	}

	void destroy() {
	}

private:
	void headerToData(uint16_t header, std::vector<uint8_t>& data) {
		data.push_back(static_cast<uint8_t>((header & 0xFF00) >> 8));
		data.push_back(static_cast<uint8_t>(header & 0x00FF));
	}

private:
	NtshEngn::ServerSocket* m_server;
	std::unordered_map<NtshEngn::ConnectedClientID, Client> m_connectedClients;

	const uint16_t messageTypeClientConnect = 0xAE75;
	const uint16_t messageTypeClientDisconnect = 0xF2E6;
	const uint16_t messageTypeClientPosition = 0xDA32;
};