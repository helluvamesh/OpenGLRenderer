#pragma once

#include "SPrimitiveCopy.h"


// SE: same endian
// Expects objects to be initialized
template<bool SE, typename BYTE, typename T>
class SCoreObjectCopy 
{
};


template<bool SE, typename BYTE>
class SCoreObjectCopy<SE, BYTE, UINT8_ENUM>
{
public:
    FORCEINLINE static cint GetByteCount(const uint8* E)
    {
        return CINT_SIZEOF(uint8);
    }

    FORCEINLINE static cint GetByteCount()
    {
        return CINT_SIZEOF(uint8);
    }

    FORCEINLINE static cint ItemToBytes(const uint8* E, BYTE* OutBytes)
    {
        *OutBytes = (BYTE)*E;
        return CINT_SIZEOF(uint8);
    }

    FORCEINLINE static cint BytesToItemChecked(const BYTE* InBytes, uint8 EnumLength, uint8* Out)
    {
        uint8 value = InBytes[0];
        if (value >= 0 && value < EnumLength)
        {
            *Out = value;
        }
        else
        {
            *Out = 0;
        }
        return CINT_SIZEOF(uint8);
    }

    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, uint8* Out)
    {
        uint8 value = InBytes[0];
        if (value >= 0 && value < UINT8_MAX)
        {
            *Out = value;
        }
        else
        {
            *Out = 0;
        }
        return CINT_SIZEOF(uint8);
    }

    FORCEINLINE static cint ArrayToBytes(const uint8* InArray, cint Length, BYTE* OutBytes)
    {
        for (cint i = 0; i < Length; ++i)
        {
            OutBytes[i] = (BYTE)(uint8)InArray[i];
        }
        return CINT_SIZEOF(uint8) * Length;
    }

    FORCEINLINE static cint BytesToArrayChecked(const BYTE* InBytes, cint ResultLength, uint8 EnumLength, uint8* OutArray)
    {
        for (cint i = 0; i < ResultLength; ++i)
        {
            uint8 value = InBytes[i];
            if (value >= 0 && value < EnumLength)
            {
                OutArray[i] = value;
            }
            else
            {
                OutArray[i] = 0;
            }
        }
        return CINT_SIZEOF(uint8) * ResultLength;
    }

    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, uint8* OutArray)
    {
        for (cint i = 0; i < ResultLength; ++i)
        {
            uint8 value = InBytes[0];
            if (value >= 0 && value < UINT8_MAX)
            {
                OutArray[i] = value;
            }
            else
            {
                OutArray[i] = 0;
            }
        }
        return CINT_SIZEOF(uint8) * ResultLength;
    }

    FORCEINLINE static cint SkipItemBytes(const BYTE* InBytes)
    {
        return CINT_SIZEOF(uint8);
    }
        
    FORCEINLINE static cint SkipArrayBytes(const BYTE* InBytes, cint ResultLength)
    {
        return CINT_SIZEOF(uint8) * ResultLength;
    }
};


template<bool SE, typename BYTE>
class SCoreObjectCopy<SE, BYTE, DString>
{
public:
    FORCEINLINE static cint GetByteCount(const DString* S)
    {
        return CINT_SIZEOF(cint) + S->Length() * CINT_SIZEOF(tchar);
    }

    FORCEINLINE static cint ItemToBytes(const DString* S, BYTE* OutBytes)
    {
        cint totalBytesWritten = 0;

        // write meta
        cint len = S->Length();
        totalBytesWritten += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);

        // write chars
        totalBytesWritten += SPrimitiveCopy<SE, BYTE, tchar>::ArrayToBytes(S->Data(), len, OutBytes + CINT_SIZEOF(cint));

        return totalBytesWritten;
    }

    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, DString* Out)
    {
        cint totalBytesRead = 0;

        // read meta
        cint len;
        totalBytesRead += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);

        // read chars
        Out->Reallocate(len, true);
        totalBytesRead += SPrimitiveCopy<SE, BYTE, tchar>::BytesToArray(InBytes + CINT_SIZEOF(cint), len, Out->GetDataUnsafe());

        return totalBytesRead;
    }

    FORCEINLINE static cint ArrayToBytes(const DString* InArray, cint Length, BYTE* OutBytes)
    {
        cint totalBytesWritten = 0;

        for (cint i = 0; i < Length; ++i)
        {
            totalBytesWritten += ItemToBytes(&InArray[i], OutBytes + totalBytesWritten);
        }

        return totalBytesWritten;
    }

    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, DString* OutArray)
    {
        cint totalBytesRead = 0;

        for (cint i = 0; i < ResultLength; ++i)
        {
            totalBytesRead += BytesToItem(InBytes + totalBytesRead, &OutArray[i]);
        }

        return totalBytesRead;
    }

    FORCEINLINE static cint SkipItemBytes(const BYTE* InBytes)
    {
        cint len;
        SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
        return CINT_SIZEOF(cint) + CINT_SIZEOF(tchar) * len;
    }

    FORCEINLINE static cint SkipArrayBytes(const BYTE* InBytes, cint ResultLength)
    {
        cint totalBytesSkipped = 0;

        for (int i = 0; i < ResultLength; ++i)
        {
            totalBytesSkipped += SkipItemBytes(InBytes + totalBytesSkipped);
        }

        return totalBytesSkipped;
    }
};


template<bool SE, typename BYTE>
class SCoreObjectCopy<SE, BYTE, DString8>
{
public:
    FORCEINLINE static cint GetByteCount(const DString8* S)
    {
        return CINT_SIZEOF(cint) + S->Length() * CINT_SIZEOF(char8);
    }

    FORCEINLINE static cint ItemToBytes(const DString8* S, BYTE* OutBytes)
    {
        cint totalBytesWritten = 0;

        // write meta
        cint len = S->Length();
        totalBytesWritten += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);

        // write chars
        totalBytesWritten += SPrimitiveCopy<SE, BYTE, char8>::ArrayToBytes(S->Data(), len, OutBytes + totalBytesWritten);
            
        return totalBytesWritten;
    }

    FORCEINLINE static cint BytesToItem(const BYTE* InBytes, DString8* Out)
    {
        cint totalBytesRead = 0;

        // read meta
        cint len;
        totalBytesRead += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);

        // read chars
        Out->Reallocate(len, true);
        totalBytesRead += SPrimitiveCopy<SE, BYTE, char8>::BytesToArray(InBytes + CINT_SIZEOF(cint), len, Out->GetDataUnsafe());
        
        return totalBytesRead;
    }

    FORCEINLINE static cint ArrayToBytes(const DString8* InArray, cint Length, BYTE* OutBytes)
    {
        cint totalBytesWritten = 0;

        for (cint i = 0; i < Length; ++i)
        {
            totalBytesWritten += ItemToBytes(&InArray[i], OutBytes + totalBytesWritten);
        }

        return totalBytesWritten;
    }

    FORCEINLINE static cint BytesToArray(const BYTE* InBytes, cint ResultLength, DString8* OutArray)
    {
        cint totalBytesRead = 0;

        for (cint i = 0; i < ResultLength; ++i)
        {
            totalBytesRead += BytesToItem(InBytes + totalBytesRead, &OutArray[i]);
        }

        return totalBytesRead;
    }

    FORCEINLINE static cint SkipItemBytes(const BYTE* InBytes)
    {
        cint len;
        SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
        return CINT_SIZEOF(cint) + CINT_SIZEOF(char8) * len;
    }

    FORCEINLINE static cint SkipArrayBytes(const BYTE* InBytes, cint ResultLength)
    {
        cint totalBytesSkipped = 0;

        for (int i = 0; i < ResultLength; ++i)
        {
            totalBytesSkipped += SkipItemBytes(InBytes + totalBytesSkipped);
        }

        return totalBytesSkipped;
    }

};
