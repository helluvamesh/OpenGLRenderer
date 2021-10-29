#include "pch.h"
#include "DByteReconverter.h"
#include "SPrimitiveCopy.h"


DByteReconverter::DByteReconverter(bool LittleEndien) :
	SameEndien(SBinaryInfo::ShouldKeepEndiannes(LittleEndien)),
	Bytes(nullptr),
	ByteCount(0),
	TotalBytesRead(0)
{
}


DByteReconverter::DByteReconverter() :
	SameEndien(SBinaryInfo::ShouldKeepEndiannes(true)),
	Bytes(nullptr),
	ByteCount(0),
	TotalBytesRead(0)
{
}


void DByteReconverter::SetChunk(DArrayView<byte> ChunkBytes)
{
	this->Bytes = ChunkBytes.Data();
	this->ByteCount = ChunkBytes.Length();
	this->TotalBytesRead = 0;
}


template<typename T>
bool DByteReconverter::ReadItem(T& OutItem)
{
	cint bytesRead;
	if (this->SameEndien)
		bytesRead = SPrimitiveCopy<true, byte, T>::BytesToItem(this->Bytes, &OutItem);
	else
		bytesRead = SPrimitiveCopy<false, byte, T>::BytesToItem(this->Bytes, &OutItem);
	this->TotalBytesRead += bytesRead;
	this->Bytes += bytesRead;

	return this->TotalBytesRead <= this->ByteCount;
}


template<typename T>
bool DByteReconverter::ReadArray(cint ItemCount, DArray<T>& OutArray)
{
	if (ItemCount * CINT_SIZEOF(T) > this->ByteCount - this->TotalBytesRead) return false;

	OutArray.Reallocate(ItemCount, true);
	cint bytesRead;
	if (this->SameEndien)
		bytesRead = SPrimitiveCopy<true, byte, T>::BytesToArray(this->Bytes, ItemCount, OutArray.GetDataUnsafe());
	else
		bytesRead = SPrimitiveCopy<false, byte, T>::BytesToArray(this->Bytes, ItemCount, OutArray.GetDataUnsafe());
	this->TotalBytesRead += bytesRead;
	this->Bytes += bytesRead;

	return true;
}


bool DByteReconverter::ReadInt8(int8& OutItem)
{
	return this->ReadItem<int8>(OutItem);
}


bool DByteReconverter::ReadInt16(int16& OutItem)
{
	return this->ReadItem<int16>(OutItem);
}


bool DByteReconverter::ReadInt32(int32& OutItem)
{
	return this->ReadItem<int32>(OutItem);
}


bool DByteReconverter::ReadInt64(int64& OutItem)
{
	return this->ReadItem<int64>(OutItem);
}


bool DByteReconverter::ReadUInt8(uint8& OutItem)
{
	return this->ReadItem<uint8>(OutItem);
}


bool DByteReconverter::ReadUInt16(uint16& OutItem)
{
	return this->ReadItem<uint16>(OutItem);
}


bool DByteReconverter::ReadUInt32(uint32& OutItem)
{
	return this->ReadItem<uint32>(OutItem);
}


bool DByteReconverter::ReadUInt64(uint64& OutItem)
{
	return this->ReadItem<uint64>(OutItem);
}


bool DByteReconverter::ReadFloat(float& OutItem)
{
	return this->ReadItem<float>(OutItem);
}


bool DByteReconverter::ReadDouble(double& OutItem)
{
	return this->ReadItem<double>(OutItem);
}


				  
bool DByteReconverter::ReadInt8Array(cint ItemCount, DArray<int8>& OutArray)
{
	return this->ReadArray<int8>(ItemCount, OutArray);
}


bool DByteReconverter::ReadInt16Array(cint ItemCount, DArray<int16>& OutArray)
{
	return this->ReadArray<int16>(ItemCount, OutArray);
}


bool DByteReconverter::ReadInt32Array(cint ItemCount, DArray<int32>& OutArray)
{
	return this->ReadArray<int32>(ItemCount, OutArray);
}


bool DByteReconverter::ReadInt64Array(cint ItemCount, DArray<int64>& OutArray)
{
	return this->ReadArray<int64>(ItemCount, OutArray);
}


bool DByteReconverter::ReadUInt8Array(cint ItemCount, DArray<uint8>& OutArray)
{
	return this->ReadArray<uint8>(ItemCount, OutArray);
}


bool DByteReconverter::ReadUInt16Array(cint ItemCount, DArray<uint16>& OutArray)
{
	return this->ReadArray<uint16>(ItemCount, OutArray);
}


bool DByteReconverter::ReadUInt32Array(cint ItemCount, DArray<uint32>& OutArray)
{
	return this->ReadArray<uint32>(ItemCount, OutArray);
}


bool DByteReconverter::ReadUInt64Array(cint ItemCount, DArray<uint64>& OutArray)
{
	return this->ReadArray<uint64>(ItemCount, OutArray);
}


bool DByteReconverter::ReadFloatArray(cint ItemCount, DArray<float>& OutArray)
{
	return this->ReadArray<float>(ItemCount, OutArray);
}


bool DByteReconverter::ReadDoubleArray(cint ItemCount, DArray<double>& OutArray)
{
	return this->ReadArray<double>(ItemCount, OutArray);
}


bool DByteReconverter::ReadString8(cint CharCount, DString8& OutString)
{
	if (CharCount * CINT_SIZEOF(char8) > this->ByteCount - this->TotalBytesRead) return false;

	OutString.Reallocate(CharCount, true);
	cint bytesRead;
	if (this->SameEndien)
		bytesRead = SPrimitiveCopy<true, byte, char8>::BytesToArray(this->Bytes, CharCount, OutString.GetDataUnsafe());
	else
		bytesRead = SPrimitiveCopy<false, byte, char8>::BytesToArray(this->Bytes, CharCount, OutString.GetDataUnsafe());
	this->TotalBytesRead += bytesRead;
	this->Bytes += bytesRead;

	return true;
}
