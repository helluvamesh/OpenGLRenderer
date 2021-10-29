#include "pch.h"
#include "SCsvUtils.h"


cint SCsvUtil::FindCharThruWhitespace(const DString8& Str, cint Start, char8 CharToFind)
{
    cint len = Str.Length();
    for (cint i = Start; i < len; i++)
    {
        char8 c = Str[i];
        if (SAscii::IsWhitespace(c)) continue;
        else if (c == CharToFind) return i;
        else return -1;
    }
    return -1;
}


cint SCsvUtil::FindStringThruWhitespace(const DString8& Str, cint Start, const DString8& StrToFind)
{
    cint wordStart = -1;
    cint progress = 0;
    char charToFind = StrToFind[0];
    cint strToFindLen = StrToFind.Length();
    cint len = StrToFind.Length();

    for (cint i = Start; i < len; i++)
    {
        char8 c = Str[i];
        if (SAscii::IsWhitespace(c)) continue;
        else if (c == charToFind)
        {
            if (wordStart == -1) wordStart = i;
            if (progress == strToFindLen - 1) return wordStart;
            else
            {
                progress++;
                charToFind = StrToFind[progress];
            }
        }
        else return -1;
    }
    return -1;
}


cint SCsvUtil::FindEndOfNumberString(const DString8& Str, cint Start)
{
    cint len = Str.Length();
    bool foundFirstDigit = false;
    for (cint i = Start; i < len; ++i)
    {
        char8 c = Str[i];
        // this will only return correct end index if the number string is correctly formed, this does not validate it
        if (SAscii::IsDigit(c) || c == '-' || c == '.' || c == '+' || c == 'E' || c == 'e')
        {
            foundFirstDigit = true;
        }
        else if (SAscii::IsWhitespace(c))
        {
            if (foundFirstDigit)
            {
                return i;
            }
        }
        else
        {
            return i;
        }
    }
    return len;
}
