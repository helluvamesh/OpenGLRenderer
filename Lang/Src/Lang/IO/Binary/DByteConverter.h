#pragma once


class DByteConverter
{
private:
	bool SameEndien;
	byte* Bytes;
	cint ByteCount;
	cint TotalBytesWritten;

public:
	DByteConverter(bool LittleEndien);
	DByteConverter();

	void SetChunk(byte* ChunkBytes, cint ChunkByteCount);

private:
	template<typename T>
	FORCEINLINE bool WriteItem(const T& Item);

	template<typename T>
	FORCEINLINE bool WriteArray(DArrayView<T> Array);

public:

	// write structs whose fields are all of the same type, like FVec3, FQuat, FMat4...
	// only works with non-polymorphic structs (no virtual methods)
	template<typename StructT, typename PrimitiveT, cint PrimitiveCount>
	bool WriteHomogenousStructArray(DArrayView<StructT> Array)
	{
		cint primitivesToWrite = Array.Length() * PrimitiveCount;
		if (primitivesToWrite * CINT_SIZEOF(PrimitiveT) > this->ByteCount - this->TotalBytesWritten) return false;

		cint bytesWritten;
		if (this->SameEndien)
			bytesWritten = SPrimitiveCopy<true, byte, PrimitiveT>::ArrayToBytes(reinterpret_cast<const PrimitiveT*>(Array.Data()), primitivesToWrite, this->Bytes);
		else
			bytesWritten = SPrimitiveCopy<false, byte, PrimitiveT>::ArrayToBytes(reinterpret_cast<const PrimitiveT*>(Array.Data()), primitivesToWrite, this->Bytes);
		this->TotalBytesWritten += bytesWritten;
		this->Bytes += bytesWritten;

		return true;
	}

	bool WriteInt8(const int8& Item);
	bool WriteInt16(const int16& Item);
	bool WriteInt32(const int32& Item);
	bool WriteInt64(const int64& Item);
	bool WriteUInt8(const uint8& Item);
	bool WriteUInt16(const uint16& Item);
	bool WriteUInt32(const uint32& Item);
	bool WriteUInt64(const uint64& Item);
	bool WriteFloat(const float& Item);
	bool WriteDouble(const double& Item);

	bool WriteInt8Array(DArrayView<int8> Array);
	bool WriteInt16Array(DArrayView<int16> Array);
	bool WriteInt32Array(DArrayView<int32> Array);
	bool WriteInt64Array(DArrayView<int64> Array);
	bool WriteUInt8Array(DArrayView<uint8> Array);
	bool WriteUInt16Array(DArrayView<uint16> Array);
	bool WriteUInt32Array(DArrayView<uint32> Array);
	bool WriteUInt64Array(DArrayView<uint64> Array);
	bool WriteFloatArray(DArrayView<float> Array);
	bool WriteDoubleArray(DArrayView<double> Array);

	bool WriteString8(DStringView8 String);
};
