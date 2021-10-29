#pragma once


class SJsonUtils
{
public:
    static const DString8 FALSE_STR;
    static const DString8 TRUE_STR;
    static const DString8 NULL_STR;
    static DNumberFormat8 NumberFormatter;

    static bool StaticInit;

    // ------------------- PARSE ----------------------

    static cint FindEndOfNumberString(const DString8& Str, cint Start);

    static cint FindCharThruWhitespace(const DString8& Str, cint Start, char8 CharToFind);

    static cint FindStringThruWhitespace(const DString8& Str, cint Start, const DString8& StrToFind);

    static cint ParseKeyword(const DString8& Str, cint Start, const DString8& Keyword);
    
};