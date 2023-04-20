#include "INetworkClient.h"

// ================================================================================
INetworkClient::INetworkClient()
{
}

// ================================================================================
INetworkClient::~INetworkClient()
{
	Disconnect();
}

// ================================================================================
bool INetworkClient::Connect(const std::string& host, const uint16_t port)
{
	try
	{
		asio::ip::tcp::resolver resolver(m_Context);
		const auto endpoints = resolver.resolve(host, std::to_string(port));

		m_Connection = std::make_unique<NetworkConnection>(ENetworkConnectionOwner::Client,
			AsioSocket(m_Context), m_Context, m_MessagesIn);

		m_Connection->ConnectToServer(endpoints);

		m_ThreadContext = std::thread([this]()
			{
				m_Context.run();
			});

		std::cout << "Connected to " << host << " on port " << std::to_string(port) << ".\n";
	}
	catch (std::exception& e)
	{
		std::cout << "Client exception: " << e.what() << "\n";
		return false;
	}

	return true;
}

// ================================================================================
void INetworkClient::Disconnect()
{
	if (IsConnected())
	{
		m_Connection->Disconnect();
	}
	m_Context.stop();

	if (m_ThreadContext.joinable())
	{
		m_ThreadContext.join();
	}

	m_Connection.release();
	std::cout << "Disonnected from server.\n";
}

// ================================================================================
bool INetworkClient::IsConnected() const
{
	if (m_Connection)
	{
		return m_Connection->IsConnected();
	}

	return false;
}

// ================================================================================
void INetworkClient::Update(size_t maxMessages)
{
	size_t messageCount = 0;
	while (messageCount < maxMessages && !m_MessagesIn.empty())
	{
		auto msg = m_MessagesIn.pop_front();
		OnMessage(msg.m_Message);
		messageCount++;
	}
}

// ================================================================================
void INetworkClient::Send(const NetworkMessage& msg)
{
	if (IsConnected())
	{
		m_Connection->Send(msg);
	}
}

// ================================================================================
TSQueue<OwnedNetworkMessage>& INetworkClient::GetMessagesIn()
{
	return m_MessagesIn;
}