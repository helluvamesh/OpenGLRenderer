#pragma once

using DTimerDelegate = DSingleCastDelegate<void>;


class HTimerManager : public HObject
{
private:
	struct DTimerJob
	{
		uint64 Id = 1ULL;
		DTimerDelegate Callback;
		int64 DelayMS = 0LL;
		int64 IntervalMS = 0LL;
		int64 StartMS = 0LL;
		bool Expired = false;
		bool PastInitialDelay = false;
	};

	DMap<uint64, DTimerJob> Jobs;  // map for fast removal
	DArray<DTimerJob*> JobArray;  // array for fast iteration
	uint64 LastAssignedId = 0ULL;

public:
	HTimerManager();

	uint64 ScheduleOnce(float DelaySec, const DTimerDelegate& Callback);
	uint64 ScheduleInterval(float DelaySec, float IntervalSec, const DTimerDelegate& Callback);
	uint64 ScheduleOnceMS(int64 DelayMS, const DTimerDelegate& Callback);
	uint64 ScheduleIntervalMS(int64 DelayMS, int64 IntervalMS, const DTimerDelegate& Callback);
	void Cancel(uint64 Id);
	bool IsValid(uint64 Id) const;
	void Update();

};