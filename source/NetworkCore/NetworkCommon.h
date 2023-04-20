#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <deque>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio\ts\buffer.hpp>
#include <asio\ts\internet.hpp>

class NetworkConnection;

// =============================================================================
enum class EMessageType
	: uint32_t
{
	Invalid = 0

	, ServerAccept
	, ServerMessage
	, PingServer
	, MessageAll

	, Count
};

// =============================================================================
typedef std::shared_ptr<NetworkConnection>		NetworkConnectionSharedPtr;
typedef std::unique_ptr<NetworkConnection>		NetworkConnectionUniquePtr;
typedef asio::io_context						AsioContext;
typedef asio::ip::tcp::socket					AsioSocket;
typedef asio::ip::tcp::acceptor					AsioAcceptor;
typedef std::thread								Thread;
typedef uint32_t								ConnectionId;