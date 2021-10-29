#pragma once


class DByteReconverter
{
private:
	bool SameEndien;
	const byte* Bytes;
	cint ByteCount;
	cint TotalBytesRead;

public:
	DByteReconverter(bool LittleEndien);
	DByteReconverter();
	
	void SetChunk(DArrayView<byte> ChunkBytes);
	
private:

	template<typename T>
	FORCEINLINE bool ReadItem(T& OutItem);

	template<typename T>
	FORCEINLINE bool ReadArray(cint ItemCount, DArray<T>& OutArray);

public:

	// read structs whose fields are all of the same type, like FVec3, FQuat, FMat4...
	// only works with non-polymorphic structs (no virtual methods)
	template<typename StructT, typename PrimitiveT, cint PrimitiveCount>
	bool ReadHomogenousStructArray(cint ItemCount, DArray<StructT>& OutArray)
	{
		cint primitivesToRead = ItemCount * PrimitiveCount;
		if (primitivesToRead * CINT_SIZEOF(PrimitiveT) > this->ByteCount - this->TotalBytesRead) return false;

		OutArray.Reallocate(ItemCount, true);
		cint bytesRead;
		if (this->SameEndien)
			bytesRead = SPrimitiveCopy<true, byte, PrimitiveT>::BytesToArray(this->Bytes, primitivesToRead, reinterpret_cast<PrimitiveT*>(OutArray.GetDataUnsafe()));
		else
			bytesRead = SPrimitiveCopy<false, byte, PrimitiveT>::BytesToArray(this->Bytes, primitivesToRead, reinterpret_cast<PrimitiveT*>(OutArray.GetDataUnsafe()));
		this->TotalBytesRead += bytesRead;

		return true;
	}

	bool ReadInt8(int8& OutItem);
	bool ReadInt16(int16& OutItem);
	bool ReadInt32(int32& OutItem);
	bool ReadInt64(int64& OutItem);
	bool ReadUInt8(uint8& OutItem);
	bool ReadUInt16(uint16& OutItem);
	bool ReadUInt32(uint32& OutItem);
	bool ReadUInt64(uint64& OutItem);
	bool ReadFloat(float& OutItem);
	bool ReadDouble(double& OutItem);
		 
	bool ReadInt8Array(cint ItemCount, DArray<int8>& OutArray);
	bool ReadInt16Array(cint ItemCount, DArray<int16>& OutArray);
	bool ReadInt32Array(cint ItemCount, DArray<int32>& OutArray);
	bool ReadInt64Array(cint ItemCount, DArray<int64>& OutArray);
	bool ReadUInt8Array(cint ItemCount, DArray<uint8>& OutArray);
	bool ReadUInt16Array(cint ItemCount, DArray<uint16>& OutArray);
	bool ReadUInt32Array(cint ItemCount, DArray<uint32>& OutArray);
	bool ReadUInt64Array(cint ItemCount, DArray<uint64>& OutArray);
	bool ReadFloatArray(cint ItemCount, DArray<float>& OutArray);
	bool ReadDoubleArray(cint ItemCount, DArray<double>& OutArray);

	bool ReadString8(cint CharCount, DString8& OutString);

};