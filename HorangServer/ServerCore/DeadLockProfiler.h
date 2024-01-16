#pragma once
#include <stack>
#include <map>
#include <vector>
#include <set>
#include <unordered_map>

namespace Horang
{
	/*
		DeadLockProfiler
	*/

	class DeadLockProfiler
	{
	public:
		void PushLock(const char* name);
		void PopLock(const char* name);
		void CheckCycle();

	private:
		void Dfs(int32 index);

	private:
		std::unordered_map<const char*, int32> _nameToId;
		std::unordered_map<int32, const char*> _idToName;

		std::map<int32, std::set<int32>> _lockHistory;

		Mutex _lock;

	private:
		std::vector<int32> _discoveredOrder; // ��尡 �߰ߵ� ���� ���
		int32 _discoveredCount = 0; // ��尡 �߰ߵ� ����
		std::vector<bool> _finished; // Dfs(i) ��°�� ����ƴ��� �Ǻ�
		std::vector<int32> _parent;
	};

}