#pragma once
#include "JobQueue.h"

class Room : public Horang::JobQueue
{
	friend class RoomManager;

private:
	struct PlayerGameData
	{
		PlayerWeakRef player;

		Protocol::PlayerData data;
	};

public:
	Room();
	~Room();

	void Initialize(int32 roomId = 0, std::string roomCode = "0000");

public:
	bool Enter(PlayerWeakRef playerWeak, std::string password = "");
	bool Leave(PlayerWeakRef playerWeak);
	void BroadCast(Horang::SendBufferRef sendBuffer);
	void ClientUpdate(PlayerWeakRef playerWeak, Protocol::C_PLAY_UPDATE& pkt);

public:
	void GameStart(PlayerWeakRef playerWeak);
	void Update();

public: // 방 기능
	// 강퇴
	void Kick(PlayerWeakRef playerWeak, std::string targetNickName = "");

	// 팀 변경
	void ChangeTeam(PlayerWeakRef playerWeak, Protocol::eTeamColor color, std::string targetNickName = "");



public: // 패킷 작성
	Protocol::RoomInfo GetRoomInfo();
	void GetRoomInfo(Protocol::RoomInfo& roomInfo);
	void GetRoomInfo(Protocol::RoomInfo* roomInfo);
	void GetRoomInfoList(Protocol::RoomInfo* roomInfo);

	void SetUpdatePacket(Protocol::S_PLAY_UPDATE& packet);
	void GetPlayerData(Protocol::PlayerData* playerData, int32 uid);

public:
	RoomRef GetSharedRef() { return this->_roomRef; }
	RoomWeakRef GetWeakRef() { return this->_roomWeakRef; }

private:
	int32 _roomId;
	std::string _roomCode;
	Protocol::eRoomState _state;

	Horang::HashMap<int32, PlayerGameData> _players;

	std::string _roomName;
	std::string _password;

	int32 _maxPlayerCount;

	bool _isPrivate;
	bool _isTeam;

private:
	// 게임 시간
	uint64 _gameTime;

	RoomRef _roomRef;
	RoomWeakRef _roomWeakRef;
};

/////////////////////////////
// 
/////////////////////////////

class EnterJob : public Horang::IJob
{
public:
	EnterJob(RoomWeakRef room, PlayerWeakRef player, std::string password = "")
		: _room(room), _player(player), _password(password)
	{}

	virtual void Execute() override
	{
		_room.lock()->Enter(_player, _password);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
	std::string _password;
};

class LeaveJob : public Horang::IJob
{
public:
	LeaveJob(RoomWeakRef room, PlayerWeakRef player)
		: _room(room), _player(player)
	{}

	virtual void Execute() override
	{
		_room.lock()->Leave(_player);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
};

class KickJob : public Horang::IJob
{
public:
	KickJob(RoomWeakRef room, PlayerWeakRef player, std::string targetNickName)
		: _room(room), _player(player), _targetNickName(targetNickName)
	{}

	virtual void Execute() override
	{
		_room.lock()->Kick(_player, _targetNickName);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
	std::string _targetNickName;
};

class TeamChangeJob : public Horang::IJob
{
public:
	TeamChangeJob(RoomWeakRef room, PlayerWeakRef player, Protocol::eTeamColor color, std::string targetNickName)
		: _room(room), _player(player), _color(color), _targetNickName(targetNickName)
	{}

	virtual void Execute() override
	{
		_room.lock()->ChangeTeam(_player, _color, _targetNickName);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
	Protocol::eTeamColor _color;
	std::string _targetNickName;
};

class GameStartJob : public Horang::IJob
{
public:
	GameStartJob(RoomWeakRef room, PlayerWeakRef player)
		: _room(room), _player(player)
	{}

	virtual void Execute() override
	{
		_room.lock()->GameStart(_player);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
};

class ClientUpdateJob : public Horang::IJob
{
public:
	ClientUpdateJob(RoomWeakRef room, PlayerWeakRef player, Protocol::C_PLAY_UPDATE pkt)
		: _room(room), _player(player), _pkt(pkt)
	{}

	virtual void Execute() override
	{
		_room.lock()->ClientUpdate(_player, _pkt);
	}

private:
	RoomWeakRef _room;
	PlayerWeakRef _player;
	Protocol::C_PLAY_UPDATE _pkt;
};

class UpdateJob : public Horang::IJob
{
public:
	UpdateJob(RoomWeakRef room)
		: _room(room)
	{}

	virtual void Execute() override
	{
		_room.lock()->Update();
	}

private:
	RoomWeakRef _room;
};

class BroadCastJob : public Horang::IJob
{
public:
	BroadCastJob(RoomWeakRef room, Horang::SendBufferRef sendBuffer)
		: _room(room), _sendBuffer(sendBuffer)
	{}

	virtual void Execute() override
	{
		_room.lock()->BroadCast(_sendBuffer);
	}

private:
	RoomWeakRef _room;
	Horang::SendBufferRef _sendBuffer;
};