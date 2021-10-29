#include "pch.h"
#include "SPrimitiveCopy.h"


bool SBinaryInfo::IsSystemLittleEndian()
{
    short int word = 0x0001;
    char* byte = (char*) &word;
    return byte[0];
}


bool SBinaryInfo::ShouldKeepEndiannes(bool LittleEndianBytes)
{
    bool LESystem = IsSystemLittleEndian();
    if (LittleEndianBytes)
    {
        return LESystem;
    }
    else
    {
        return LESystem == false;
    }
}
