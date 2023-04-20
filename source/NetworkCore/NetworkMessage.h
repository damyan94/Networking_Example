#pragma once

#include "NetworkCommon.h"

class NetworkConnection;

// =============================================================================
struct NetworkMessageHeader
{
	EMessageType			m_Type = EMessageType::Invalid;
	uint32_t				m_Size = 0;
};

// =============================================================================
struct NetworkMessageBody
{
	std::vector<uint8_t>	m_Data;
};

// =============================================================================
struct NetworkMessage
{
	uint32_t GetSize() const;

	template<typename T>
	friend NetworkMessage& operator<<(NetworkMessage& msg, const T& data);
	template<typename T>
	friend NetworkMessage& operator>>(NetworkMessage& msg, T& data);

	NetworkMessageHeader		m_Header;
	NetworkMessageBody			m_Body;
};

// =============================================================================
struct OwnedNetworkMessage
{
	NetworkConnectionSharedPtr	m_Remote;
	NetworkMessage				m_Message;
};

// =============================================================================
template<typename T>
inline NetworkMessage& operator<<(NetworkMessage& msg, const T& data)
{
	static_assert(std::is_standard_layout<T>::value, "Data too complex to serialize trivially.");
	size_t bodySize = msg.m_Body.m_Data.size();
	size_t dataSize = sizeof(T);

	msg.m_Body.m_Data.resize(bodySize + dataSize);
	memcpy(msg.m_Body.m_Data.data() + bodySize, &data, dataSize);

	msg.m_Header.m_Size = msg.GetSize();

	return msg;
}

// =============================================================================
template<typename T>
inline NetworkMessage& operator>>(NetworkMessage& msg, T& data)
{
	static_assert(std::is_standard_layout<T>::value, "Data too complex to serialize trivially.");
	size_t bodySize = msg.m_Body.m_Data.size();
	size_t dataSize = sizeof(T);
	size_t startPos = bodySize - dataSize;

	memcpy(&data, msg.m_Body.m_Data.data() + startPos, dataSize);
	msg.m_Body.m_Data.resize(startPos);

	msg.m_Header.m_Size = msg.GetSize();

	return msg;
}