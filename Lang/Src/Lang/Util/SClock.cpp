#include "pch.h"
#include "SClock.h"
#include <chrono>


std::chrono::system_clock _SystemClock;


int64 SClock::GetEpochMicro()
{
	return (int64)(_SystemClock.now().time_since_epoch().count() / 10.0);
}


int64 SClock::GetEpochMS()
{
	return (int64)(_SystemClock.now().time_since_epoch().count() / 10000.0);
}


double SClock::GetEpochSec()
{
	return _SystemClock.now().time_since_epoch().count() / 10000000.0;
}
