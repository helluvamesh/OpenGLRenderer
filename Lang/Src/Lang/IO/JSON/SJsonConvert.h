#pragma once

#include "DJsonException.h"
#include "SJsonUtils.h"


template<typename T>
class SJsonConvert { };


template<>
class SJsonConvert<bool>
{
public:
	static void AppendItemToJsonString(bool Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, bool& Result);
};


template<>
class SJsonConvert<UINT8_ENUM>
{
public:
	static void AppendItemToJsonString(DEnumClass* EnClass, uint8 Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(DEnumClass* EnClass, const DString8& JStr, cint Start, uint8& Result);
};


template<>
class SJsonConvert<int32>
{
public:
	static void AppendItemToJsonString(int32 Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, int32& Result);
};


template<>
class SJsonConvert<int64>
{
public:
	static void AppendItemToJsonString(int64 Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, int64& Result);
};


template<>
class SJsonConvert<float>
{
public:
	static void AppendItemToJsonString(float Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, float& Result);
};


template<>
class SJsonConvert<double>
{
public:
	static void AppendItemToJsonString(double Value, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, double& Result);
};


template<>
class SJsonConvert<DString8>
{
public:
	static void AppendItemToJsonString(const DString8& Str, DString8& JStr);
	// returns progress
	static cint JsonStringToItem(const DString8& JStr, cint Start, DString8& Result);
};
