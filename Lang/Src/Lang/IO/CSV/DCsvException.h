#pragma once


class DCsvException : public DRuntimeException
{
public:
	DString8 ErrorPartString;

	DCsvException(DStringView8 InMessage);

	DCsvException(DStringView8 InMessage, const DString8& FullString, cint Index);

};