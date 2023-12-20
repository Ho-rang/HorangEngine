#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

// Manager ���� �Ҹ� ���� ����
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
