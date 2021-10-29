#include "pch.h"
#include "SJsonUtils.h"


const DString8 SJsonUtils::FALSE_STR = "false";
const DString8 SJsonUtils::TRUE_STR = "true";
const DString8 SJsonUtils::NULL_STR = "null";

DNumberFormat8 SJsonUtils::NumberFormatter;


bool SJsonUtils::StaticInit = [](){

    NumberFormatter.SetDecimalSeparator('.');
    NumberFormatter.SetMinFloatFractionDigits(1);
    NumberFormatter.SetMaxFloatFractionDigits(7);
    NumberFormatter.SetMinDoubleFractionDigits(1);
    NumberFormatter.SetMaxDoubleFractionDigits(15);
    NumberFormatter.ClearGroupSeparator();
    NumberFormatter.SetMinIntegralDigits(0);

    return true;
}();


// -------------------------- PARSE --------------------------

cint SJsonUtils::FindEndOfNumberString(const DString8& Str, cint Start)
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


cint SJsonUtils::FindCharThruWhitespace(const DString8& Str, cint Start, char8 CharToFind)
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


cint SJsonUtils::FindStringThruWhitespace(const DString8& Str, cint Start, const DString8& StrToFind)
{
    cint wordStart = -1;
    cint progress = 0;
    char charToFind = StrToFind[0];
    cint strToFindLen = StrToFind.Length();
    cint len = Str.Length();

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


cint SJsonUtils::ParseKeyword(const DString8& Str, cint Start, const DString8& Keyword)
{
    if (Str.Length() >= Start + Keyword.Length())
    {   
        cint len = Keyword.Length();
        for (cint i = 0; i < len; i++)
        {
            if ((Str[Start + i] == Keyword[i]) == false)
            {
                return -1;
            }
        }
        return Start + len;
    }
    return -1;
}
