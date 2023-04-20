#pragma once

#include "NetworkCommon.h"
#include "TSQueue.h"
#include "NetworkMessage.h"
#include "NetworkConnection.h"

class INetworkServer
{
public:
	INetworkServer(uint16_t port);
	virtual ~INetworkServer();

	bool Start();
	void Stop();
	void Update(size_t maxMessages = -1);

	void WaitForClientConnection();
	void MessageClient(NetworkConnectionSharedPtr client, const NetworkMessage& msg);
	void MessageAllClients(const NetworkMessage& msg, NetworkConnectionSharedPtr ignoreClient = nullptr);

	virtual void OnClientValidated(NetworkConnectionSharedPtr client);

protected:
	virtual bool OnClientConnect(NetworkConnectionSharedPtr client) = 0;
	virtual void OnClientDisconnect(NetworkConnectionSharedPtr client) = 0;
	virtual void OnMessage(NetworkConnectionSharedPtr client, NetworkMessage& msg) = 0;

protected:
	AsioContext									m_Context;
	Thread										m_ThreadContext;
	AsioAcceptor								m_Acceptor;
	ConnectionId								m_GUIDCounter;

	TSQueue<OwnedNetworkMessage>				m_MessagesIn;
	std::deque<NetworkConnectionSharedPtr>		m_Connections;
};