#pragma once


struct DDate : public DStruct
{
	REFLECT_H(DDate, DStruct)

public:

	int32 Year;
	int32 Month;
	int32 Day;

	DDate();
	DDate(int32 InYear, int32 InMonth, int32 InDay);

	DString ToString();

};


struct DTime : public DStruct
{
	REFLECT_H(DTime, DStruct)

public:

	int32 Hour;
	int32 Minute;
	int32 Second;
	int32 Millisecond;


	DTime();
	DTime(int32 InHour, int32 InMinute, int32 InSecond, int32 InMillisecond);

	DString ToString();

};


struct DDateTime : public DStruct
{
	REFLECT_H(DDateTime, DStruct)

public:

	int32 Year;
	int32 Month;
	int32 Day;

	int32 Hour;
	int32 Minute;
	int32 Second;
	int32 Millisecond;


	DDateTime();
	DDateTime(int32 InYear, int32 InMonth, int32 InDay, int32 InHour, int32 InMinute, int32 InSecond, int32 InMillisecond);

	static DDateTime UTCFromEpochMicro(int64 EpochMicro);
	static DDateTime LocalFromEpochMicro(int64 EpochMicro);

	static DDateTime UTCNow();
	static DDateTime LocalNow();

	DDate ToDate();
	DTime ToTime();

	DString ToString();

};
