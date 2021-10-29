#pragma once


class SCsvUtil
{
public:

    static cint FindCharThruWhitespace(const DString8& Str, cint Start, char8 CharToFind);

    static cint FindStringThruWhitespace(const DString8& Str, cint Start, const DString8& StrToFind);

    static cint FindEndOfNumberString(const DString8& Str, cint Start);

};
