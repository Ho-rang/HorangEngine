#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "JobTimer.h"
#include "Log.h"

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
	LogBuffer log("Enter");

	auto player = playerWeak.lock();
	if (player == nullptr)
	{
		log.PlayerNullptr();
		return false;
	}

	if (password != this->_password)
	{
		log << "Password Incorrect";
		player->ownerGameSession->SendError(ErrorCode::ROOM_PASSWORD_INCORRECT);
		return false;
	}

	// ������
	if (_state != Protocol::eRoomState::ROOM_STATE_LOBBY)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_PLAYING);

		log << "Room Playing";
		return false;
	}

	// �ο� �ʰ�
	if (_players.size() > 6)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_FULL);

		log << "Room Full";

		return false;
	}

	log << "Player : " << player->nickname;

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

	log << "Success";

	return true;
}

bool Room::Leave(PlayerWeakRef playerWeak)
{
	LogBuffer log("Leave");

	auto player = playerWeak.lock();

	// Todo 1�� �������� �� ����
	if (this->_players.size() <= 1)
	{
		GRoomManager->Push(Horang::MakeShared<DestroyRoomJob>(this->GetWeakRef()));
		log << "Destroy Room";
		return true;
	}


	if (player == nullptr)
	{
		log.PlayerNullptr();
		return false;
	}

	// ������ ��������
	if (this->_players[player->uid].data.host() == true)
	{
		log << "Host Leave";
		this->_players[player->uid].data.set_host(false);
		for (auto& [uid, player] : this->_players)
		{
			if (player.data.host() == false)
			{
				player.data.set_host(true);
				log << "New Host : " << player.data.userinfo().nickname();
				break;
			}
		}
	}

	this->_players.erase(player->uid);
	player->ownerGameSession->_room.reset();

	log << "Player : " << player->nickname;

	// ������ �ٸ� ������� �˷��ֱ�
	{
		Protocol::S_ANOTHER_LEAVE_ROOM packet;
		auto roomInfo = packet.mutable_roominfo();
		this->GetRoomInfo(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	{
		Protocol::S_ROOM_LEAVE packet;

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		player->ownerGameSession->Send(sendBuffer);
	}

	log << "Success";

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
	LogBuffer log("GameStart");

	auto player = playerWeak.lock();
	if (player == nullptr)
	{
		log.PlayerNullptr();
		return;
	}

	if (this->_players[player->uid].data.host() == false)
	{
		log << "Not Host";
		return;
	}

	if (_players.size() < 2)
	{
		log << "Not Enough Player";
		return;
	}

	log << "Player : " << player->nickname;

	this->_state = Protocol::eRoomState::ROOM_STATE_PLAY;
	_gameTime = ::GetTickCount64();

	// Todo �������� ����
	// Todo �÷��̾� ��ġ ����
	// Todo �÷��̾� ���� ����
	// Todo �÷��̾� ���� �����ֱ�

	// Todo ���� ���� ���� �����ֱ�
	{
		Protocol::S_ROOM_START packet;
		this->GetRoomInfo(packet.mutable_roominfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	// Todo ���� ����
	Horang::JobRef job = Horang::MakeShared<UpdateJob>(this->GetSharedRef());
	this->Push(job);

	log << "Success";
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

void Room::Kick(PlayerWeakRef playerWeak, std::string targetNickName /*= ""*/)
{
	LogBuffer log("Kick");

	if (this->_state != Protocol::ROOM_STATE_LOBBY)
	{
		log << "Not Lobby";
		return;
	}

	auto playerRef = playerWeak.lock();
	if (playerRef == nullptr)
	{
		log.PlayerNullptr();
		return;
	}

	log << "Host : " << playerRef->nickname;

	if (this->_players.find(playerRef->uid) == this->_players.end())
	{
		log << "Not Found Player";
		return;
	}

	if (targetNickName.length() == 0)
	{
		log << "Not Found Target";
		return;
	}

	if (_players[playerRef->uid].data.host() == false)
	{
		log << "Not Host";
		return;
	}

	for (auto& [uid, playerData] : _players)
	{
		if (playerData.data.userinfo().nickname() != targetNickName ||
			_players[playerRef->uid].data.userinfo().nickname() == targetNickName)
			continue;

		{
			Protocol::S_ROOM_KICK packet;

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
			playerData.player.lock()->ownerGameSession->Send(sendBuffer);
		}

		this->_players.erase(uid);
		playerRef->ownerGameSession->_room.reset();

		{
			Protocol::S_ANOTHER_LEAVE_ROOM packet;

			auto roomInfo = packet.mutable_roominfo();
			this->GetRoomInfo(roomInfo);

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
			this->BroadCast(sendBuffer);
		}

		log << "kickPlayer : " << targetNickName;
		break;
	}

	log << "Success";
}

void Room::ChangeTeam(PlayerWeakRef playerWeak, Protocol::eTeamColor color, std::string targetNickName /*= ""*/)
{
	LogBuffer log("ChangeTeam");

	if (this->_state != Protocol::ROOM_STATE_LOBBY)
	{
		log << "Not Lobby";
		return;
	}

	auto playerRef = playerWeak.lock();
	if (playerRef == nullptr)
	{
		log.PlayerNullptr();
		return;
	}

	if (this->_players.find(playerRef->uid) == this->_players.end())
	{
		log << "Not Found Player";
		return;
	}

	if (color == Protocol::TEAM_COLOR_NONE ||
		color < Protocol::TEAM_COLOR_RED ||
		color > Protocol::TEAM_COLOR_BLUE)
	{
		log << "Invalid Team Color";
		return;
	}

	auto& player = _players[playerRef->uid];
	if (player.data.team() == color)
	{
		log << "Same Team";
		return;
	}

	if (targetNickName.length() == 0)
	{
		log << "No Target";
		return;
	}

	if (targetNickName == player.data.userinfo().nickname())
	{
		player.data.set_team(color);
		log << "Change My Team";
	}
	else if (player.data.host())
	{
		for (auto& [uid, playerData] : _players)
		{
			if (playerData.data.userinfo().nickname() == targetNickName)
			{
				playerData.data.set_team(color);
				log << "Change Target Team : " << targetNickName;
				break;
			}
		}
	}
	else
	{
		log << "Not Host";
		return;
	}

	// �濡 �ִ� ����鿡�� �����ֱ�
	{
		Protocol::S_ROOM_CHANGE_TEAM packet;
		this->GetRoomInfo(packet.mutable_roominfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	log << "Success";
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
}