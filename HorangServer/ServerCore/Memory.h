#pragma once
#include "Allocator.h"

class MemoryPool;

/*
	Memory
*/

class Memory
{
	enum
	{
		// ~1024 ���� 32����
		// ~2048 ���� 128 ����
		// ~4096 ���� 256 ����

		POOL_COUNT = (1024 / 32) * (1024 / 128) * (2048 / 256),
		MAX_ALLOC_SIZE = 4096,
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// �޸� ũ��  <-> �޸� Ǯ
	// 0(1)�� ������ ã�� ���� ���̺�
	
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};


template <typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new
	new(memory)Type(std::forward<Args>(args)...);

	return memory;
}

template <typename Type>
void xdelete(Type* obj)
{
	obj->~Type();

	PoolAllocator::Release(obj);
}

template <typename Type,typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{xnew<Type>(std::forward<Args>(args)...), xdelete<Type>};
}