#pragma once


class DRuntimeException : public std::exception 
{
public:

    DRuntimeException(const char* NullTerminatedMessage);
    
};
