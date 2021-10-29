#include "Engine/pch.h"
#include "DWavefrontException.h"


DWavefrontException::DWavefrontException(DStringView8 InMessage) : 
    DRuntimeException(("WavefronException: " + InMessage).CString())
{
}


DWavefrontException::DWavefrontException(DStringView8 InMessage, const DString8& FullString, int32 Index) : 
    DRuntimeException(("WavefronException: " + InMessage).CString())
{
    this->ErrorPartString = FullString.SubstrRange(
        FMath::Max(0, Index), 
        FMath::Min(Index + 50, FullString.Length())
    );
}
