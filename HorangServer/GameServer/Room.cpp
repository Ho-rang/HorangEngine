#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "JobTimer.h"

using namespace Horang;

Room::Room()
{
	this->Initialize();
}

Room::~Room()
{
	Horang::DebugLog() << L"NnNNNNNNNNNNNNNNNNNNNN";
	this->Initialize();
}

void Room::Initialize(int32 roomId /*= 0*/, std::string roomCode /*= "0000"*/)
{
	this->_roomId = roomId;
	this->_roomCode = roomCode;
	this->_state = Protocol::eRoomState::ROOM_STATE_NONE;

	for (auto& [uid, user] : _players)
	{
		auto player = user.player.lock();
		if (player == nullptr)
			continue;

		player->ownerGameSession->_room.reset();
	}

	this->_players.clear();

	this->_roomName = "Init";
	this->_password = "Init";
	this->_maxPlayerCount = 6;
	this->_isPrivate = false;
	this->_isTeam = false;
	this->_gameTime = 0;
}

bool Room::Enter(PlayerWeakRef playerWeak, std::string password)
{
	auto player = playerWeak.lock();
	if (player == nullptr)
		return false;

	if (password != this->_password)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_PASSWORD_INCORRECT);
		return false;
	}

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

	player->ownerGameSession->_room = this->GetWeakRef();

	auto& playerData = _players[player->uid].data;
	auto userInfo = playerData.mutable_userinfo();
	player->GetUserInfo(userInfo);

	// ȥ���϶� ���� ����
	if (_players.size() == 1)
		playerData.set_host(true);

	auto roomInfo = this->GetRoomInfo();
	// ������ ������� �� ���� �����ֱ�
	{
		Protocol::S_ROOM_ENTER packet;
		packet.mutable_roominfo()->CopyFrom(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		player->ownerGameSession->Send(sendBuffer);
	}

	// ���� �� ����鿡�� ����
	{
		Protocol::S_ANOTHER_ENTER_ROOM packet;
		packet.mutable_roominfo()->CopyFrom(roomInfo);

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
		GRoomManager->Push(Horang::MakeShared<DestroyRoomJob>(this->GetWeakRef()));
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
		// Todo dummy ����� �ڵ�
		auto player = playerData.player.lock();
		if (player == nullptr)
			continue;

		player->ownerGameSession->Send(sendBuffer);
		//playerData.player.lock()->ownerGameSession->Send(sendBuffer);
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

	// player ���� ������Ʈ
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
	Horang::JobRef job = Horang::MakeShared<UpdateJob>(this->GetSharedRef());
	this->Push(job);
}

void Room::Update()
{
	if (this->_state != Protocol::eRoomState::ROOM_STATE_PLAY)
		return;

	auto currentTime = ::GetTickCount64();
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
		auto job = Horang::MakeShared<UpdateJob>(this->GetSharedRef());
		/*this->Push(job);*/

		GJobTimer->Reserve(16, this->weak_from_this(), job);
	}
}

void Room::ChangeTeam(PlayerWeakRef playerWeak, Protocol::eTeamColor color)
{
	if (this->_state != Protocol::ROOM_STATE_LOBBY)
		return;

	auto playerRef = playerWeak.lock();
	if (playerRef == nullptr)
		return;

	if (this->_players.find(playerRef->uid) == this->_players.end())
		return;

	if (color == Protocol::TEAM_COLOR_NONE ||
		color < Protocol::TEAM_COLOR_RED ||
		color > Protocol::TEAM_COLOR_BLUE)
		return;

	auto& player = _players[playerRef->uid];
	if (player.data.team() == color)
		return;

	player.data.set_team(color);

	// �濡 �ִ� ����鿡�� �����ֱ�
	{
		Protocol::S_ROOM_CHANGE_TEAM packet;
		this->GetRoomInfo(packet.mutable_roominfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
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
		auto playerRef = player.player.lock();
		if (playerRef == nullptr)
			continue;

		auto playerData = roomInfo.add_users();
		this->GetPlayerData(playerData, playerRef->uid);
	}

	roomInfo.set_roomname(_roomName);
	roomInfo.set_password(_password);
	roomInfo.set_maxplayercount(_maxPlayerCount);
	roomInfo.set_currentplayercount(_players.size());
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
	roomInfo->set_currentplayercount(_players.size());
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

void Room::GetPlayerData(Protocol::PlayerData* playerData, int32 uid)
{
	//playerData->CopyFrom(_players[uid].data);
	*playerData = _players[uid].data;
	*playerData->mutable_userinfo() = _players[uid].data.userinfo();
	std::cout << " TT : " << playerData->userinfo().nickname() << std::endl;
}