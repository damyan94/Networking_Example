#include "NetworkMessage.h"

// =============================================================================
uint32_t NetworkMessage::GetSize() const
{
	return (uint32_t)m_Body.m_Data.size();
}