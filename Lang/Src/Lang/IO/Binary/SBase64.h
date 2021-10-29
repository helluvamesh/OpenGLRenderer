#pragma once


class SBase64
{
private:

	static const char8 BASE64_CHARS[65];
	static const char8 BASE64_CHARS_URL[65];
	static const byte BYTE_LUT[128];

public:

	// hint: remove linebreaks?
	static bool StringToByteArray(DStringView8 Str, DArray<byte>& Bytes, bool URL);
	
private:

	// returns 255 as invalid value
	static byte IndexOfChar(char8 C);  
	// returns 255 as invalid value
	static byte IndexOfCharInURL(char8 C);

public:

	// unsafe but fast
	// hint: remove linebreaks?
	static void StringToByteArrayFast(DStringView8 Str, DArray<byte>& Bytes, bool URL);

	static bool IsValidBase64(DStringView8 Str, bool URL);
	static void MakeURLFriendly(DString8& Str);

public:

	static void ByteArrayToString(DArrayView<byte> Bytes, DString8& Str, bool URL);

};
