#pragma once


class DStrings
{
public:

	static void UTF16ToUTF8(DStringView UTF16Buffer, DString8& OutUTF8String);
	static DString8 UTF16ToUTF8(DStringView UTF16Buffer);
	static bool AppendUTF16ToUTF8(DStringView UTF16Buffer, DString8& UTF8String);
	
	static void UTF8ToUTF16(DStringView8 UTF8Buffer, DString& OutUTF16String);
	static DString UTF8ToUTF16(DStringView8 UTF8Buffer);

	static DString AsciiToUTF16(DStringView8 AsciiBuffer);
	static DString8 UTF16ToAscii(DStringView UTF16Buffer);

	static std::wstring DStringToSTDWideStr(DStringView DStr);

};