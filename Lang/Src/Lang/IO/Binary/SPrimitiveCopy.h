#pragma once


class SBinaryInfo
{
public:
    static bool IsSystemLittleEndian();
    static bool ShouldKeepEndiannes(bool LittleEndianBytes);
};


template<typename BYTE>
class SBinaryUtil
{
public:

    FORCEINLINE static void Reverse(BYTE* Bytes, cint ByteCount)
    {
        cint middle = ByteCount / 2;
        cint end = ByteCount - 1;
        for (cint i = 0; i < middle; ++i)
        {
            BYTE temp = Bytes[i];
            Bytes[i] = Bytes[end - i];
            Bytes[end - i] = temp;
        }
    }


    FORCEINLINE static void ReverseArrayItems(BYTE* Bytes, cint ByteCount, cint ItemByteCount)
    {
        for (cint i = 0; i < ByteCount; i += ItemByteCount)
        {
            Reverse(Bytes + i, ItemByteCount);
        }
    }


    FORCEINLINE static void ReverseCopy(const BYTE* InBytes, cint ByteCount, BYTE* OutBytes)
    {
        cint last = ByteCount - 1;
        for (cint i = last; i >= 0; --i)
        {
            OutBytes[last - i] = InBytes[i];
        }
    }


    FORCEINLINE static void Copy(const BYTE* InBytes, cint ByteCount, BYTE* OutBytes)
    {
        for (cint i = 0; i < ByteCount; ++i)
        {
            OutBytes[i] = InBytes[i];
        }
    }


    FORCEINLINE static void CopyArrayReversingItems(const BYTE* InBytes, cint ByteCount, cint ItemByteCount, BYTE* OutBytes)
    {
        for (cint i = 0; i < ByteCount; i += ItemByteCount)
        {
            ReverseCopy(InBytes + i, ItemByteCount, OutBytes + i);
        }
    }

};


// SE: same endian
template<bool SE, typename BYTE, typename T>
class SPrimitiveCopy
{
public:
    // returns number of bytes written
    FORCEINLINE static cint ItemToBytes(const T* InValue, BYTE* OutBytes);
    // returns number of bytes read
    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, T* Out);
    // returns number of bytes written
    FORCEINLINE static cint ArrayToBytes(const T* InArray, cint Length, BYTE* OutBytes);
    // returns number of bytes read
    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, T* OutArray);
};

// ------------------------- SAME ENDIAN --------------------------

template<typename BYTE, typename T>
class SPrimitiveCopy<true, BYTE, T>
{
public:
    FORCEINLINE static cint ItemToBytes(const T* InValue, BYTE* OutBytes)
    {
        memcpy(OutBytes, InValue, sizeof(T));
        return CINT_SIZEOF(T);
    }


    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, T* OutValue)
    {
        memcpy(OutValue, InBytes, sizeof(T));
        return CINT_SIZEOF(T);
    }


    FORCEINLINE static cint ArrayToBytes(const T* InArray, cint Length, BYTE* OutBytes)
    {
        cint len = CINT_SIZEOF(T) * Length;
        memcpy(OutBytes, InArray, len);
        return len;
    }


    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, T* OutArray)
    {
        cint len = ResultLength * CINT_SIZEOF(T);
        memcpy(OutArray, InBytes, len);
        return len;
    }
};

// ------------------------- OPPOSITE ENDIAN --------------------------

template<typename BYTE, typename T>
class SPrimitiveCopy<false, BYTE, T>
{
public:
    FORCEINLINE static cint ItemToBytes(const T* InValue, BYTE* OutBytes)
    {
        BYTE* bytes = reinterpret_cast<BYTE*>(const_cast<T*>(InValue));
        SBinaryUtil<BYTE>::ReverseCopy(bytes, CINT_SIZEOF(T), OutBytes);
        return CINT_SIZEOF(T);
    }


    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, T* OutValue)
    {
        BYTE* outBytes = reinterpret_cast<BYTE*>(OutValue);
        SBinaryUtil<BYTE>::ReverseCopy(InBytes, CINT_SIZEOF(T), outBytes);
        return CINT_SIZEOF(T);
    }


    FORCEINLINE static cint ArrayToBytes(const T* InArray, cint Length, BYTE* OutBytes)
    {
        cint len = CINT_SIZEOF(T) * Length;
        BYTE* bytes = reinterpret_cast<BYTE*>(const_cast<T*>(InArray));
        SBinaryUtil<BYTE>::CopyArrayReversingItems(bytes, len, CINT_SIZEOF(T), OutBytes);
        return len;
    }


    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, T* OutArray)
    {
        cint len = ResultLength * CINT_SIZEOF(T);
        BYTE* outBytes = reinterpret_cast<BYTE*>(OutArray);
        SBinaryUtil<BYTE>::CopyArrayReversingItems(InBytes, len, CINT_SIZEOF(T), outBytes);
        return len;
    }
};
