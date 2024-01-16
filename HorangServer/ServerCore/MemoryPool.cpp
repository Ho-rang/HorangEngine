#include "pch.h"
#include "MemoryPool.h"

using namespace Horang;

/*
	MemoryPool
*/

Horang::MemoryPool::MemoryPool(int32 allocSize)
	: _allocSize(allocSize)
{
	::InitializeSListHead(&_header);
}

Horang::MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void Horang::MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->allocSize = 0;

	// Pool�� �޸� �ݳ�
	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_useCount.fetch_sub(1);
	_reserveCount.fetch_add(1);
}

MemoryHeader* Horang::MemoryPool::Pop()
{
	// Pool ���� �ϳ� ������

	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// ������ ���� �����
	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
	}

	_useCount.fetch_add(1);
	_reserveCount.fetch_sub(1);

	return memory;
}
