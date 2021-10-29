#pragma once


class DJsonException : public DRuntimeException
{
public:
	DString8 ErrorPartString;

	DJsonException(DStringView8 InMessage);

	DJsonException(DStringView8 InMessage, const DString8& FullString, cint Index);

};