#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

using namespace Horang;
/*
	Memory
*/

Horang::Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
	for (size = 1024; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
	for (size = 2048; size <= 4069; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

Horang::Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
	{
		delete pool;
	}

	_pools.clear();
}

void* Horang::Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;

	const int32 allocSize = size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 그냥 할당
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// 메모리 풀에서 꺼내오기
		header = _poolTable[allocSize]->Pop();
	}
#endif 

	return MemoryHeader::AttachHeader(header, allocSize);
}

void Horang::Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		::_aligned_free(header);
	}
	else
	{
		// 메모리 풀에 반납
		_poolTable[allocSize]->Push(header);
	}
#endif // _STOMP
}
