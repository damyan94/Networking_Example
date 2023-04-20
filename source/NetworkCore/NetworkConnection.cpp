#include "NetworkConnection.h"
#include "INetworkServer.h"

// ================================================================================
NetworkConnection::NetworkConnection(ENetworkConnectionOwner owner, AsioSocket socket, AsioContext& context, TSQueue<OwnedNetworkMessage>& messagesIn)
    : m_Id(0)
    , m_Owner(owner)
    , m_Socket(std::move(socket))
    , m_Context(context)
    , m_MessagesIn(messagesIn)
    , m_HandshakeOut(0)
    , m_HandshakeIn(0)
    , m_HandshakeCheck(0)
{
    if (m_Owner == ENetworkConnectionOwner::Server)
    {
        m_HandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
        m_HandshakeCheck = ScrambleData(m_HandshakeOut);
    }
    else
    {
        // Do nothing
    }
}

// ================================================================================
NetworkConnection::~NetworkConnection()
{
}

// ================================================================================
void NetworkConnection::ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
{
    if (m_Owner == ENetworkConnectionOwner::Client)
    {
        asio::async_connect(m_Socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
            {
                if (!ec)
                {
                    ReadValidation();
                }
            });
    }
}

// ================================================================================
void NetworkConnection::ConnectToClient(INetworkServer* server, ConnectionId id)
{
    if (m_Owner == ENetworkConnectionOwner::Server && m_Socket.is_open())
    {
        m_Id = id;
        WriteValidation();
        ReadValidation(server);
    }
}

// ================================================================================
void NetworkConnection::Disconnect()
{
    if (IsConnected())
    {
        asio::post(m_Context, [this]()
            {
                m_Socket.close();
            });
    }
}

// ================================================================================
bool NetworkConnection::IsConnected() const
{
    return m_Socket.is_open();
}

// ================================================================================
void NetworkConnection::Send(const NetworkMessage& msg)
{
    asio::post(m_Context, [this, msg]()
        {
            bool writingMessage = !m_MessagesOut.empty();
            m_MessagesOut.push_back(msg);
            if (!writingMessage)
            {
                WriteHeader();
            }
        });
}

// ================================================================================
ConnectionId NetworkConnection::GetId() const
{
    return m_Id;
}

// ================================================================================
void NetworkConnection::WriteValidation()
{
    asio::async_write(m_Socket, asio::buffer(&m_HandshakeOut, sizeof(m_HandshakeOut)),
        [this](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                if (m_Owner == ENetworkConnectionOwner::Client)
                {
                    ReadHeader();
                }
            }
            else
            {
                std::cout << "[" << m_Id << "] Write validation fail.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
void NetworkConnection::ReadValidation(INetworkServer* server)
{
    asio::async_read(m_Socket, asio::buffer(&m_HandshakeIn, sizeof(m_HandshakeIn)),
        [this, server](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                if (m_Owner == ENetworkConnectionOwner::Server)
                {
                    if (m_HandshakeIn == m_HandshakeCheck)
                    {
                        std::cout << "[SERVER] Client validated.\n";
                        server->OnClientValidated(this->shared_from_this());
                        ReadHeader();
                    }
                    else
                    {
                        std::cout << "Client validation failed.\n";
                        m_Socket.close();
                    }
                }
                else if (m_Owner == ENetworkConnectionOwner::Client)
                {
                    m_HandshakeOut = ScrambleData(m_HandshakeIn);
                    WriteValidation();
                }
            }
            else
            {
                std::cout << "[" << m_Id << "] Read validation failed.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
uint64_t NetworkConnection::ScrambleData(uint64_t data)
{
    uint64_t result = data ^ 0XDEADBEEFC0DECAFE;
    result = (result & 0x123456789ABCDEF) >> 4 | (result & 0xFEDCBA9876543210) << 4;
    return result ^ 0x0123ABBA4567ACDC;
}

// ================================================================================
void NetworkConnection::ReadHeader()
{
    asio::async_read(m_Socket, asio::buffer(&m_TempMessage.m_Header, sizeof(NetworkMessageHeader)),
        [this](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                const size_t messageSize = m_TempMessage.m_Header.m_Size;
                if (messageSize > 0)
                {
                    m_TempMessage.m_Body.m_Data.resize(messageSize);
                    ReadBody();
                }
                else
                {
                    AddToIncomingMessageQueue();
                }
            }
            else
            {
                std::cout << "[" << m_Id << "] Read header fail.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
void NetworkConnection::ReadBody()
{
    asio::async_read(m_Socket, asio::buffer(m_TempMessage.m_Body.m_Data.data(), m_TempMessage.m_Body.m_Data.size()),
        [this](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                AddToIncomingMessageQueue();
            }
            else
            {
                std::cout << "[" << m_Id << "] Read body fail.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
void NetworkConnection::WriteHeader()
{
    asio::async_write(m_Socket, asio::buffer(&m_MessagesOut.front().m_Header, sizeof(NetworkMessageHeader)),
        [this](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                if (m_MessagesOut.front().m_Body.m_Data.size() > 0)
                {
                    WriteBody();
                }
                else
                {
                    m_MessagesOut.pop_front();
                    if (!m_MessagesOut.empty())
                    {
                        WriteHeader();
                    }
                }
            }
            else
            {
                std::cout << "[" << m_Id << "] Write header fail.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
void NetworkConnection::WriteBody()
{
    asio::async_write(m_Socket, asio::buffer(m_MessagesOut.front().m_Body.m_Data.data(), m_MessagesOut.front().m_Body.m_Data.size()),
        [this](std::error_code ec, size_t length)
        {
            if (!ec)
            {
                m_MessagesOut.pop_front();
                if (!m_MessagesOut.empty())
                {
                    WriteHeader();
                }
            }
            else
            {
                std::cout << "[" << m_Id << "] Write body fail.\n";
                m_Socket.close();
            }
        });
}

// ================================================================================
void NetworkConnection::AddToIncomingMessageQueue()
{
    if (m_Owner == ENetworkConnectionOwner::Server)
    {
        m_MessagesIn.push_back({ this->shared_from_this(), m_TempMessage });
    }
    else
    {
        m_MessagesIn.push_back({ nullptr, m_TempMessage });
    }

    ReadHeader();
}