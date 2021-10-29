#pragma once


class TChar
{
public:

	// -------- CHARACTER ---------

	static tchar ToLower(tchar Char);
	
	static tchar ToUpper(tchar Char);

	static bool IsLower(tchar Char);

	static bool IsUpper(tchar Char);

	static bool IsBlank(tchar Char);

	static bool IsDigit(tchar Char);
	
	static bool IsLetter(tchar Char);

	static bool IsLetterOrDigit(tchar Char);

	static bool IsPunctuation(tchar Char);

	static bool IsSpace(tchar Char);

	static bool IsWhitespace(tchar Char);
	
	static cint Compare(tchar A, tchar B);
	
	static cint CompareIgnoreCase(tchar A, tchar B);
	
	// ------------ STRING -----------

	static void ToLowerString(const tchar* InBuffer, cint InLength, tchar* OutBuffer);

	static void ToUpperString(const tchar* InBuffer, cint InLength, tchar* OutBuffer);

	static cint CompareStrings(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB);

	static cint CompareStringsIgnoreCase(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB);

	static bool EqualStrings(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB);

	static bool EqualStringsIgnoreCase(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB);

};