#pragma once

#include "DRuntimeException.h"


class DIndexException : public DRuntimeException
{
public:
	DIndexException(cint Index, cint Length);
	DIndexException(cint RangeStart, cint RangeStop, cint Length);
	DIndexException(cint Index, cint RangeStart, cint RangeStop, bool);
	DIndexException(const char* InMessage);

};