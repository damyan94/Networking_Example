#include "INetworkServer.h"

// =============================================================================
INetworkServer::INetworkServer(uint16_t port)
	: m_Acceptor(m_Context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	, m_GUIDCounter(100000)
{
}

// =============================================================================
INetworkServer::~INetworkServer()
{
	Stop();
}

// =============================================================================
bool INetworkServer::Start()
{
	try
	{
		WaitForClientConnection();

		m_ThreadContext = std::thread([this]()
			{
				m_Context.run();
			});
	}
	catch (std::exception& e)
	{
		std::cout << "[SERVER] exception: " << e.what() << "\n";
		return false;
	}

	std::cout << "[SERVER] Server started.\n";

	return true;
}

// =============================================================================
void INetworkServer::Stop()
{
	m_Context.stop();

	if (m_ThreadContext.joinable())
	{
		m_ThreadContext.join();
	}

	std::cout << "[SERVER] Server stopped.\n";
}

// =============================================================================
void INetworkServer::Update(size_t maxMessages)
{
	size_t messageCount = 0;
	while (messageCount < maxMessages && !m_MessagesIn.empty())
	{
		auto msg = m_MessagesIn.pop_front();
		OnMessage(msg.m_Remote, msg.m_Message);
		messageCount++;
	}
}

// =============================================================================
void INetworkServer::WaitForClientConnection()
{
	m_Acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
		{
			if (!ec)
			{
				std::cout << "[SERVER] New connection: " <<
					socket.remote_endpoint().address().to_string() <<
					": " << socket.remote_endpoint().port() << "\n";

				std::shared_ptr<NetworkConnection> newConnection =
					std::make_shared<NetworkConnection>(ENetworkConnectionOwner::Server,
						std::move(socket), m_Context, m_MessagesIn);

				if (OnClientConnect(newConnection))
				{
					m_Connections.push_back(std::move(newConnection));
					m_Connections.back()->ConnectToClient(this, m_GUIDCounter++);
					std::cout << "[" << m_Connections.back()->GetId() << "] Connection approved.\n";
				}
				else
				{
					std::cout << "[SERVER] New connection denied.\n";
				}
			}
			else
			{
				std::cout << "[SERVER] New connection error: " << ec.message().c_str() << "\n";
			}

			WaitForClientConnection();
		});
}

// =============================================================================
void INetworkServer::MessageClient(NetworkConnectionSharedPtr client, const NetworkMessage& msg)
{
	if (client && client->IsConnected())
	{
		client->Send(msg);
	}
	else
	{
		OnClientDisconnect(client);
		client.reset();
		m_Connections.erase(std::remove(m_Connections.begin(), m_Connections.end(), client), m_Connections.end());
	}
}

// =============================================================================
void INetworkServer::MessageAllClients(const NetworkMessage& msg, NetworkConnectionSharedPtr ignoreClient)
{
	bool invalidClientExists = false;

	for (auto& client : m_Connections)
	{
		if (client == ignoreClient)
		{
			continue;
		}

		if (client && client->IsConnected())
		{
			client->Send(msg);
		}
		else
		{
			if (client)
			{
				OnClientDisconnect(client);
				client.reset();
			}
			invalidClientExists = true;
		}
	}

	if (invalidClientExists)
	{
		m_Connections.erase(std::remove(m_Connections.begin(), m_Connections.end(), nullptr), m_Connections.end());
	}
}

// =============================================================================
void INetworkServer::OnClientValidated(NetworkConnectionSharedPtr client)
{
}