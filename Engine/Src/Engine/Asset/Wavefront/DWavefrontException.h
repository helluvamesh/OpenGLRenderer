#pragma once


class DWavefrontException : public DRuntimeException
{
public:
	DString8 ErrorPartString;
	
	DWavefrontException(DStringView8 InMessage);

	DWavefrontException(DStringView8 InMessage, const DString8& FullString, int32 Index);
};