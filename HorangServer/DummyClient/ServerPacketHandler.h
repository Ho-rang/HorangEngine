#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(Horang::PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_TEST = 1000,
	PKT_S_TEST = 1001,
	PKT_C_MOVE = 1002,
	PKT_S_ERROR = 1003,
	PKT_S_CONNECTED = 1004,
	PKT_C_AUTOLOGIN = 1005,
	PKT_C_SIGNIN = 1006,
	PKT_S_SIGNIN_OK = 1007,
	PKT_C_SIGNUP = 1008,
	PKT_S_SIGNUP_OK = 1009,
	PKT_C_ROOM_CREATE = 1010,
	PKT_C_ROOM_SETTING = 1011,
	PKT_C_ROOM_ENTER = 1012,
	PKT_S_ROOM_ENTER = 1013,
	PKT_C_ROOM_LEAVE = 1014,
	PKT_S_ROOM_LEAVE = 1015,
	PKT_S_ANOTHER_ENTER_ROOM = 1016,
	PKT_S_ANOTHER_LEAVE_ROOM = 1017,
	PKT_C_ROOM_START = 1018,
	PKT_S_ROOM_START = 1019,
	PKT_C_ROOM_CHANGE_TEAM = 1020,
	PKT_S_ROOM_CHANGE_TEAM = 1021,
	PKT_C_PLAY_UPDATE = 1022,
	PKT_S_PLAY_UPDATE = 1023,
	PKT_C_ROOM_LIST_REQUEST = 1024,
	PKT_S_ROOM_LIST = 1025,
};

// Custom Handlers
bool Handle_INVALID(Horang::PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_TEST(Horang::PacketSessionRef& session, Protocol::S_TEST& pkt);
bool Handle_S_ERROR(Horang::PacketSessionRef& session, Protocol::S_ERROR& pkt);
bool Handle_S_CONNECTED(Horang::PacketSessionRef& session, Protocol::S_CONNECTED& pkt);
bool Handle_S_SIGNIN_OK(Horang::PacketSessionRef& session, Protocol::S_SIGNIN_OK& pkt);
bool Handle_S_SIGNUP_OK(Horang::PacketSessionRef& session, Protocol::S_SIGNUP_OK& pkt);
bool Handle_S_ROOM_ENTER(Horang::PacketSessionRef& session, Protocol::S_ROOM_ENTER& pkt);
bool Handle_S_ROOM_LEAVE(Horang::PacketSessionRef& session, Protocol::S_ROOM_LEAVE& pkt);
bool Handle_S_ANOTHER_ENTER_ROOM(Horang::PacketSessionRef& session, Protocol::S_ANOTHER_ENTER_ROOM& pkt);
bool Handle_S_ANOTHER_LEAVE_ROOM(Horang::PacketSessionRef& session, Protocol::S_ANOTHER_LEAVE_ROOM& pkt);
bool Handle_S_ROOM_START(Horang::PacketSessionRef& session, Protocol::S_ROOM_START& pkt);
bool Handle_S_ROOM_CHANGE_TEAM(Horang::PacketSessionRef& session, Protocol::S_ROOM_CHANGE_TEAM& pkt);
bool Handle_S_PLAY_UPDATE(Horang::PacketSessionRef& session, Protocol::S_PLAY_UPDATE& pkt);
bool Handle_S_ROOM_LIST(Horang::PacketSessionRef& session, Protocol::S_ROOM_LIST& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_TEST] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
		GPacketHandler[PKT_S_ERROR] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ERROR>(Handle_S_ERROR, session, buffer, len); };
		GPacketHandler[PKT_S_CONNECTED] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CONNECTED>(Handle_S_CONNECTED, session, buffer, len); };
		GPacketHandler[PKT_S_SIGNIN_OK] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SIGNIN_OK>(Handle_S_SIGNIN_OK, session, buffer, len); };
		GPacketHandler[PKT_S_SIGNUP_OK] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SIGNUP_OK>(Handle_S_SIGNUP_OK, session, buffer, len); };
		GPacketHandler[PKT_S_ROOM_ENTER] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ROOM_ENTER>(Handle_S_ROOM_ENTER, session, buffer, len); };
		GPacketHandler[PKT_S_ROOM_LEAVE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ROOM_LEAVE>(Handle_S_ROOM_LEAVE, session, buffer, len); };
		GPacketHandler[PKT_S_ANOTHER_ENTER_ROOM] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ANOTHER_ENTER_ROOM>(Handle_S_ANOTHER_ENTER_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_ANOTHER_LEAVE_ROOM] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ANOTHER_LEAVE_ROOM>(Handle_S_ANOTHER_LEAVE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_ROOM_START] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ROOM_START>(Handle_S_ROOM_START, session, buffer, len); };
		GPacketHandler[PKT_S_ROOM_CHANGE_TEAM] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ROOM_CHANGE_TEAM>(Handle_S_ROOM_CHANGE_TEAM, session, buffer, len); };
		GPacketHandler[PKT_S_PLAY_UPDATE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLAY_UPDATE>(Handle_S_PLAY_UPDATE, session, buffer, len); };
		GPacketHandler[PKT_S_ROOM_LIST] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ROOM_LIST>(Handle_S_ROOM_LIST, session, buffer, len); };
	}

	static bool HandlePacket(Horang::PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		Horang::PacketHeader* header = reinterpret_cast<Horang::PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_TEST& pkt) { return MakeSendBuffer(pkt, PKT_C_TEST); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_AUTOLOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_AUTOLOGIN); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_SIGNIN& pkt) { return MakeSendBuffer(pkt, PKT_C_SIGNIN); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_SIGNUP& pkt) { return MakeSendBuffer(pkt, PKT_C_SIGNUP); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_CREATE& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_CREATE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_SETTING& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_SETTING); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_ENTER); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_LEAVE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_START& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_START); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_CHANGE_TEAM& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_CHANGE_TEAM); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_PLAY_UPDATE& pkt) { return MakeSendBuffer(pkt, PKT_C_PLAY_UPDATE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::C_ROOM_LIST_REQUEST& pkt) { return MakeSendBuffer(pkt, PKT_C_ROOM_LIST_REQUEST); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, Horang::PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(Horang::PacketHeader), len - sizeof(Horang::PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static Horang::SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(Horang::PacketHeader);

		Horang::SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		Horang::PacketHeader* header = reinterpret_cast<Horang::PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};