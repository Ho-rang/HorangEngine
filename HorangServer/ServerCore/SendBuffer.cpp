#include "pch.h"
#include "SendBuffer.h"

using namespace Horang;

/*
	SendBuffer
*/

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
	:_owner(owner), _buffer(buffer), _allocSize(allocSize)
{

}

SendBuffer::~SendBuffer()
{

}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}

/*
	SendBufferChunk
*/

SendBufferChunk::SendBufferChunk()
{

}

SendBufferChunk::~SendBufferChunk()
{

}

void SendBufferChunk::Reset()
{
	_open = false;
	_useSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_useSize += writeSize;
}

/*
	SendBufferManager
*/

SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	//cout << "Thread : " <<LThreadId << "  " << "Free : " << LSendBufferChunk->FreeSize() << endl;

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	//cout << "Pop SendBufferChunk" << endl;

	{
		WRITE_LOCK;
		if (_sendBufferChunk.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunk.back();
			_sendBufferChunk.pop_back();
			return sendBufferChunk;
		}
	}

	return SendBufferChunkRef(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunk.push_back(buffer);

}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	//cout << "PushGloval SendBufferChunk" << endl;

	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}
