#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
#include "GameSession.h"
#include "Player.h"
#include "ClientPacketHandler.h"

RoomManagerRef GRoomManager;

RoomManager::RoomManager()
{

}

RoomManager::~RoomManager()
{
	// _rooms 모두 해제 후 _roomPool 모두 해제

	for (const auto& [code, room] : _rooms)
		room->Initialize();
	_rooms.clear();

	_roomPool.clear();
}

void RoomManager::Initialize()
{
	//_rooms.reserve(10000);

	for (int32 i = 1000; i < 10000; ++i)
	{
		//auto room = Horang::MakeShared<Room>();
		auto room = std::make_shared<Room>();

		room->_roomRef = room;
		room->_roomWeakRef = room;

		room->_roomId = i;
		room->_roomCode = std::to_string(i);

		_roomPool.push_back(room->GetSharedRef());
	}
}

RoomRef RoomManager::CreateRoom(std::string roomName /*= "DefaultRoomName"*/, std::string password /*= ""*/, int32 maxPlayerCount /*= 6*/, bool isPrivate /*= false*/, bool isTeam /*= false*/, int32 dummyClient /*= 0*/)
{
	if (_roomPool.empty())
		return nullptr;

	auto room = _roomPool.back();
	_roomPool.pop_back();

	room->_roomName = roomName;
	room->_password = password;

	room->_maxPlayerCount = maxPlayerCount;
	room->_isPrivate = isPrivate;
	room->_isTeam = isTeam;

	room->_state = Protocol::ROOM_STATE_LOBBY;

	_rooms[room->_roomId] = room;

	for (int32 i = 1; i <= dummyClient; i++)
	{
		Room::PlayerGameData playerData;

		auto userInfo = playerData.data.mutable_userinfo();
		userInfo->set_uid(-i);
		userInfo->set_id("Dummy" + i);
		userInfo->set_nickname("Dummy" + i);

		room->_players[-i] = playerData;
	}

	return room->GetSharedRef();
}

void RoomManager::DestroyRoom(RoomRef room)
{
	_rooms.erase(room->_roomId);

	room->Initialize(room->_roomId, room->_roomCode);
	_roomPool.push_back(room);
}

void RoomManager::RoomListUpdate()
{
	static uint64 _lastRoomListUpdateTime = 0;

	if (_lastRoomListUpdateTime > ::GetTickCount64())
		return;

	_roomList.Clear();

	for (const auto& [code, room] : _rooms)
	{
		if (room->_state != Protocol::ROOM_STATE_LOBBY)
			continue;

		auto roomInfo = _roomList.add_roominfo();
		room->GetRoomInfoList(roomInfo);
	}

	_lastRoomListUpdateTime = ::GetTickCount64() + 100;
}

void RoomManager::SendRoomList(PlayerWeakRef player)
{
	auto playerRef = player.lock();
	if (playerRef == nullptr)
		return;

	this->RoomListUpdate();

	Horang::DebugLog() << "SendRoomList";
	std::cout << "SendRoomList" << std::endl;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(_roomList);
	playerRef->ownerGameSession->Send(sendBuffer);
}

void RoomManager::EnterRoom(PlayerWeakRef player, int32 roomId, std::string password /*= ""*/)
{
	auto playerRef = player.lock();
	if (playerRef == nullptr)
		return;

	auto room = _rooms[roomId];
	if (room == nullptr)
	{
		playerRef->ownerGameSession->SendError(ErrorCode::ROOM_NOT_FOUND);
		return;
	}

	room->Push(Horang::MakeShared<EnterJob>(room, player));
}
