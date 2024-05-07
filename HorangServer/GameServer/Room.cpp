#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"

using namespace Horang;

Room::Room()
{
	this->Initialize();
}

Room::~Room()
{
	this->Initialize();
}

void Room::Initialize()
{
	this->_roomId = 0;
	this->_roomCode = "0000";
	this->_state = Protocol::eRoomState::ROOM_STATE_LOBBY;

	for (auto& [uid, user] : _players)
	{
		auto player = user.player.lock();
		if (player == nullptr)
			continue;

		player->ownerGameSession->_room.reset();
	}

	this->_players.clear();

	this->_roomName = "";
	this->_password = "";
	this->_maxPlayerCount = 6;
	this->_currentPlayerCount = 0;
	this->_isPrivate = false;
	this->_isTeam = false;
	this->_gameTime = 0;
}

bool Room::Enter(PlayerWeakRef playerWeak)
{
	auto player = playerWeak.lock();
	if (player == nullptr)
		return false;

	// ������
	if (_state != Protocol::eRoomState::ROOM_STATE_LOBBY)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_PLAYING);

		return false;
	}

	// �ο� �ʰ�
	if (_players.size() > 6)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_FULL);
		return false;
	}

	_players[player->uid] = { player };

	player->ownerGameSession->_room = std::enable_shared_from_this<Room>::weak_from_this();

	// ������ ���� ��
	if (_players.size() == 1)
		_players[player->uid].data.set_host(true);

	// ������ ������� �� ���� �����ֱ�
	{
		Protocol::S_ROOM_ENTER packet;
		auto roomInfo = packet.mutable_roominfo();
		this->GetRoomInfo(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		player->ownerGameSession->Send(sendBuffer);
	}

	// ���� �� ����鿡�� ����
	{
		Protocol::S_ANOTHER_ENTER_ROOM packet;
		auto roomInfo = packet.mutable_roominfo();
		this->GetRoomInfo(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	

	return true;
}

bool Room::Leave(PlayerWeakRef playerWeak)
{
	auto player = playerWeak.lock();
	if (player == nullptr)
		return false;

	// Todo 1�� �������� �� ����
	if (this->_players.size() == 1)
	{
		this->_players.clear();
		this->_state = Protocol::eRoomState::ROOM_STATE_LOBBY;
		GRoomManager.Push(Horang::MakeShared<DestroyRoomJob>(this->std::enable_shared_from_this<Room>::shared_from_this()));
		return true;
	}

	// ������ ��������
	if (this->_players[player->uid].data.host() == true)
	{
		this->_players[player->uid].data.set_host(false);
		for (auto& [uid, player] : this->_players)
		{
			if (player.data.host() == false)
			{
				player.data.set_host(true);
				break;
			}
		}
	}

	this->_players.erase(player->uid);
	player->ownerGameSession->_room.reset();

	// ������ �ٸ� ������� �˷��ֱ�
	{
		Protocol::S_ANOTHER_LEAVE_ROOM packet;
		auto roomInfo = packet.mutable_roominfo();
		this->GetRoomInfo(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	return true;
}

void Room::BroadCast(Horang::SendBufferRef sendBuffer)
{
	for (auto& [id, playerData] : _players)
	{
		playerData.player.lock()->ownerGameSession->Send(sendBuffer);
	}
}

void Room::ClientUpdate(PlayerWeakRef playerWeak, Protocol::C_PLAY_UPDATE& pkt)
{
	auto player = playerWeak.lock();
	if (player == nullptr)
		return;

	// �� ���°� �������� �ƴҶ�
	if (this->_state != Protocol::eRoomState::ROOM_STATE_PLAY)
		return;

	// �÷��̾ �濡 �������� ������
	if (_players.find(player->uid) == _players.end())
		return;

	// player�� ���� ������Ʈ
	_players[player->uid].data.CopyFrom(pkt.playerdata());
}

void Room::GameStart(PlayerWeakRef playerWeak)
{
	auto player = playerWeak.lock();
	if (player == nullptr)
		return;

	if (this->_players[player->uid].data.host() == false)
		return;

	if (_players.size() < 2)
		return;

	this->_state = Protocol::eRoomState::ROOM_STATE_PLAY;
	_gameTime = ::GetTickCount64();

	// Todo �������� ����
	// Todo �÷��̾� ��ġ ����
	// Todo �÷��̾� ���� ����
	// Todo �÷��̾� ���� �����ֱ�
	// Todo ���� ���� ���� �����ֱ�


	// Todo ���� ����
	Horang::JobRef job = Horang::MakeShared<UpdateJob>(this->std::enable_shared_from_this<Room>::shared_from_this());
	this->Push(job);
}

void Room::Update()
{
	if (this->_state != Protocol::eRoomState::ROOM_STATE_PLAY)
		return;

	// ����� ���� ��
	if (_players.size() == 0)
	{
		this->Initialize();
		return;
	}

	// ���� �ð��� ���� 16ms (60fps) �̻� ������ ���� ������Ʈ
	auto currentTime = ::GetTickCount64();

	// 16ms �̻� ������ ���� ������Ʈ
	if (currentTime - _gameTime < 16)
	{
		JobRef job = Horang::MakeShared<UpdateJob>(this->std::enable_shared_from_this<Room>::shared_from_this());
		this->Push(job);
		return;
	}

	_gameTime = currentTime;

	// �̶� �濡 �����ϴ� player�鿡�� player ������ ���������
	{
		Protocol::S_PLAY_UPDATE packet;
		this->SetUpdatePacket(packet);


		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}


	// �ٽ� Update Job ���� ��⿭�� �ֱ�
	{
		Horang::JobRef job = Horang::MakeShared<UpdateJob>(this->std::enable_shared_from_this<Room>::shared_from_this());
		this->Push(job);
	}
}

Protocol::RoomInfo Room::GetRoomInfo()
{
	Protocol::RoomInfo roomInfo;
	this->GetRoomInfo(roomInfo);
	return roomInfo;
}

void Room::GetRoomInfo(Protocol::RoomInfo& roomInfo)
{
	roomInfo.set_roomid(_roomId);
	roomInfo.set_roomcode(_roomCode);
	roomInfo.set_state(_state);

	for (auto& [uid, player] : _players)
	{
		auto userInfo = roomInfo.add_users();
		player.player.lock()->GetUserInfo(userInfo);
	}

	roomInfo.set_roomname(_roomName);
	roomInfo.set_password(_password);
	roomInfo.set_maxplayercount(_maxPlayerCount);
	roomInfo.set_currentplayercount(_currentPlayerCount);
	roomInfo.set_isprivate(_isPrivate);
	roomInfo.set_isteam(_isTeam);
}

void Room::GetRoomInfo(Protocol::RoomInfo* roomInfo)
{
	this->GetRoomInfo(*roomInfo);
}

void Room::GetRoomInfoList(Protocol::RoomInfo* roomInfo)
{
	roomInfo->set_roomid(_roomId);
	roomInfo->set_roomcode(_roomCode);
	roomInfo->set_state(_state);
	roomInfo->set_roomname(_roomName);
	roomInfo->set_password(_password);
	roomInfo->set_maxplayercount(_maxPlayerCount);
	roomInfo->set_currentplayercount(_currentPlayerCount);
	roomInfo->set_isprivate(_isPrivate);
	roomInfo->set_isteam(_isTeam);
}

void Room::SetUpdatePacket(Protocol::S_PLAY_UPDATE& packet)
{
	this->GetRoomInfo(packet.mutable_roominfo());
	for (auto& [uid, playerData] : _players)
	{
		auto player = packet.add_playerdata();
		player->CopyFrom(playerData.data);
	}
}

void Room::GetPlayerData(Protocol::PlayerData& playerData, int32 uid)
{
	playerData.CopyFrom(_players[uid].data);
}