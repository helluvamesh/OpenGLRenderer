#include "pch.h"
#include "DCsvException.h"


DCsvException::DCsvException(DStringView8 InMessage) :
    DRuntimeException(("CSVException: " + InMessage).CString())
{

}


DCsvException::DCsvException(DStringView8 InMessage, const DString8& FullString, cint Index) :
    DRuntimeException(("CSVException: " + InMessage).CString())
{
    this->ErrorPartString = FullString.SubstrRange(
        SCounterUtil::Max(0, Index), 
        SCounterUtil::Min(Index + 50, FullString.Length())
    );
}