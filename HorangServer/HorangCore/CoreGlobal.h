#pragma once

// 전역으로 사용 할 Manager들 초기화 담당

namespace Horang
{
	extern class ThreadManager* GThreadManager;

	// Manager 생성 소멸 순서 관리
	class CoreGlobal
	{
	public:
		CoreGlobal();
		~CoreGlobal();
	};
}