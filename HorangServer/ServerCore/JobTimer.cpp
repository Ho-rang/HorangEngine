#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"
#include "Job.h"

using namespace Horang;

void Horang::JobTimer::Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, Horang::JobRef job)
{
	const auto executeTick = tickAfter + ::GetTickCount64();
	auto jobData = Horang::ObjectPool<JobData>::Pop(owner, job);

	WRITE_LOCK;

	_jobs.push(TimerJob{ executeTick, jobData });
}

void Horang::JobTimer::Distribute(uint64 now)
{
	if (_distributing.exchange(true) == true)
		return;

	Vector<TimerJob> jobs;

	{
		WRITE_LOCK;

		while (!_jobs.empty())
		{
			const auto& job = _jobs.top();
			if (now < job.executeTick)
				break;

			jobs.push_back(job);
			_jobs.pop();
		}
	}

	for (auto& job : jobs)
	{
		if (auto owner = job.jobData->owner.lock())
			owner->Push(std::move(job.jobData->job), true);

		Horang::ObjectPool<JobData>::Push(job.jobData);
	}

	_distributing.store(false);
}

void Horang::JobTimer::Clear()
{
	WRITE_LOCK;

	while (!_jobs.empty())
	{
		Horang::ObjectPool<JobData>::Push(_jobs.top().jobData);
		_jobs.pop();
	}
}
