﻿#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;

		Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	ServerPacketHandler::Init();


	int count = 64*4;

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager 등
		count
	);

	ASSERT_CRASH(service->Start());

	this_thread::sleep_for(10s);

	for (int32 i = 0; i < 10; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	//this_thread::sleep_for(5s);
	/*
	Protocol::C_CHAT chatPkt;
	chatPkt.set_msg(u8"Hello World");
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
	while (true)
	{
		service->BroadCast(sendBuffer);

		this_thread::sleep_for(1s);
	}
	*/

	for (int32 i = 0; i < 4; i++)
	{
		GThreadManager->Launch([=]()
			{
				auto startTime = std::chrono::steady_clock::now();

				int second = 0;

				while (true)
				{
					auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);;

					if (deltaTime.count() >= 1000 / 60)
					{
						Protocol::C_TEST packet;
						packet.set_num(0);
						for (int i = 0; i < count / 4; i++)
						{
							auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
							service->BroadCast(sendBuffer);
						}

						// 시작 시간 갱신
						startTime = std::chrono::steady_clock::now();
					}

					std::this_thread::sleep_for(1ms);
				}
			});
	}


	/*this_thread::sleep_for(1s);
	Protocol::C_TEST packet;
	packet.set_num(0);
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(packet);
	service->BroadCast(sendBuffer);

	this_thread::sleep_for(20s);*/

	GThreadManager->Join();

	return 0;
}