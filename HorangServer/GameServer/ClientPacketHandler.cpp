#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include <codecvt>

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
	auto dbConn = GDBConnectionPool->Pop();

	DBBind<2, 2> dbBind(*dbConn, L"SELECT uid, nickname FROM user WHERE id = (?) AND password = (?);");

	//dbBind.BindParam(0, pkt.id());
	//dbBind.BindParam(1, pkt.password());

	int32 uid = 0;
	WCHAR nickName[16] = L"";

	//dbBind.BindCol(0, uid);
	//dbBind.BindCol(1, nickName);

	ASSERT_CRASH(dbBind.Execute());

	if (dbBind.Fetch())
	{
		wcout << uid << " : " << nickName << endl;
		// 성공 동작
		Protocol::S_SIGNIN_OK packet;
		packet.set_uid(uid);
		packet.set_nickname(nickName);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		session->Send(sendBuffer);
	}
	else
	{
		// Null
		Protocol::S_ERROR packet;
		packet.set_errorcode(ErrorCode::SIGNIN_FAIL);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		session->Send(sendBuffer);
	}

	GDBConnectionPool->Push(dbConn);

	return true;
}

bool Handle_C_SIGNUP(PacketSessionRef& session, Protocol::C_SIGNUP& pkt)
{
	

	//auto dbConn = GDBConnectionPool->Pop();

	//GDBConnectionPool->Push(dbConn);

	return true;
}
