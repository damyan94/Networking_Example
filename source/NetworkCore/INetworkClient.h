#pragma once

#include "NetworkCommon.h"
#include "TSQueue.h"
#include "NetworkMessage.h"
#include "NetworkConnection.h"

class INetworkClient
{
public:
	INetworkClient();
	virtual ~INetworkClient();

	bool Connect(const std::string& host, const uint16_t port);
	void Disconnect();
	bool IsConnected() const;
	void Update(size_t maxMessages = -1);

	void Send(const NetworkMessage& msg);

	TSQueue<OwnedNetworkMessage>& GetMessagesIn();

protected:
	virtual void OnMessage(NetworkMessage& msg) = 0;

protected:
	AsioContext							m_Context;
	Thread								m_ThreadContext;
	NetworkConnectionUniquePtr			m_Connection;

private:
	TSQueue<OwnedNetworkMessage>		m_MessagesIn;
};