#include "pch.h"
#include "DByteConverter.h"
#include "SPrimitiveCopy.h"


DByteConverter::DByteConverter(bool LittleEndien) :
	SameEndien(SBinaryInfo::ShouldKeepEndiannes(LittleEndien)),
	Bytes(nullptr),
	ByteCount(0),
	TotalBytesWritten(0)
{
}


DByteConverter::DByteConverter() : 
	SameEndien(SBinaryInfo::ShouldKeepEndiannes(true)),
	Bytes(nullptr),
	ByteCount(0),
	TotalBytesWritten(0)
{
}


void DByteConverter::SetChunk(byte* ChunkBytes, cint ChunkByteCount)
{
	this->Bytes = ChunkBytes;
	this->ByteCount = ChunkByteCount;
	this->TotalBytesWritten = 0;
}


template<typename T>
bool DByteConverter::WriteItem(const T& Item)
{
	cint bytesWritten;
	if (this->SameEndien)
		bytesWritten = SPrimitiveCopy<true, byte, T>::ItemToBytes(&Item, this->Bytes);
	else
		bytesWritten = SPrimitiveCopy<false, byte, T>::ItemToBytes(&Item, this->Bytes);
	this->TotalBytesWritten += bytesWritten;
	this->Bytes += bytesWritten;

	return this->TotalBytesWritten <= this->ByteCount;
}


template<typename T>
bool DByteConverter::WriteArray(DArrayView<T> Array)
{
	if (Array.Length() * CINT_SIZEOF(T) > this->ByteCount - this->TotalBytesWritten) return false;

	cint bytesWritten;
	if (this->SameEndien)
		bytesWritten = SPrimitiveCopy<true, byte, T>::ArrayToBytes(Array.Data(), Array.Length(), this->Bytes);
	else
		bytesWritten = SPrimitiveCopy<false, byte, T>::ArrayToBytes(Array.Data(), Array.Length(), this->Bytes);
	this->TotalBytesWritten += bytesWritten;
	this->Bytes += bytesWritten;

	return true;
}


bool DByteConverter::WriteInt8(const int8& Item)
{
	return this->WriteItem<int8>(Item);
}


bool DByteConverter::WriteInt16(const int16& Item)
{
	return this->WriteItem<int16>(Item);
}


bool DByteConverter::WriteInt32(const int32& Item)
{
	return this->WriteItem<int32>(Item);
}


bool DByteConverter::WriteInt64(const int64& Item)
{
	return this->WriteItem<int64>(Item);
}


bool DByteConverter::WriteUInt8(const uint8& Item)
{
	return this->WriteItem<uint8>(Item);
}


bool DByteConverter::WriteUInt16(const uint16& Item)
{
	return this->WriteItem<uint16>(Item);
}


bool DByteConverter::WriteUInt32(const uint32& Item)
{
	return this->WriteItem<uint32>(Item);
}


bool DByteConverter::WriteUInt64(const uint64& Item)
{
	return this->WriteItem<uint64>(Item);
}


bool DByteConverter::WriteFloat(const float& Item)
{
	return this->WriteItem<float>(Item);
}


bool DByteConverter::WriteDouble(const double& Item)
{
	return this->WriteItem<double>(Item);
}


bool DByteConverter::WriteInt8Array(DArrayView<int8> Array)
{
	return this->WriteArray<int8>(Array);
}


bool DByteConverter::WriteInt16Array(DArrayView<int16> Array)
{
	return this->WriteArray<int16>(Array);
}


bool DByteConverter::WriteInt32Array(DArrayView<int32> Array)
{
	return this->WriteArray<int32>(Array);
}


bool DByteConverter::WriteInt64Array(DArrayView<int64> Array)
{
	return this->WriteArray<int64>(Array);
}


bool DByteConverter::WriteUInt8Array(DArrayView<uint8> Array)
{
	return this->WriteArray<uint8>(Array);
}


bool DByteConverter::WriteUInt16Array(DArrayView<uint16> Array)
{
	return this->WriteArray<uint16>(Array);
}


bool DByteConverter::WriteUInt32Array(DArrayView<uint32> Array)
{
	return this->WriteArray<uint32>(Array);
}


bool DByteConverter::WriteUInt64Array(DArrayView<uint64> Array)
{
	return this->WriteArray<uint64>(Array);
}


bool DByteConverter::WriteFloatArray(DArrayView<float> Array)
{
	return this->WriteArray<float>(Array);
}


bool DByteConverter::WriteDoubleArray(DArrayView<double> Array)
{
	return this->WriteArray<double>(Array);
}


bool DByteConverter::WriteString8(DStringView8 String)
{
	if (String.Length() * CINT_SIZEOF(char8) > this->ByteCount - this->TotalBytesWritten) return false;

	cint bytesWritten;
	if (this->SameEndien)
		bytesWritten = SPrimitiveCopy<true, byte, char8>::ArrayToBytes(String.Data(), String.Length(), this->Bytes);
	else
		bytesWritten = SPrimitiveCopy<false, byte, char8>::ArrayToBytes(String.Data(), String.Length(), this->Bytes);
	this->TotalBytesWritten += bytesWritten;
	this->Bytes += bytesWritten;

	return true;
}
