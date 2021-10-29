#include "pch.h"
#include "TChar.h"
#include "unicode\uchar.h"
#include "unicode\ustring.h"
#include "unicode\coll.h"


// ------------ TChar ------------


tchar TChar::ToLower(tchar Char)
{
	return u_tolower(Char);
}


tchar TChar::ToUpper(tchar Char)
{
	return u_toupper(Char);
}


bool TChar::IsLower(tchar Char)
{
	return u_islower(Char);
}


bool TChar::IsUpper(tchar Char)
{
	return u_isupper(Char);
}


bool TChar::IsBlank(tchar Char)
{
	return u_isblank(Char);
}


bool TChar::IsDigit(tchar Char)
{
	return u_isdigit(Char);
}


bool TChar::IsLetter(tchar Char)
{
	return u_isalpha(Char);
}


bool TChar::IsLetterOrDigit(tchar Char)
{
	return u_isalnum(Char);
}


bool TChar::IsPunctuation(tchar Char)
{
	return u_ispunct(Char);
}


bool TChar::IsSpace(tchar Char)
{
	return u_isspace(Char);
}


bool TChar::IsWhitespace(tchar Char)
{
	return u_isWhitespace(Char);
}


cint TChar::Compare(tchar A, tchar B)
{
	return u_strCompare(&A, 1, &B, 1, false);
}


cint TChar::CompareIgnoreCase(tchar A, tchar B)
{
	A = u_tolower(A);
	B = u_tolower(B);
	return u_strCompare(&A, 1, &B, 1, false);
}


void TChar::ToLowerString(const tchar* InBuffer, cint InLength, tchar* OutBuffer)
{
	for (cint i = 0; i < InLength; i++)
	{
		OutBuffer[i] = u_tolower(InBuffer[i]);
	}
}


void TChar::ToUpperString(const tchar* InBuffer, cint InLength, tchar* OutBuffer)
{
	for (cint i = 0; i < InLength; i++)
	{
		OutBuffer[i] = u_toupper(InBuffer[i]);
	}
}


cint TChar::CompareStrings(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB)
{
	return u_strCompare(BufferA, LengthA, BufferB, LengthB, false);
}


cint TChar::CompareStringsIgnoreCase(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB)
{
	cint overlap = SCounterUtil::Min(LengthA, LengthB);
	for (cint i = 0; i < overlap; i++)
	{
		tchar a = u_tolower(BufferA[i]);
		tchar b = u_tolower(BufferB[i]);
		cint cmp = u_strCompare(&a, 1, &b, 1, false);
		if (cmp != 0)
		{
			return cmp;
		}
	}
	return LengthA - LengthB;
}


bool TChar::EqualStrings(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB)
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


bool TChar::EqualStringsIgnoreCase(const tchar* BufferA, cint LengthA, const tchar* BufferB, cint LengthB)
{
	if (LengthA == LengthB)
	{
		for (cint i = 0; i < LengthA; i++)
		{
			if (u_tolower(BufferA[i]) != u_tolower(BufferB[i]))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}