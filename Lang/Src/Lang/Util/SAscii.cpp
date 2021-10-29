#include "pch.h"
#include "SAscii.h"


void SAscii::ToLowerString(const char8* InBuffer, cint InLength, char8* OutBuffer)
{
	for (cint i = 0; i < InLength; i++)
	{
		OutBuffer[i] = std::tolower(InBuffer[i]);
	}
}


void SAscii::ToUpperString(const char8* InBuffer, cint InLength, char8* OutBuffer)
{
	for (cint i = 0; i < InLength; i++)
	{
		OutBuffer[i] = std::toupper(InBuffer[i]);
	}
}


cint SAscii::CompareStrings(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB)
{
	cint overlap = SCounterUtil::Min(LengthA, LengthB);
	for (cint i = 0; i < overlap; i++)
	{
		char8 a = BufferA[i];
		char8 b = BufferB[i];
		if (a != b)
		{
			return a - b;
		}
	}
	return LengthA - LengthB;
}


cint SAscii::CompareStringsIgnoreCase(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB)
{
	cint overlap = SCounterUtil::Min(LengthA, LengthB);
	for (cint i = 0; i < overlap; i++)
	{
		char8 a = std::tolower(BufferA[i]);
		char8 b = std::tolower(BufferB[i]);
		if (a != b)
		{
			return a - b;
		}
	}
	return LengthA - LengthB;
}


bool SAscii::EqualStrings(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB)
{
	if (LengthA == LengthB)
	{
		for (cint i = 0; i < LengthA; i++)
		{
			if (BufferA[i] != BufferB[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}


bool SAscii::EqualStringsIgnoreCase(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB)
{
	if (LengthA == LengthB)
	{
		for (cint i = 0; i < LengthA; i++)
		{
			if (std::tolower(BufferA[i]) != std::tolower(BufferB[i]))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}