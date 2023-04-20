#include "ServerApplication.h"

// =============================================================================
ServerApplication::ServerApplication(uint16_t port)
	: INetworkServer(port)
{
}

// =============================================================================
ServerApplication::~ServerApplication()
{
}

// =============================================================================
bool ServerApplication::OnClientConnect(NetworkConnectionSharedPtr client)
{
	NetworkMessage msg;
	msg.m_Header.m_Type = EMessageType::ServerAccept;
	client->Send(msg);

	return true;
}

// =============================================================================
void ServerApplication::OnClientDisconnect(NetworkConnectionSharedPtr client)
{
	std::cout << "Removing client [" << client->GetId() << "].\n";
}

// =============================================================================
void ServerApplication::OnMessage(NetworkConnectionSharedPtr client, NetworkMessage& msg)
{
#define MESSAGES_ITERATOR(_Type, _Client, _Msg)\
	case EMessageType::_Type:\
	OnMessage##_Type##(_Client, _Msg);\
	break

	switch (msg.m_Header.m_Type)
	{
		MESSAGES_ITERATOR(PingServer, client, msg);
		MESSAGES_ITERATOR(MessageAll, client, msg);

	default:
		break;
	}
}

// =============================================================================
void ServerApplication::OnMessagePingServer(NetworkConnectionSharedPtr client, NetworkMessage& msg)
{
	std::cout << "[" << client->GetId() << "]: Server ping.\n";
	MessageClient(client, msg);
}

// =============================================================================
void ServerApplication::OnMessageMessageAll(NetworkConnectionSharedPtr client, NetworkMessage& msg)
{
	NetworkMessage newMessage;
	std::cout << "[" << client->GetId() << "]: Message all.\n";
	newMessage.m_Header.m_Type = EMessageType::MessageAll;
	newMessage << client->GetId();

	MessageAllClients(newMessage, client);
}