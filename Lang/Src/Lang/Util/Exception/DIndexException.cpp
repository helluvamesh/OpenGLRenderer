#include "pch.h"
#include "DIndexException.h"


DIndexException::DIndexException(cint Index, cint Length)
	: DRuntimeException(
		(
			  std::string("Index Exception: index: ") 
			+ std::to_string(Index) 
			+ std::string(", length: ") 
			+ std::to_string(Length)
		).c_str()
	)
{
}


DIndexException::DIndexException(cint RangeStart, cint RangeStop, cint Length)
	:DRuntimeException(
		(
			  std::string("Range Out of Length Exception: start: ")
			+ std::to_string(RangeStart) 
			+ std::string(", stop: ") + std::to_string(RangeStop) 
			+ std::string(", length: ") + std::to_string(Length)
		).c_str()
	)
{
}


DIndexException::DIndexException(cint Index, cint RangeStart, cint RangeStop, bool)
	: DRuntimeException(
		(
			  std::string("Out of Range Exception: index: ")
			+ std::to_string(Index) 
			+ std::string(", range start: ") + std::to_string(RangeStart) 
			+ std::string(", range stop: ") + std::to_string(RangeStop)
		).c_str()
	)
{
}


DIndexException::DIndexException(const char* InMessage)
	: DRuntimeException(InMessage)
{
}