#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "DBConnectionPool.h"
#include "GlobalQueue.h"
#include "JobTimer.h"

using namespace Horang;

Horang::ThreadManager* GThreadManager = nullptr;
Horang::Memory* GMemory = nullptr;
Horang::SendBufferManager* GSendBufferManager = nullptr;

Horang::GlobalQueue* GGlobalQueue = nullptr;
Horang::JobTimer* GJobTimer = nullptr;

Horang::DeadLockProfiler* GDeadLockProfiler = nullptr;

Horang::DBConnectionPool* GDBConnectionPool = nullptr;

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

	GGlobalQueue = new GlobalQueue();
	GJobTimer = new JobTimer();

	GDeadLockProfiler = new DeadLockProfiler();

	GDBConnectionPool = new DBConnectionPool();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GSendBufferManager;

	delete GGlobalQueue;
	delete GJobTimer;

	delete GDBConnectionPool;
	delete GMemory;

	delete GDeadLockProfiler;

	SocketUtils::Clear();
}
