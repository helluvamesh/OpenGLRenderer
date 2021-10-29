#include "pch.h"
#include "DStrings.h"
#include "unicode\ustring.h"


void DStrings::UTF16ToUTF8(DStringView UTF16Buffer, DString8& OutUTF8String)
{
	if (UTF16Buffer.IsValid() == false)
	{
		return;
	}

	// allocate memory for output
	cint outLenReserved = UTF16Buffer.Length() * 3 + 1;
	OutUTF8String.Reallocate(outLenReserved, true);
	
	// do the conversion
	UErrorCode err = U_ZERO_ERROR;
	cint outLen = 0;
	u_strToUTF8WithSub(
		OutUTF8String.GetDataUnsafe(), outLenReserved, &outLen,
		UTF16Buffer.Data(), UTF16Buffer.Length(),
		0xFFFD, NULL, &err
	);

	OutUTF8String.OverwriteLengthUnsafe(outLen);

#ifdef LANG_DEBUG_CONTAINERS
	if (U_FAILURE(err)) 
	{
		SLog::Error(TX("Error while converting UTF-8 string to UTF16"));
		SLog::Error(u_errorName(err));
	}
#endif
}


DString8 DStrings::UTF16ToUTF8(DStringView UTF16Buffer)
{
	DString8 out;
	UTF16ToUTF8(UTF16Buffer, out);
	return out;
}


bool DStrings::AppendUTF16ToUTF8(DStringView UTF16Buffer, DString8& UTF8String)
{
	if (UTF16Buffer.IsValid() == false)
	{
		return true;
	}

	// allocate memory for output
	cint outLenReserved = UTF16Buffer.Length() * 3 + 1;
	UTF8String.ReserveExtra(outLenReserved);

	// do the conversion
	UErrorCode err = U_ZERO_ERROR;
	cint outLen = 0;
	u_strToUTF8WithSub(
		UTF8String.GetDataUnsafe() + UTF8String.Length(), outLenReserved, &outLen,
		UTF16Buffer.Data(), UTF16Buffer.Length(),
		0xFFFD, NULL, &err
	);

	if (U_FAILURE(err)) 
	{
#ifdef LANG_DEBUG_CONTAINERS
		SLog::Error(TX("Error while converting UTF-8 string to UTF16"));
		SLog::Error(u_errorName(err));
#endif
		UTF8String.OverwriteLengthUnsafe(UTF8String.Length());  // re-add null terminator
		return false;
	}
	else
	{
		// adjust the output string's size variable
		UTF8String.OverwriteLengthUnsafe(UTF8String.Length() + outLen);
		//out_UTF8.TrimCapacity();  // uncomment this to adjust the output string's buffer to size
		return true;
	}
}


void DStrings::UTF8ToUTF16(DStringView8 UTF8Buffer, DString& OutUTF16String)
{
	if (UTF8Buffer.IsValid() == false)
	{
		return;
	}

	// allocate memory for output
	cint outLenReserved = UTF8Buffer.Length();
	OutUTF16String.Reallocate(outLenReserved, true);

	// do the conversion
	UErrorCode err = U_ZERO_ERROR;
	cint outLen = 0;
	u_strFromUTF8WithSub(
		OutUTF16String.GetDataUnsafe(), outLenReserved, &outLen,
		UTF8Buffer.Data(), UTF8Buffer.Length(),
		0xfffd, NULL, &err
	);

	OutUTF16String.OverwriteLengthUnsafe(outLen);

#ifdef LANG_DEBUG_CONTAINERS
	if (U_FAILURE(err)) 
	{
		SLog::Error("Error while converting UTF-8 string to UTF16");
		SLog::Error(u_errorName(err));
	}
#endif
}


DString DStrings::UTF8ToUTF16(DStringView8 UTF8Buffer)
{
	DString out;
	UTF8ToUTF16(UTF8Buffer, out);
	return out;
}


DString DStrings::AsciiToUTF16(DStringView8 AsciiBuffer)
{
	cint len = AsciiBuffer.Length();
	const char8* arr = AsciiBuffer.Data();
	DString out_UTF16 (len, true);  // uninitialized array
	tchar* targetArr = out_UTF16.GetDataUnsafe();
	for (cint i = 0; i < len; i++)
	{
		targetArr[i] = (tchar)arr[i];
	}
	return out_UTF16;
}


DString8 DStrings::UTF16ToAscii(DStringView UTF16Buffer)
{
	cint len = UTF16Buffer.Length();
	const tchar* arr = UTF16Buffer.Data();
	DString8 out_Ascii (len, true);  // uninitialized array
	char8* targetArr = out_Ascii.GetDataUnsafe();
	for (cint i = 0; i < len; i++)
	{
		targetArr[i] = (char8)arr[i];
	}
	return out_Ascii;
}


std::wstring DStrings::DStringToSTDWideStr(DStringView DStr)
{
	std::wstring wideStr;
	cint len = DStr.Length();
	wideStr.resize(len);
	for (cint i = 0; i < len; i++)
	{
		wideStr[i] = (wchar_t)DStr[i];
	}
	return wideStr;
}
