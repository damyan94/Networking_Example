#pragma once

#include "INetworkServer.h"

class ServerApplication
	: public INetworkServer
{
public:
	ServerApplication(uint16_t port);
	~ServerApplication();

protected:
	bool OnClientConnect(NetworkConnectionSharedPtr client) final;
	void OnClientDisconnect(NetworkConnectionSharedPtr client) final;
	void OnMessage(NetworkConnectionSharedPtr client, NetworkMessage& msg) final;

private:
	void OnMessagePingServer(NetworkConnectionSharedPtr client, NetworkMessage& msg);
	void OnMessageMessageAll(NetworkConnectionSharedPtr client, NetworkMessage& msg);
};