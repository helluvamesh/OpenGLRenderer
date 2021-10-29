#include "pch.h"
#include "DRuntimeException.h"


DRuntimeException::DRuntimeException(const char* NullTerminatedMessage) 
    : std::exception(NullTerminatedMessage)
{
    SLog::Error(NullTerminatedMessage);
}

