#include "pch.h"
#include "DDateTime.h"
#include <time.h>


// ------------------- DDate ------------------


DDate::DDate() : 
	Year(0), Month(0), Day(0)
{
}


DDate::DDate(int32 InYear, int32 InMonth, int32 InDay) : 
	Year(InYear), Month(InMonth), Day(InDay)
{
}


DString DDate::ToString()
{
	return DString::Format(
		TX("{0}.{1}.{2}"), 
		{ this->Year, this->Month, this->Day }
	);
}


// ------------------- DTime ------------------


DTime::DTime() :
	Hour(0), Minute(0), Second(0), Millisecond(0)
{
}


DTime::DTime(int32 InHour, int32 InMinute, int32 InSecond, int32 InMillisecond) : 
	Hour(InHour), Minute(InMinute), Second(InSecond), Millisecond(InMillisecond)
{
}


DString DTime::ToString()
{
	return DString::Format(
		TX("{3}:{4}:{5}.{6}"), 
		{ this->Hour, this->Minute, this->Second, this->Millisecond }
	);
}


// ------------------- DDateTime ------------------


DDateTime::DDateTime() :
	Year(0), Month(0), Day(0), Hour(0), Minute(0), Second(0), Millisecond(0)
{
}


DDateTime::DDateTime(int32 InYear, int32 InMonth, int32 InDay, int32 InHour, int32 InMinute, int32 InSecond, int32 InMillisecond) :
	Year(InYear), Month(InMonth), Day(InDay), 
	Hour(InHour), Minute(InMinute), Second(InSecond), Millisecond(InMillisecond)
{
}


/*
1 sec =      1,000 ms
1 min =     60,000 ms
1 hr  =  3,600,000 ms
1 day = 86,400,000 ms
*/

DDateTime DDateTime::UTCFromEpochMicro(int64 EpochMicro)
{
	DDateTime d;

	const time_t et = (time_t)(EpochMicro / 1000000);
	tm t; 
	gmtime_s(&t, &et);
	d.Year = t.tm_year + 1900;
	d.Month = t.tm_mon + 1;
	d.Day = t.tm_mday;
	d.Hour = t.tm_hour;
	d.Minute = t.tm_min;
	d.Second = t.tm_sec;
	d.Millisecond = (int32)((EpochMicro % 1000000) / 1000);

	return d;
}


DDateTime DDateTime::LocalFromEpochMicro(int64 EpochMicro)
{
	DDateTime d;

	const time_t et = (time_t)(EpochMicro / 1000000);
	tm t;
	localtime_s(&t, &et);
	d.Year = t.tm_year + 1900;
	d.Month = t.tm_mon + 1;
	d.Day = t.tm_mday;
	d.Hour = t.tm_hour;
	d.Minute = t.tm_min;
	d.Second = t.tm_sec;
	d.Millisecond = (int32)((EpochMicro % 1000000) / 1000);

	return d;
}


DDateTime DDateTime::UTCNow()
{
	return DDateTime::UTCFromEpochMicro(SClock::GetEpochMicro());
}


DDateTime DDateTime::LocalNow()
{
	return DDateTime::LocalFromEpochMicro(SClock::GetEpochMicro());
}


DDate DDateTime::ToDate()
{
	return DDate(this->Year, this->Month, this->Day);
}


DTime DDateTime::ToTime()
{
	return DTime(this->Hour, this->Month, this->Second, this->Millisecond);
}


DString DDateTime::ToString()
{
	return DString::Format(
		TX("{0}.{1}.{2} {3}:{4}:{5}.{6}"), 
		{ this->Year, this->Month, this->Day, this->Hour, this->Minute, this->Second, this->Millisecond }
	);
}


// -----------------------------------------------


REFLECT(DDate)
FIELD(Year)
FIELD(Month)
FIELD(Day)
REFLECT_END()


REFLECT(DTime)
FIELD(Hour)
FIELD(Minute)
FIELD(Second)
FIELD(Millisecond)
REFLECT_END()


REFLECT(DDateTime)
FIELD(Year)
FIELD(Month)
FIELD(Day)
FIELD(Hour)
FIELD(Minute)
FIELD(Second)
FIELD(Millisecond)
REFLECT_END()
