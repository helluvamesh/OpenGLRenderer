#include "pch.h"
#include "HTimerManager.h"


HTimerManager::HTimerManager()
{
	this->Jobs.Reserve(100);
	this->JobArray.Reserve(100);
}


uint64 HTimerManager::ScheduleOnce(float DelaySec, const DTimerDelegate& Callback)
{
	uint64 id = ++this->LastAssignedId;
	DTimerJob& job = this->Jobs.AddDefault(id);
	job.Callback = Callback;
	job.DelayMS = (int64)((double)DelaySec * 1000.0);
	job.StartMS = SClock::GetEpochMS();
	job.Id = id;
	this->JobArray.Add(&job);
	return id;
}


uint64 HTimerManager::ScheduleInterval(float DelaySec, float IntervalSec, const DTimerDelegate& Callback)
{
	uint64 id = ++this->LastAssignedId;
	DTimerJob& job = this->Jobs.AddDefault(id);
	job.Callback = Callback;
	job.DelayMS = (int64)((double)DelaySec * 1000.0);
	job.IntervalMS = (int64)((double)IntervalSec * 1000.0);
	job.StartMS = SClock::GetEpochMS();
	job.Id = id;
	this->JobArray.Add(&job);
	return id;
}


uint64 HTimerManager::ScheduleOnceMS(int64 DelayMS, const DTimerDelegate& Callback)
{
	uint64 id = ++this->LastAssignedId;
	DTimerJob& job = this->Jobs.AddDefault(id);
	job.Callback = Callback;
	job.DelayMS = DelayMS;
	job.StartMS = SClock::GetEpochMS();
	job.Id = id;
	this->JobArray.Add(&job);
	return id;
}


uint64 HTimerManager::ScheduleIntervalMS(int64 DelayMS, int64 IntervalMS, const DTimerDelegate& Callback)
{
	uint64 id = ++this->LastAssignedId;
	DTimerJob& job = this->Jobs.AddDefault(id);
	job.Callback = Callback;
	job.DelayMS = DelayMS;
	job.IntervalMS = IntervalMS;
	job.StartMS = SClock::GetEpochMS();
	job.Id = id;
	this->JobArray.Add(&job);
	return id;
}


void HTimerManager::Update()
{
	// remove expired jobs
	this->JobArray.RemoveIf([this](DTimerJob* job, cint index) {

		if (job->Expired)
		{
			this->Jobs.Remove(job->Id);
			return true;
		}
		else
		{
			return false;
		}
	});

	// execute jobs
	int64 now = SClock::GetEpochMS();
	
	for (DTimerJob* job : this->JobArray)
	{
		// is looping
		if (job->IntervalMS > 0)
		{
			int64 limit = job->PastInitialDelay? job->IntervalMS : job->DelayMS;
			if (now - job->StartMS >= limit)
			{
				job->PastInitialDelay = true;
				job->StartMS = now;
				job->Callback.Call();
			}
		}
		else
		{
			if (now - job->StartMS >= job->DelayMS)
			{
				job->Expired = true;
				job->Callback.Call();
			}
		}
	}
}


void HTimerManager::Cancel(uint64 Id)
{
	DTimerJob* job = this->Jobs.Find(Id);
	if (job)
	{
		job->Expired = true;
	}
}


bool HTimerManager::IsValid(uint64 Id) const
{
	const DTimerJob* job = this->Jobs.Find(Id);
	return job && job->Expired == false;
}
