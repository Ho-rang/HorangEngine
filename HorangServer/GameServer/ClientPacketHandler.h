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
	PKT_C_SIGNIN = 1004,
	PKT_S_SIGNIN_OK = 1005,
	PKT_C_SIGNUP = 1006,
	PKT_S_SIGNUP_OK = 1007,
	PKT_C_ROOM_CREATE = 1008,
	PKT_C_ROOM_ENTER = 1009,
	PKT_S_ROOM_ENTER = 1010,
	PKT_C_ROOM_LEAVE = 1011,
	PKT_S_ROOM_LEAVE = 1012,
	PKT_S_ANOTHER_ENTER_ROOM = 1013,
	PKT_S_ANOTHER_LEAVE_ROOM = 1014,
	PKT_C_ROOM_START = 1015,
	PKT_S_ROOM_START = 1016,
	PKT_C_PLAY_UPDATE = 1017,
	PKT_S_PLAY_UPDATE = 1018,
	PKT_C_ROOM_LIST_REQUEST = 1019,
	PKT_S_ROOM_LIST = 1020,
};

// Custom Handlers
bool Handle_INVALID(Horang::PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_TEST(Horang::PacketSessionRef& session, Protocol::C_TEST& pkt);
bool Handle_C_MOVE(Horang::PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_SIGNIN(Horang::PacketSessionRef& session, Protocol::C_SIGNIN& pkt);
bool Handle_C_SIGNUP(Horang::PacketSessionRef& session, Protocol::C_SIGNUP& pkt);
bool Handle_C_ROOM_CREATE(Horang::PacketSessionRef& session, Protocol::C_ROOM_CREATE& pkt);
bool Handle_C_ROOM_ENTER(Horang::PacketSessionRef& session, Protocol::C_ROOM_ENTER& pkt);
bool Handle_C_ROOM_LEAVE(Horang::PacketSessionRef& session, Protocol::C_ROOM_LEAVE& pkt);
bool Handle_C_ROOM_START(Horang::PacketSessionRef& session, Protocol::C_ROOM_START& pkt);
bool Handle_C_PLAY_UPDATE(Horang::PacketSessionRef& session, Protocol::C_PLAY_UPDATE& pkt);
bool Handle_C_ROOM_LIST_REQUEST(Horang::PacketSessionRef& session, Protocol::C_ROOM_LIST_REQUEST& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_TEST] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_SIGNIN] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SIGNIN>(Handle_C_SIGNIN, session, buffer, len); };
		GPacketHandler[PKT_C_SIGNUP] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_SIGNUP>(Handle_C_SIGNUP, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_CREATE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ROOM_CREATE>(Handle_C_ROOM_CREATE, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_ENTER] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ROOM_ENTER>(Handle_C_ROOM_ENTER, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_LEAVE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ROOM_LEAVE>(Handle_C_ROOM_LEAVE, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_START] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ROOM_START>(Handle_C_ROOM_START, session, buffer, len); };
		GPacketHandler[PKT_C_PLAY_UPDATE] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_PLAY_UPDATE>(Handle_C_PLAY_UPDATE, session, buffer, len); };
		GPacketHandler[PKT_C_ROOM_LIST_REQUEST] = [](Horang::PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_ROOM_LIST_REQUEST>(Handle_C_ROOM_LIST_REQUEST, session, buffer, len); };
	}

	static bool HandlePacket(Horang::PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		Horang::PacketHeader* header = reinterpret_cast<Horang::PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ERROR& pkt) { return MakeSendBuffer(pkt, PKT_S_ERROR); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_SIGNIN_OK& pkt) { return MakeSendBuffer(pkt, PKT_S_SIGNIN_OK); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_SIGNUP_OK& pkt) { return MakeSendBuffer(pkt, PKT_S_SIGNUP_OK); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ROOM_ENTER& pkt) { return MakeSendBuffer(pkt, PKT_S_ROOM_ENTER); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ROOM_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_S_ROOM_LEAVE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ANOTHER_ENTER_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_S_ANOTHER_ENTER_ROOM); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ANOTHER_LEAVE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_S_ANOTHER_LEAVE_ROOM); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ROOM_START& pkt) { return MakeSendBuffer(pkt, PKT_S_ROOM_START); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_PLAY_UPDATE& pkt) { return MakeSendBuffer(pkt, PKT_S_PLAY_UPDATE); }
	static Horang::SendBufferRef MakeSendBuffer(Protocol::S_ROOM_LIST& pkt) { return MakeSendBuffer(pkt, PKT_S_ROOM_LIST); }

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