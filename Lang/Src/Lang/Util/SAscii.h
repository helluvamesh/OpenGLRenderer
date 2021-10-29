#pragma once

#include "CoreTypes.h"
#include <string>


class SAscii
{
public:

	inline static char8 ToLower(char8 Char)
	{
		return std::tolower(Char);
	}

	inline static char8 ToUpper(char8 Char)
	{
		return std::toupper(Char);
	}

	inline static bool IsLower(char8 Char)
	{
		return std::islower(Char);
	}

	inline static bool IsUpper(char8 Char)
	{
		return std::isupper(Char);
	}

	inline static bool IsBlank(char8 Char)
	{
		return std::isblank(Char);
	}

	inline static bool IsDigit(char8 Char)
	{
		return std::isdigit(Char);
	}

	inline static bool IsLetter(char8 Char)
	{
		return std::isalpha(Char);
	}

	inline static bool IsLetterOrDigit(char8 Char)
	{
		return std::isalnum(Char);
	}

	inline static bool IsPunctuation(char8 Char)
	{
		return std::ispunct(Char);
	}

	inline static bool IsWhitespace(char8 Char)
	{
		return std::isspace(Char);
	}

	inline static cint Compare(char8 A, char8 B)
	{
		return A - B;
	}

	inline static cint CompareIgnoreCase(char8 A, char8 B)
	{
		return std::tolower(A) - std::tolower(B);
	}

	// ------------ STRING -----------

	static void ToLowerString(const char8* InBuffer, cint InLength, char8* OutBuffer);

	static void ToUpperString(const char8* InBuffer, cint InLength, char8* OutBuffer);

	static cint CompareStrings(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB);

	static cint CompareStringsIgnoreCase(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB);

	static bool EqualStrings(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB);
	
	static bool EqualStringsIgnoreCase(const char8* BufferA, cint LengthA, const char8* BufferB, cint LengthB);

};