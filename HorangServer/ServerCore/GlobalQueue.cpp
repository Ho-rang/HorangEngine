#include "pch.h"
#include "GlobalQueue.h"

Horang::GlobalQueue::GlobalQueue()
{

}

Horang::GlobalQueue::~GlobalQueue()
{

}

void Horang::GlobalQueue::Push(JobQueueRef jobQueue)
{
	_jobQueues.Push(jobQueue);
}

Horang::JobQueueRef Horang::GlobalQueue::Pop()
{
	return _jobQueues.Pop();
}
