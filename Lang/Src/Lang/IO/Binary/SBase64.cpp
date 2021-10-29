#include "pch.h"
#include "SBase64.h"


const char8 SBase64::BASE64_CHARS[65] =

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/";


const char8 SBase64::BASE64_CHARS_URL[65] {

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_" 
};


const byte SBase64::BYTE_LUT[128] = {

    // 8 * 16

    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,   0, 255, 255, 255,
    255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
    41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255
};


bool SBase64::StringToByteArray(DStringView8 Str, DArray<byte>& Bytes, bool URL)
{
    const char8* chars64 = URL? BASE64_CHARS_URL : BASE64_CHARS;
    const char8 paddingChar = URL? '.' : '=';
    byte (*indexOf)(char8) = URL? SBase64::IndexOfCharInURL : SBase64::IndexOfChar;

    cint strLen = Str.Length();
    cint byteLen = strLen / 4 * 3;
    
    Bytes.Reallocate(byteLen, true);

    if (Str.IsValid() == false) return false;

    const char8* str = Str.Data();
    byte* bytes = Bytes.GetDataUnsafe();

    cint str_i = 0;
    cint byte_i = 0;

    while (str_i < strLen) 
    {
        // char 0
        byte char_0_i = indexOf(str[str_i]);
        if (char_0_i == 255) return false;

        // char 1
        byte char_1_i = indexOf(str[str_i + 1]);
        if (char_1_i == 255) return false;
        bytes[byte_i++] = (char_0_i << 2 ) + ( (char_1_i & 0x30 ) >> 4 );

        if (str_i + 2 < strLen &&  // check for data that is not padded
            str[str_i + 2] != paddingChar)
        {
            // char 2 (optional in the last chunk)
            byte char_2_i = indexOf(str[str_i + 2]);
            if (char_2_i == 255) return false;
            bytes[byte_i++] = ( (char_1_i & 0x0f) << 4 ) + ( (char_2_i & 0x3c) >> 2 );

            if (str_i + 3 < strLen &&
                str[str_i + 3] != paddingChar)
            {
                // char 3 (optional int the last chunk)
                byte char_3_i = indexOf(str[str_i + 3]);
                if (char_3_i == 255) return false;
                bytes[byte_i++] = ( (char_2_i & 0x03) << 6 ) + char_3_i;
            }
        }

        str_i += 4;
    }

    return true;
}


byte SBase64::IndexOfChar(char8 C) 
{
    if      (C >= 'A' && C <= 'Z') return C - 'A';
    else if (C >= 'a' && C <= 'z') return C - 'a' + ('Z' - 'A') + 1;
    else if (C >= '0' && C <= '9') return C - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (C == '+')             return 62;
    else if (C == '/')             return 63;
    else                           return 0;
}


byte SBase64::IndexOfCharInURL(char8 C) 
{
    if      (C >= 'A' && C <= 'Z') return C - 'A';
    else if (C >= 'a' && C <= 'z') return C - 'a' + ('Z' - 'A') + 1;
    else if (C >= '0' && C <= '9') return C - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (C == '-')             return 62;
    else if (C == '_')             return 63;
    else                           return 0;
}


void SBase64::StringToByteArrayFast(DStringView8 Str, DArray<byte>& Bytes, bool URL)
{
    const char8* chars64 = URL? BASE64_CHARS_URL : BASE64_CHARS;
    const char8 paddingChar = URL? '.' : '=';
    
    cint strLen = Str.Length();
    cint byteLen = strLen / 4 * 3;

    Bytes.Reallocate(byteLen, true);

    if (Str.IsValid() == false) return;

    const char8* str = Str.Data();
    byte* bytes = Bytes.GetDataUnsafe();

    cint str_i = 0;
    cint byte_i = 0;

    while (str_i < strLen) 
    {
        // char 0
        byte char_0_i = BYTE_LUT[str[str_i]];

        // char 1
        byte char_1_i = BYTE_LUT[str[str_i + 1]];
        bytes[byte_i++] = ( char_0_i << 2 ) + ( (char_1_i & 0x30 ) >> 4 );

        if (str_i + 2 < strLen &&  // check for data that is not padded
            str[str_i + 2] != paddingChar)
        {
            // char 2 (optional in the last chunk)
            byte char_2_i = BYTE_LUT[str[str_i + 2]];
            bytes[byte_i++] = ( (char_1_i & 0x0f) << 4 ) + ( (char_2_i & 0x3c) >> 2 );

            if (str_i + 3 < strLen &&
                str[str_i + 3] != paddingChar)
            {
                // char 3 (optional int the last chunk)
                byte char_3_i = BYTE_LUT[str[str_i + 3]];
                bytes[byte_i++] = ( (char_2_i & 0x03) << 6 ) + char_3_i;
            }
        }

        str_i += 4;
    }
}


bool SBase64::IsValidBase64(DStringView8 Str, bool URL)
{
    const char8* data = Str.Data();
    if (URL)
    {
        for (cint i = 0; i < Str.Length(); ++i)
        {
            char8 c = data[i];
            if ((
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '-' ||
                c == '_' ||
                c == '.'
                ) == false) return false;
        }
    }
    else
    {
        for (cint i = 0; i < Str.Length(); ++i)
        {
            char8 c = data[i];
            if ((
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '+' ||
                c == '/' ||
                c == '='
            ) == false) return false;
        }
    }

    return true;
}


void SBase64::MakeURLFriendly(DString8& Str)
{
    char8* buf = Str.GetDataUnsafe();
    cint len = Str.Length();
    for (cint i = 0; i < len; ++i)
    {
        char8 c = buf[i];
        if (c == '+') buf[i] = '-';
        else if (c == '/') buf[i] = '_';
        else if (c == '=') buf[i] = '.';
    }
}


void SBase64::ByteArrayToString(DArrayView<byte> Bytes, DString8& Str, bool URL)
{
    const char8* chars64 = URL? BASE64_CHARS_URL : BASE64_CHARS;
    const char8 paddingChar = URL? '.' : '=';

    cint byteLen = Bytes.Length();
    cint strLen = (byteLen + 2) / 3 * 4;

    Str.Reallocate(strLen, true);

    if (Bytes.IsValid() == false) return;

    const byte* bytes = Bytes.Data();
    char8* str = Str.GetDataUnsafe();

    cint byte_i = 0;
    cint str_i = 0;

    while (byte_i < byteLen)
    {
        str[str_i++] = chars64[(bytes[byte_i + 0] & 0xfc) >> 2];

        if (byte_i + 1 < byteLen) 
        {
            str[str_i++] = chars64[((bytes[byte_i + 0] & 0x03) << 4) + ((bytes[byte_i + 1] & 0xf0) >> 4)];
            if (byte_i + 2 < byteLen) 
            {
                str[str_i++] = chars64[((bytes[byte_i + 1] & 0x0f) << 2) + ((bytes[byte_i + 2] & 0xc0) >> 6)];
                str[str_i++] = chars64[bytes[byte_i + 2] & 0x3f];
            }
            else 
            {
                str[str_i++] = chars64[(bytes[byte_i + 1] & 0x0f) << 2];
                str[str_i++] = paddingChar;
            }
        }
        else 
        {
            str[str_i++] = chars64[(bytes[byte_i] & 0x03) << 4];
            str[str_i++] = paddingChar;
            str[str_i++] = paddingChar;
        }

        byte_i += 3;
    }
    
}
