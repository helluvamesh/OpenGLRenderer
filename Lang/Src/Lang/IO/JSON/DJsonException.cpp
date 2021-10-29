#include "pch.h"
#include "DJsonException.h"


DJsonException::DJsonException(DStringView8 InMessage) : 
    DRuntimeException(("JsonException: " + InMessage).CString())
{

}


DJsonException::DJsonException(DStringView8 InMessage, const DString8& FullString, cint Index) :
    DRuntimeException(("JsonException: " + InMessage).CString())
{
    this->ErrorPartString = FullString.SubstrRange(
        SCounterUtil::Max(0, Index), 
        SCounterUtil::Min(Index + 50, FullString.Length())
    );
}