#pragma once

#include "INetworkClient.h"

class ClientApplication
	: public INetworkClient
{
public:
	void SendMessagePingServer();
	void SendMessageMessageAll();

protected:
	void OnMessage(NetworkMessage& msg) final;

private:
	void OnMessageServerAccept(NetworkMessage& msg);
	void OnMessageServerMessage(NetworkMessage& msg);
	void OnMessagePingServer(NetworkMessage& msg);
	void OnMessageMessageAll(NetworkMessage& msg);
};