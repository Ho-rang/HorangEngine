#pragma once

// �������� ��� �� Manager�� �ʱ�ȭ ���

namespace Horang
{
	extern class ThreadManager* GThreadManager;

	// Manager ���� �Ҹ� ���� ����
	class CoreGlobal
	{
	public:
		CoreGlobal();
		~CoreGlobal();
	};
}