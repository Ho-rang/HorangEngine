#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

/*
	ServerPacketHandler
*/

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
	Protocol::C_TEST packet;

	//cout << pkt.num() << endl;

	packet.set_num(pkt.num() + 1);

	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ERROR(PacketSessionRef& session, Protocol::S_ERROR& pkt)
{
	//cout << pkt.errorcode() << endl;

	switch (pkt.errorcode())
	{
		case 1001:
			cout << "�α��� ����" << endl;
		case 1002:
			cout << "ȸ������ ����" << endl;
		default:
			break;
	}

	return true;
}

bool Handle_S_SIGNIN_OK(PacketSessionRef& session, Protocol::S_SIGNIN_OK& pkt)
{
	cout << "�α��� ����! " << "UID : " << pkt.uid() << " NickName : " << pkt.nickname() << endl;


	return true;
}

bool Handle_S_SIGNUP_OK(PacketSessionRef& session, Protocol::S_SIGNUP_OK& pkt)
{
	cout << "ȸ������ ����!" << endl;

	return true;
}
