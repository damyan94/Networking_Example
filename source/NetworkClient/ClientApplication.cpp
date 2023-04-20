#include "ClientApplication.h"

#include <chrono>

// =============================================================================
void ClientApplication::SendMessagePingServer()
{
	NetworkMessage msg;
	msg.m_Header.m_Type = EMessageType::PingServer;

	auto now = std::chrono::system_clock::now();
	msg << now;

	Send(msg);
}

// =============================================================================
void ClientApplication::SendMessageMessageAll()
{
	NetworkMessage msg;
	msg.m_Header.m_Type = EMessageType::MessageAll;

	Send(msg);
}

// =============================================================================
void ClientApplication::OnMessage(NetworkMessage& msg)
{
#define MESSAGES_ITERATOR(_Type, _Msg)\
	case EMessageType::_Type:\
	OnMessage##_Type##(_Msg);\
	break

	switch (msg.m_Header.m_Type)
	{
		MESSAGES_ITERATOR(ServerAccept, msg);
		MESSAGES_ITERATOR(ServerMessage, msg);
		MESSAGES_ITERATOR(PingServer, msg);
		MESSAGES_ITERATOR(MessageAll, msg);

	default:
		break;
	}
}

// =============================================================================
void ClientApplication::OnMessageServerAccept(NetworkMessage& msg)
{
	std::cout << "Server accepted the connection.\n";
}

// =============================================================================
void ClientApplication::OnMessageServerMessage(NetworkMessage& msg)
{
	ConnectionId clientId = 0;
	msg >> clientId;
	std::cout << "Hello from" << clientId << ".\n";
}

// =============================================================================
void ClientApplication::OnMessagePingServer(NetworkMessage& msg)
{
	auto now = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point then;
	msg >> then;
	std::cout << "Ping: " << std::chrono::duration<double>(now - then).count() << "\n";
}

// =============================================================================
void ClientApplication::OnMessageMessageAll(NetworkMessage& msg)
{
	ConnectionId id = 0;
	msg >> id;

	std::cout << "Hello from " << id << "\n";
}