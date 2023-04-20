#pragma once

#include "NetworkCommon.h"
#include "TSQueue.h"
#include "NetworkMessage.h"

class INetworkServer;

enum class ENetworkConnectionOwner
	: uint8_t
{
	Server,
	Client
};

class NetworkConnection
	: public std::enable_shared_from_this<NetworkConnection>
{
public:
	NetworkConnection(ENetworkConnectionOwner owner, AsioSocket socket, AsioContext& context, TSQueue<OwnedNetworkMessage>& messagesIn);
	virtual ~NetworkConnection();

	void				ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints);
	void				ConnectToClient(INetworkServer* server, ConnectionId id);
	void				Disconnect();
	bool				IsConnected() const;

	void				Send(const NetworkMessage& msg);
	ConnectionId		GetId() const;

	void				WriteValidation();
	void				ReadValidation(INetworkServer* server = nullptr);

protected:
	uint64_t			ScrambleData(uint64_t data);

private:
	void				ReadHeader();
	void				ReadBody();
	void				WriteHeader();
	void				WriteBody();

	void				AddToIncomingMessageQueue();

protected:
	ConnectionId					m_Id;
	ENetworkConnectionOwner			m_Owner;
	AsioContext&					m_Context;
	AsioSocket						m_Socket;
	TSQueue<NetworkMessage>			m_MessagesOut;
	TSQueue<OwnedNetworkMessage>&	m_MessagesIn;
	NetworkMessage					m_TempMessage;

	uint64_t						m_HandshakeOut;
	uint64_t						m_HandshakeIn;
	uint64_t						m_HandshakeCheck;
};