#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 만들기
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_C_TEST(PacketSessionRef& session, Protocol::C_TEST& pkt)
{
	Protocol::S_TEST patket;
	patket.set_num(pkt.num() + 1);

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(patket);
	// session->Send(sendBuffer);

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	return true;
}

bool Handle_C_SIGNIN(PacketSessionRef& session, Protocol::C_SIGNIN& pkt)
{

	return true;
}