#include "pch.h"
#include "SJsonConvert.h"


void SJsonConvert<bool>::AppendItemToJsonString(bool Value, DString8& JStr)
{
    JStr.Append(
        Value? "true" : "false"
    );
}


cint SJsonConvert<bool>::JsonStringToItem(const DString8& JStr, cint Start, bool& Result)
{
    cint i = SJsonUtils::FindStringThruWhitespace(JStr, Start, SJsonUtils::FALSE_STR);
    if (i != -1)
    {
        Result = false;
        return i + SJsonUtils::FALSE_STR.Length();
    }
    else
    {
        i = SJsonUtils::FindStringThruWhitespace(JStr, Start, SJsonUtils::TRUE_STR);
        if (i != -1)
        {
            Result = true;
            return i + SJsonUtils::TRUE_STR.Length();
        }
        else
        {
            throw DJsonException("failed to parse boolean", JStr, Start);
        }
    }
}


cint SJsonConvert<UINT8_ENUM>::JsonStringToItem(DEnumClass* EnClass, const DString8& JStr, cint Start, uint8& Result)
{
    DString8 itemName;
    cint end = SJsonConvert<DString8>::JsonStringToItem(JStr, Start, itemName);

    if (EnClass)
    {
        Result = EnClass->GetItemByName(itemName);
    }
    else
    {
        Result = 0;
    }

    return end;
}


void SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(DEnumClass* EnClass, uint8 Value, DString8& JStr)
{
    if (EnClass)
    {
        const DString8& itemName = EnClass->GetItemName(Value);
        SJsonConvert<DString8>::AppendItemToJsonString(itemName, JStr);
    }
    else
    {
        DString8 s;
        s.Append('\"');
        SJsonUtils::NumberFormatter.AppendTo(Value, s);
        s.Append('\"');
        JStr.Append(s);
    }
}


void SJsonConvert<int32>::AppendItemToJsonString(int32 Value, DString8& JStr)
{
    SJsonUtils::NumberFormatter.AppendTo(Value, JStr);
}


cint SJsonConvert<int32>::JsonStringToItem(const DString8& JStr, cint Start, int32& Result)
{
    Result = DString8::ParseInt32(JStr.ViewRange(Start, JStr.Length()));
    return SJsonUtils::FindEndOfNumberString(JStr, Start);
}


void SJsonConvert<int64>::AppendItemToJsonString(int64 Value, DString8& JStr)
{
    SJsonUtils::NumberFormatter.AppendTo(Value, JStr);
}


cint SJsonConvert<int64>::JsonStringToItem(const DString8& JStr, cint Start, int64& Result)
{
    Result = DString8::ParseInt64(JStr.ViewRange(Start, JStr.Length()));
    return SJsonUtils::FindEndOfNumberString(JStr, Start);
}


void SJsonConvert<float>::AppendItemToJsonString(float Value, DString8& JStr)
{
    SJsonUtils::NumberFormatter.AppendTo(Value, JStr);
}


cint SJsonConvert<float>::JsonStringToItem(const DString8& JStr, cint Start, float& Result)
{
    Result = DString8::ParseFloat(JStr.ViewRange(Start, JStr.Length()));
    return SJsonUtils::FindEndOfNumberString(JStr, Start);
}


void SJsonConvert<double>::AppendItemToJsonString(double Value, DString8& JStr)
{
    SJsonUtils::NumberFormatter.AppendTo(Value, JStr);
}


cint SJsonConvert<double>::JsonStringToItem(const DString8& JStr, cint Start, double& Result)
{
    Result = DString8::ParseDouble(JStr.ViewRange(Start, JStr.Length()));
    return SJsonUtils::FindEndOfNumberString(JStr, Start);
}


void SJsonConvert<DString8>::AppendItemToJsonString(const DString8& Str, DString8& JStr)
{
    JStr.Append('\"');
    cint len = Str.Length();
    for (cint i = 0; i < len; ++i)
    {
        char8 c = Str[i];
        switch (c)
        {
        case '\"': JStr.Append("\\\""); break;
        case '\\': JStr.Append("\\\\"); break;
        //case '/': JStr.Append("\\/"); break;  some JSON exporters escape '/', could be an option
        case '\b': JStr.Append("\\b"); break;
        case '\f': JStr.Append("\\f"); break;
        case '\n': JStr.Append("\\n"); break;
        case '\r': JStr.Append("\\r"); break;
        case '\t': JStr.Append("\\t"); break;
        default: JStr.Append(c);
        }
    }
    JStr.Append('\"');
}


cint SJsonConvert<DString8>::JsonStringToItem(const DString8& JStr, cint Start, DString8& OutStr)
{
    bool prevCharWasEscape = false;
    bool foundOpeningQuote = false;

    cint end = -1;

    OutStr.Clear();

    cint len = JStr.Length();
    for (cint i = Start; i < len; i++)
    {
        char8 c = JStr[i];

        if (foundOpeningQuote == false)
        {
            if (c == '"') foundOpeningQuote = true;
            else if (SAscii::IsWhitespace(c) == false) break;
        }
        else
        {
            if (c == '\\')
            {
                if (prevCharWasEscape) 
                {
                    OutStr.Append('\\');
                    prevCharWasEscape = false;
                }
                else prevCharWasEscape = true;
            }
            else
            {
                // non-quote char
                if (c != '"') 
                {
                    if (prevCharWasEscape)
                    {
                        switch (c)
                        {
                        case u'n': OutStr.Append(u'\n'); break;
                        case u'r': OutStr.Append(u'\r'); break;
                        case u't': OutStr.Append(u'\t'); break;
                        case u'b': OutStr.Append(u'\b'); break;
                        case u'f': OutStr.Append(u'\f'); break;
                        case u'/': OutStr.Append(u'/'); break;
                        case u'u':
                        {
                            std::string s;
                            for (cint ii = 1; ii < 5; ii++)
                            {
                                s.append( 1, (char)JStr[i + ii] );
                            }
                            OutStr.Append( (char8)std::stoul(s, 0, 16) );
                            i += 4;  // skip four characters
                            break;
                        }
                        default: OutStr.Append(c);
                        }
                    }
                    else OutStr.Append(c);
                }
                // double quate
                else
                {
                    // opening quote
                    foundOpeningQuote = true;
                    // " without escape char
                    if (prevCharWasEscape == false)
                    {
                        end = i;
                        break;
                    }
                    // " with escape char
                    else OutStr.Append(u'\"');
                }

                prevCharWasEscape = false;
            }
        }
    }
    if (end >= 0)
    {
        return end + 1;
    }
    else
    {
        return -1;
    }
}