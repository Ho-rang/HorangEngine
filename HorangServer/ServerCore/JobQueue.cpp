#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void Horang::JobQueue::DoAsyncJob(Horang::JobRef&& job)
{
	this->Push(std::move(job));
}

void Horang::JobQueue::Push(Horang::JobRef job, bool pushOnly /*= false*/)
{
	const int32 prevJobCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	if (prevJobCount == 0)
	{
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			this->Execute();
		}
		else
		{
			GGlobalQueue->Push(this->shared_from_this());
		}
	}
}

void Horang::JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<Horang::JobRef> jobs;
		_jobs.PopAll(jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());

		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const auto now = ::GetTickCount64();
		if (now > LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			GGlobalQueue->Push(this->shared_from_this());
			return;
		}
	}
}
