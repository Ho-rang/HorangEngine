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

	// 게임중
	if (_state != Protocol::eRoomState::ROOM_STATE_LOBBY)
	{
		player->ownerGameSession->SendError(ErrorCode::ROOM_PLAYING);

		log << "Room Playing";
		return false;
	}

	// 인원 초과
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

	// 혼자일때 방장 권한
	if (_players.size() == 1)
		playerData.set_host(true);

	auto roomInfo = this->GetRoomInfo();
	// 접속한 사람에게 방 정보 보내주기
	{
		Protocol::S_ROOM_ENTER packet;
		packet.mutable_roominfo()->CopyFrom(roomInfo);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		player->ownerGameSession->Send(sendBuffer);
	}

	// 기존 방 사람들에게 보냄
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

	// Todo 1명만 남았을때 방 삭제
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

	// 방장이 나갔을때
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

	// 나간거 다른 사람에게 알려주기
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
		// Todo dummy 고려한 코드
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

	// 방 상태가 게임중이 아닐때
	if (this->_state != Protocol::eRoomState::ROOM_STATE_PLAY)
		return;

	// 플레이어가 방에 존재하지 않을때
	if (_players.find(player->uid) == _players.end())
		return;

	// player 정보 업데이트
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

	// Todo 시작지점 설정
	// Todo 플레이어 위치 설정
	// Todo 플레이어 상태 설정
	// Todo 플레이어 정보 보내주기

	// Todo 게임 시작 정보 보내주기
	{
		Protocol::S_ROOM_START packet;
		this->GetRoomInfo(packet.mutable_roominfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}

	// Todo 게임 시작
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

	// 이때 방에 존재하는 player들에게 player 정보를 보내줘야함
	{
		Protocol::S_PLAY_UPDATE packet;
		this->SetUpdatePacket(packet);


		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
		this->BroadCast(sendBuffer);
	}


	// 다시 Update Job 만들어서 대기열에 넣기
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

	// 방에 있는 사람들에게 보내주기
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