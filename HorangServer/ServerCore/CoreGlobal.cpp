#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "DBConnectionPool.h"

ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
SendBufferManager* GSendBufferManager = nullptr;


DeadLockProfiler* GDeadLockProfiler = nullptr;

DBConnectionPool* GDBConnectionPool = nullptr;

class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();

private:
} GCoreGlobal;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
	GMemory = new Memory();
	GSendBufferManager = new SendBufferManager();


	GDeadLockProfiler = new DeadLockProfiler();

	GDBConnectionPool = new DBConnectionPool();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GMemory;
	delete GSendBufferManager;


	delete GDeadLockProfiler;
	
	delete GDBConnectionPool;

	SocketUtils::Clear();
}

