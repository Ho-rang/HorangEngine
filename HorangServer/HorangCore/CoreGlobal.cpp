#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

// Manager 积己 家戈 鉴辑 包府
namespace Horang
{
	class CoreGlobal
	{
	public:
		CoreGlobal()
		{
			GThreadManager = new ThreadManager();
		}
		~CoreGlobal()
		{
			delete GThreadManager;
		}
	} GCoreGlobal;

	ThreadManager* Horang::GThreadManager = nullptr;
}
