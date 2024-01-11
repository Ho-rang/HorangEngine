#include "pch.h"
#include <iostream>

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "tchar.h"
#include "Protocol.pb.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "DBConnectionRef.h"

#include <codecvt>
int main()
{

	{
		//
			// Todo
			// Driver와 Server IP Database name은 별도의 config파일로 분리해서 관리하기
		{
			/*ASSERT_CRASH(GDBConnectionPool->Connect(1,
				L"Driver={MySQL ODBC 8.2 Unicode Driver};\
			Server=14.35.71.148;\
			Database=VGunDB;\
			User=VGun2;\
			Password=akdgoTdj;"));*/

			ASSERT_CRASH(GDBConnectionPool->Connect(1,
				L"Driver={MySQL ODBC 8.2 Unicode Driver};\
					Server=127.0.0.1;\
					Database=VGunDB;\
					User=VGun2;\
					Password=akdgoTdj;"));
		}

		{
			auto dbConn = GDBConnectionPool->Pop();

			DBBind<2, 2> dbBind(*dbConn, L"SELECT uid, nickname FROM user WHERE id = (?) AND password = (?);");

			string id = u8"test1";
			string password = u8"test1";
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			auto a = converter.from_bytes(id).c_str();
			auto b = converter.from_bytes(password).c_str();
			dbBind.BindParam(0, a, ::wcslen(a));
			dbBind.BindParam(1, b, ::wcslen(b));

			int32 uid = 0;
			WCHAR nickName[16] = L"";

			dbBind.BindCol(0, uid);
			dbBind.BindCol(1, nickName);

			ASSERT_CRASH(dbBind.Execute());

			if (dbBind.Fetch())
			{
				wcout << uid << " : " << nickName << endl;
				// 성공 동작
				Protocol::S_SIGNIN_OK packet;
				//packet.set_uid(uid);
				//packet.set_nickname(nickName);

				auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
			}
			else
			{
				// 실패 동작
				Protocol::S_ERROR packet;
				packet.set_errorcode(ErrorCode::SIGNIN_FAIL);

				auto sendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
			}

			GDBConnectionPool->Push(dbConn);
		}

		{
			auto dbConn = GDBConnectionPool->Pop();

			DBBind<2, 2> dbBind(*dbConn, L"SELECT uid, nickname FROM user WHERE id = (?) AND password = (?);");

			WCHAR a[40] = L"test1";
			WCHAR b[80] = L"test2";
			dbBind.BindParam(0, a);
			dbBind.BindParam(1, b);

			int32 uid = 0;
			WCHAR nickName[16] = L"";

			dbBind.BindCol(0, uid);
			dbBind.BindCol(1, nickName);

			ASSERT_CRASH(dbBind.Execute());

			if (dbBind.Fetch())
			{
				wcout << uid << " : " << nickName << endl;
			}
			else
			{
				// Null
			}

			GDBConnectionPool->Push(dbConn);
		}

		// 회원가입
		for (int32 i = 0; i < 0; i++)
		{
			auto dbConn = GDBConnectionPool->Pop();

			DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO user (id, password, nickname) VALUES(?, ?, ?)");

			WCHAR id[30] = L"test";
			dbBind.BindParam(0, id);

			WCHAR password[30] = L"1234";
			dbBind.BindParam(1, password);

			WCHAR nickName[30] = L"Test";
			dbBind.BindParam(2, nickName);

			/*TIMESTAMP_STRUCT ts = { 2023, 11, 5 };
			dbBind.BindParam(2, ts);*/

			ASSERT_CRASH(dbBind.Execute());

			{
				//// 기존의 바인딩 된 정보 날리기
				//dbConn->Unbind();

				//int32 gold = 100;
				//SQLLEN len = 0;
				//
				//WCHAR name[100] = L"호랑";
				//SQLLEN nameLen = 0;

				//TIMESTAMP_STRUCT ts = {};
				//ts.year = 2023;
				//ts.month = 11;
				//ts.day = 5;
				//SQLLEN tsLen = 0;

				//ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));
				//ASSERT_CRASH(dbConn->BindParam(2, name, &nameLen));
				//ASSERT_CRASH(dbConn->BindParam(3, &ts, &tsLen));

				//// SQL 실행
				//ASSERT_CRASH(dbConn->Execute(L"INSERT INTO Gold (gold, name, createDate) VALUES(?, ?, ?)"));
			}

			GDBConnectionPool->Push(dbConn);
		}

		// Read ID
		{
			auto dbConn = GDBConnectionPool->Pop();

			DBBind<2, 3> dbBind(*dbConn, L"SELECT uid, id, nickname FROM user WHERE id = (?) AND password = (?)");

			WCHAR a[20] = L"test";
			WCHAR b[20] = L"1234";
			dbBind.BindParam(0, a);
			dbBind.BindParam(1, b);

			int32 uid = 0;
			WCHAR id[20] = L"";
			WCHAR nickName[16] = L"";

			dbBind.BindCol(0, uid);
			dbBind.BindCol(1, id);
			dbBind.BindCol(2, nickName);

			ASSERT_CRASH(dbBind.Execute());

			while (dbBind.Fetch())
			{
				cout << "select" << endl;
			}

			GDBConnectionPool->Push(dbConn);
		}

		// Read
		{
			//auto dbConn = GDBConnectionPool->Pop();

			//DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM Gold WHERE gold = (?)");

			//int32 gold = 100;
			//dbBind.BindParam(0, gold);

			//int32 outId = 0;
			//int32 outGold = 0;
			//WCHAR outName[100];
			//TIMESTAMP_STRUCT outDate = {};

			//dbBind.BindCol(0, OUT outId);
			//dbBind.BindCol(1, OUT outGold);
			//dbBind.BindCol(2, OUT outName);
			//dbBind.BindCol(3, OUT outDate);

			//ASSERT_CRASH(dbBind.Execute());


			//dbConn->Unbind();

			//{
			//	/*int32 gold = 100;
			//	SQLLEN len = 0;
			//	ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));

			//	int32 outId = 0;
			//	SQLLEN outIdLen = 0;
			//	ASSERT_CRASH(dbConn->BindCol(1, &outId, &outIdLen));

			//	int32 outGold = 0;
			//	SQLLEN outGoldLen = 0;
			//	ASSERT_CRASH(dbConn->BindCol(2, &outGold, &outGoldLen));

			//	WCHAR outName[100];
			//	SQLLEN outNameLen = 0;
			//	ASSERT_CRASH(dbConn->BindCol(3, outName, len32(outName), &outNameLen));

			//	TIMESTAMP_STRUCT outDate = {};
			//	SQLLEN outDateLen = 0;
			//	ASSERT_CRASH(dbConn->BindCol(4, &outDate, &outDateLen));


			//	ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold, name, createDate FROM Gold WHERE gold = (?)"));*/
			//}


			//wcout.imbue(locale("kor"));
			//while (dbConn->Fetch())
			//{
			//	wcout << "id : " << outId << " gold : " << outGold << " name : " << outName << endl;
			//	wcout << "Date : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
			//}
			//GDBConnectionPool->Push(dbConn);
		}
	}



	//
	GSessionManager = new GameSessionManager();
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		1
	);

	ASSERT_CRASH(service->Start());

	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			}
		);
	}

	GThreadManager->Join();

	return 0;
}