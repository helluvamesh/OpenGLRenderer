#include "pch.h"
#include "DNumberFormat.h"


template<typename CharType>
class DNumberFormatCore
{
protected:
    /*
        %d   -> for int

        %u   -> for unsigned int

        %ld  -> for long int or long

        %lu  -> for unsigned long int or long unsigned int or unsigned long

        %lld -> for long long int or long long

        %llu -> for unsigned long long int or unsigned long long
    */
    static inline const char* FloatFormatStrings[] {

        "%.1f", "%.2", "%.3f", "%.4f", "%.5f", "%.6f", "%.7f", "%.8f", "%.9f", "%.10f",
        "%.11f", "%.12f", "%.13f", "%.14f", "%.15f", "%.16f", "%.17f", "%.18f"
    };

public:
    static void Reverse(CharType* Buffer, cint Length)
    {
        if (Length > 1)
        {
            cint middle = (Length % 2 == 0)? (Length / 2) : ((Length + 1) / 2);
            cint last = Length - 1;
            for (cint i = 0; i < middle; i++)
            {
                CharType temp = Buffer[i];
                Buffer[i] = Buffer[last - i];
                Buffer[last - i] = temp;
            }
        }
    }


    template<typename IntegerType>
    static cint IntToString(IntegerType Value, CharType* Buffer, int32 MinDigits, bool DoGroupping, CharType GroupSeparator)
    {
        bool negative = Value < 0;
        if (negative)
        {
            Value *= -1;
        }

        cint i = 0;

        if (DoGroupping)
        {
            cint groupped_i = 0;
            while (Value) 
            {
                Buffer[i++] = (CharType)'0' + (CharType)(Value % 10);
                if (++groupped_i == 3 && Value / 10)
                {
                    Buffer[i++] = GroupSeparator;
                    groupped_i = 0;
                }
                Value /= 10;
            }
        }
        else
        {
            while (Value)
            {
                Buffer[i++] = (CharType)'0' + (Value % 10);
                Value /= 10;
            }
        }

        if (i > 0)
        {
            while (i < MinDigits)
            {
                Buffer[i++] = (CharType)'0';
            }
        }
        else
        {
            Buffer[i++] = (CharType)'0';
        }

        if (negative)
        {
            Buffer[i++] = (CharType)'-';
        }

        Buffer[i] = '\0';
        Reverse(Buffer, i);
        return i;
    }


    template<typename IntegerType>
    static cint FractionToString(IntegerType Value, CharType* Buffer, int32 MinFractionDigits, int32 MaxFractionDigits)
    {
        cint i = 0;
        cint droppedFinalZeros = 0;

        bool nonZeroFound = false;
        while (Value) 
        {
            int64 digit = Value % 10;
            if (digit != 0)
            {
                nonZeroFound = true;
                Buffer[i++] = (CharType)'0' + (CharType)digit;
            }
            else 
            {
                if (nonZeroFound)
                {
                    Buffer[i++] = (CharType)'0' + (CharType)digit;
                }
                else
                {
                    droppedFinalZeros++;
                }
            }
            Value /= 10;
        }

        MaxFractionDigits -= droppedFinalZeros;
        if (i > 0)
        {
            while (i < MaxFractionDigits)
            {
                Buffer[i++] = (CharType)'0';
            }
        }
        else
        {
            while (i < MinFractionDigits)
            {
                Buffer[i++] = (CharType)'0';
            }
        }

        Buffer[i] = '\0';
        Reverse(Buffer, i);
        return i;
    }


    template<typename IntegerType>
    static cint GetIntBufferSize(IntegerType Value, int32 MinDigits, bool DoGroupping)
    {
        cint i = 0;
        if (Value < 0)
        {
            i++;
        }
        if (Value == 0)
        {
            i++;
        }
        else
        {
            while (Value)
            {
                i++;
                Value /= 10;
            }
        }
        if (DoGroupping)
        {
            i += i / 3;
        }
        return SCounterUtil::Max(i, MinDigits);
    }

    // Min/Max FractionDigits: [1...18] (int64 can safely have 18 digits)
    // FastMethod will produce garbage if Value is out of int64 range
    // FranctionDigits >= 0 & FranctionDigits <= 16
    template<typename FloatType>
    static cint FloatToString(
        FloatType Value, CharType* Buffer, cint BufferSize, bool FastMethod, 
        int32 MinFractionDigits, int32 MaxFractionDigits, 
        CharType DecimalSeparator, bool DoGroupping, CharType GroupSeparator
    )
    {
        if (FastMethod)
        {
            bool negative = Value < .0f;
            if (negative)
            {
                Value *= -1;
            }

            int64 integer = (int64)Value;
            FloatType fraction = Value - (FloatType)integer;

            cint i = 0;
            if (negative)
            {
                Buffer[i++] = (CharType)'-';
            }
            i += IntToString<int64>(integer, Buffer + i, 0, DoGroupping, GroupSeparator);

            if (MaxFractionDigits > 0)
            {
                Buffer[i++] = (CharType)DecimalSeparator;
                int64 fraction_int = (int64)(fraction * SCounterUtil::Pow(10.0, MaxFractionDigits));
                if (fraction_int < 0)
                {
                    fraction_int *= -1;
                }
                i += FractionToString<int64>(fraction_int, Buffer + i, MinFractionDigits, MaxFractionDigits);
            }
            return i;
        }
        else
        {
            size_t len = (size_t)BufferSize + 1;
            char* ascii = (char*)_malloca(len * sizeof(char));
            if (ascii)
            {
                sprintf_s(ascii, len, FloatFormatStrings[MaxFractionDigits - 1], Value);
                for (cint i = 0; i < BufferSize; i++)
                {
                    Buffer[i] = (CharType)ascii[i];
                }
                Buffer[BufferSize] = '\0';
            }
            _freea(ascii);
            return (cint)len;
        }
    }


    template<typename FloatType>
    static cint GetMaxNeededFloatBufferSize(FloatType Value, int32 MaxFractionDigits, bool DoGroupping, bool& OutFastMethod)
    {
        bool positive = Value >= .0f;
        OutFastMethod = positive?
            Value < (FloatType)INT64_MAX :
            Value > (FloatType)INT64_MIN;
        
        if (OutFastMethod)
        {
            if (!positive)
            {
                Value *= -1;
            }

            int64 integer = (int64)Value;
            FloatType fraction = Value - (FloatType)integer;
            int64 fraction_int = (int64)(fraction * SCounterUtil::Pow(10.0, MaxFractionDigits));

            return
                  (positive? 0 : 1)  // sign
                + GetIntBufferSize(integer, 0, DoGroupping) 
                + 1  // decimal separator
                + GetIntBufferSize(fraction_int, MaxFractionDigits, false);
        }
        else
        {
            return (cint)_scprintf(FloatFormatStrings[MaxFractionDigits - 1], Value);
        }
    }

};


// ----------------------- TCHAR ----------------------


const DNumberFormat DNumberFormat::DEFAULT;

const DString DNumberFormat::NAN_STRING = TX("nan");
const DString DNumberFormat::INF_STRING = TX("inf");
const DString DNumberFormat::NEG_INF_STRING = TX("-inf");


tchar DNumberFormat::GetDecimalSeparator() const { return this->DecimalSeparator; }
void DNumberFormat::SetDecimalSeparator(tchar V) { this->DecimalSeparator = V; }

tchar DNumberFormat::GetGroupSeparator() const { return this->GroupSeparator; }
tchar DNumberFormat::GetUseGroupSeparator() const { return this->UseGroupSeparator; }
void DNumberFormat::SetGroupSeparator(tchar V) { this->GroupSeparator = V; this->UseGroupSeparator = true; }
void DNumberFormat::ClearGroupSeparator() { this->GroupSeparator = ' '; this->UseGroupSeparator = false; }

cint DNumberFormat::GetMinFloatFractionDigits() const { return this->MinFloatFractionDigits; }
void DNumberFormat::SetMinFloatFractionDigits(cint V) { this->MinFloatFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat::GetMinDoubleFractionDigits() const { return this->MinDoubleFractionDigits; }
void DNumberFormat::SetMinDoubleFractionDigits(cint V) { this->MinDoubleFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat::GetMaxFloatFractionDigits() const { return this->MaxFloatFractionDigits; }
void DNumberFormat::SetMaxFloatFractionDigits(cint V) { this->MaxFloatFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat::GetMaxDoubleFractionDigits() const { return this->MaxDoubleFractionDigits; }
void DNumberFormat::SetMaxDoubleFractionDigits(cint V) { this->MaxDoubleFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat::GetMinIntegralDigits() const { return this->MinIntegralDigits; }
void DNumberFormat::SetMinIntegralDigits(cint V) { this->MinIntegralDigits = SCounterUtil::Max(V, 1); }


DNumberFormat::DNumberFormat():
	DecimalSeparator(u'.'),
	GroupSeparator(u' '),
	UseGroupSeparator(false),
	MinFloatFractionDigits(1),
	MinDoubleFractionDigits(1),
	MaxFloatFractionDigits(7),
	MaxDoubleFractionDigits(15),
	MinIntegralDigits(1)
{
}


void DNumberFormat::ToString(int32 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<int32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<int32>(
        Value, 
        Result.GetDataUnsafe(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(int32 Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(int32 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<int32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen =DNumberFormatCore<tchar>::IntToString<int32>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat::ToString(int64 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<int64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<int64>(
        Value, 
        Result.GetDataUnsafe(), 
        this->MinIntegralDigits, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(int64 Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(int64 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<int64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen =DNumberFormatCore<tchar>::IntToString<int64>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat::ToString(uint32 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<uint32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<uint32>(
        Value, 
        Result.GetDataUnsafe(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(uint32 Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(uint32 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<uint32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<uint32>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat::ToString(uint64 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<uint64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<uint64>(
        Value, 
        Result.GetDataUnsafe(), 
        this->MinIntegralDigits, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(uint64 Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(uint64 Value, DString& Result) const
{
    cint len = DNumberFormatCore<tchar>::GetIntBufferSize<uint64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<tchar>::IntToString<uint64>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat::ToString(int8 Value, DString& Result) const
{
    this->ToString((int32)Value, Result);
}


DString DNumberFormat::ToString(int8 Value) const
{
    return this->ToString((int32)Value);
}


void DNumberFormat::AppendTo(int8 Value, DString& Result) const
{
    this->AppendTo((int32)Value, Result);
}


void DNumberFormat::ToString(int16 Value, DString& Result) const
{
    this->ToString((int32)Value, Result);
}


DString DNumberFormat::ToString(int16 Value) const
{
    return this->ToString((int32)Value);
}


void DNumberFormat::AppendTo(int16 Value, DString& Result) const
{
    this->AppendTo((int32)Value, Result);
}


void DNumberFormat::ToString(uint8 Value, DString& Result) const
{
    this->ToString((uint32)Value, Result);
}


DString DNumberFormat::ToString(uint8 Value) const
{
    return this->ToString((uint32)Value);
}


void DNumberFormat::AppendTo(uint8 Value, DString& Result) const
{
    this->AppendTo((uint32)Value, Result);
}


void DNumberFormat::ToString(uint16 Value, DString& Result) const
{
    this->ToString((uint32)Value, Result);
}


DString DNumberFormat::ToString(uint16 Value) const
{
    return this->ToString((uint32)Value);
}


void DNumberFormat::AppendTo(uint16 Value, DString& Result) const
{
    this->AppendTo((uint32)Value, Result);
}


void DNumberFormat::ToString(float Value, DString& Result) const
{
    if (FMath::IsNaN(Value))
    {
        Result = NAN_STRING;
        return;
    }
    else if (FMath::IsInf(Value))
    {
        Result = Value >= .0f? INF_STRING : NEG_INF_STRING;
        return;
    }

	bool fastMethod;
	cint len = DNumberFormatCore<tchar>::GetMaxNeededFloatBufferSize<float>(
        Value, 
        this->MaxFloatFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
	Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::FloatToString<float>(
        Value, 
        Result.GetDataUnsafe(), 
        len, 
        fastMethod, 
        this->MinFloatFractionDigits, 
        this->MaxFloatFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
	Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(float Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(float Value, DString& Result) const
{
    if (FMath::IsNaN(Value))
    {
        Result.Append(NAN_STRING);
        return;
    }
    else if (FMath::IsInf(Value))
    {
        Result.Append(Value >= .0f? INF_STRING : NEG_INF_STRING);
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<tchar>::GetMaxNeededFloatBufferSize<float>(
        Value, 
        this->MaxFloatFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<tchar>::FloatToString<float>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(), 
        len, 
        fastMethod, 
        this->MinFloatFractionDigits, 
        this->MaxFloatFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}



void DNumberFormat::ToString(double Value, DString& Result) const
{
    if (DMath::IsNaN(Value))
    {
        Result = NAN_STRING;
        return;
    }
    else if (DMath::IsInf(Value))
    {
        Result = Value >= .0? INF_STRING : NEG_INF_STRING;
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<tchar>::GetMaxNeededFloatBufferSize<double>(
        Value, 
        this->MaxDoubleFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<tchar>::FloatToString<double>(
        Value, 
        Result.GetDataUnsafe(), 
        len, 
        fastMethod, 
        this->MinDoubleFractionDigits, 
        this->MaxDoubleFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString DNumberFormat::ToString(double Value) const
{
    DString buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat::AppendTo(double Value, DString& Result) const
{
    if (DMath::IsNaN(Value))
    {
        Result.Append(NAN_STRING);
        return;
    }
    else if (DMath::IsInf(Value))
    {
        Result.Append(Value >= .0? INF_STRING : NEG_INF_STRING);
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<tchar>::GetMaxNeededFloatBufferSize<double>(
        Value, 
        this->MaxDoubleFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<tchar>::FloatToString<double>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(), 
        len, 
        fastMethod, 
        this->MinDoubleFractionDigits, 
        this->MaxDoubleFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}




const DNumberFormat8 DNumberFormat8::DEFAULT;

const DString8 DNumberFormat8::NAN_STRING = "nan";
const DString8 DNumberFormat8::INF_STRING = "inf";
const DString8 DNumberFormat8::NEG_INF_STRING = "-inf";

char8 DNumberFormat8::GetDecimalSeparator() const { return this->DecimalSeparator; }
void DNumberFormat8::SetDecimalSeparator(char8 V) { this->DecimalSeparator = V; }

char8 DNumberFormat8::GetGroupSeparator() const { return this->GroupSeparator; }
char8 DNumberFormat8::GetUseGroupSeparator() const { return this->UseGroupSeparator; }
void DNumberFormat8::SetGroupSeparator(char8 V) { this->GroupSeparator = V; this->UseGroupSeparator = true; }
void DNumberFormat8::ClearGroupSeparator() { this->GroupSeparator = ' '; this->UseGroupSeparator = false; }

cint DNumberFormat8::GetMinFloatFractionDigits() const { return this->MinFloatFractionDigits; }
void DNumberFormat8::SetMinFloatFractionDigits(cint V) { this->MinFloatFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat8::GetMinDoubleFractionDigits() const { return this->MinDoubleFractionDigits; }
void DNumberFormat8::SetMinDoubleFractionDigits(cint V) { this->MinDoubleFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat8::GetMaxFloatFractionDigits() const { return this->MaxFloatFractionDigits; }
void DNumberFormat8::SetMaxFloatFractionDigits(cint V) { this->MaxFloatFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat8::GetMaxDoubleFractionDigits() const { return this->MaxDoubleFractionDigits; }
void DNumberFormat8::SetMaxDoubleFractionDigits(cint V) { this->MaxDoubleFractionDigits = SCounterUtil::Clamp(V, 1, 18); }

cint DNumberFormat8::GetMinIntegralDigits() const { return this->MinIntegralDigits; }
void DNumberFormat8::SetMinIntegralDigits(cint V) { this->MinIntegralDigits = SCounterUtil::Max(V, 1); }


DNumberFormat8::DNumberFormat8():
    DecimalSeparator(u'.'),
    GroupSeparator(u' '),
    UseGroupSeparator(false),
    MinFloatFractionDigits(1),
    MinDoubleFractionDigits(1),
    MaxFloatFractionDigits(7),
    MaxDoubleFractionDigits(15),
    MinIntegralDigits(1)
{
}


void DNumberFormat8::ToString(int32 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<int32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<int32>(
        Value, 
        Result.GetDataUnsafe(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(int32 Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(int32 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<int32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<int32>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat8::ToString(int64 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<int64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<int64>(
        Value, 
        Result.GetDataUnsafe(), 
        this->MinIntegralDigits, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(int64 Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(int64 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<int64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen =DNumberFormatCore<char8>::IntToString<int64>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat8::ToString(uint32 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<uint32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<uint32>(
        Value, 
        Result.GetDataUnsafe(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(uint32 Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(uint32 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<uint32>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<uint32>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat8::ToString(uint64 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<uint64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<uint64>(
        Value, 
        Result.GetDataUnsafe(), 
        this->MinIntegralDigits, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(uint64 Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(uint64 Value, DString8& Result) const
{
    cint len = DNumberFormatCore<char8>::GetIntBufferSize<uint64>(
        Value, 
        this->MinIntegralDigits,
        this->UseGroupSeparator
    );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<char8>::IntToString<uint64>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(),
        this->MinIntegralDigits, 
        this->UseGroupSeparator, 
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}


void DNumberFormat8::ToString(int8 Value, DString8& Result) const
{
    this->ToString((int32)Value, Result);
}


DString8 DNumberFormat8::ToString(int8 Value) const
{
    return this->ToString((int32)Value);
}


void DNumberFormat8::AppendTo(int8 Value, DString8& Result) const
{
    this->AppendTo((int32)Value, Result);
}


void DNumberFormat8::ToString(int16 Value, DString8& Result) const
{
    this->ToString((int32)Value, Result);
}


DString8 DNumberFormat8::ToString(int16 Value) const
{
    return this->ToString((int32)Value);
}


void DNumberFormat8::AppendTo(int16 Value, DString8& Result) const
{
    this->AppendTo((int32)Value, Result);
}


void DNumberFormat8::ToString(uint8 Value, DString8& Result) const
{
    this->ToString((uint32)Value, Result);
}


DString8 DNumberFormat8::ToString(uint8 Value) const
{
    return this->ToString((uint32)Value);
}


void DNumberFormat8::AppendTo(uint8 Value, DString8& Result) const
{
    this->AppendTo((uint32)Value, Result);
}


void DNumberFormat8::ToString(uint16 Value, DString8& Result) const
{
    this->ToString((uint32)Value, Result);
}


DString8 DNumberFormat8::ToString(uint16 Value) const
{
    return this->ToString((uint32)Value);
}


void DNumberFormat8::AppendTo(uint16 Value, DString8& Result) const
{
    this->AppendTo((uint32)Value, Result);
}


void DNumberFormat8::ToString(float Value, DString8& Result) const
{
    if (FMath::IsNaN(Value))
    {
        Result = NAN_STRING;
        return;
    }
    else if (FMath::IsInf(Value))
    {
        Result = Value >= .0f? INF_STRING : NEG_INF_STRING;
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<char8>::GetMaxNeededFloatBufferSize<float>(
        Value, 
        this->MaxFloatFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::FloatToString<float>(
        Value, 
        Result.GetDataUnsafe(), 
        len, 
        fastMethod, 
        this->MinFloatFractionDigits, 
        this->MaxFloatFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(float Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(float Value, DString8& Result) const
{
    if (FMath::IsNaN(Value))
    {
        Result.Append(NAN_STRING);
        return;
    }
    else if (FMath::IsInf(Value))
    {
        Result.Append(Value >= .0f? INF_STRING : NEG_INF_STRING);
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<char8>::GetMaxNeededFloatBufferSize<float>(
        Value, 
        this->MaxFloatFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<char8>::FloatToString<float>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(), 
        len, 
        fastMethod, 
        this->MinFloatFractionDigits, 
        this->MaxFloatFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}



void DNumberFormat8::ToString(double Value, DString8& Result) const
{
    if (DMath::IsNaN(Value))
    {
        Result = NAN_STRING;
        return;
    }
    else if (DMath::IsInf(Value))
    {
        Result = Value >= .0? INF_STRING : NEG_INF_STRING;
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<char8>::GetMaxNeededFloatBufferSize<double>(
        Value, 
        this->MaxDoubleFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.Reallocate(len);
    cint finalLen = DNumberFormatCore<char8>::FloatToString<double>(
        Value, 
        Result.GetDataUnsafe(), 
        len, 
        fastMethod, 
        this->MinDoubleFractionDigits, 
        this->MaxDoubleFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(finalLen);
}


DString8 DNumberFormat8::ToString(double Value) const
{
    DString8 buffer;
    ToString(Value, buffer);
    return buffer;
}


void DNumberFormat8::AppendTo(double Value, DString8& Result) const
{
    if (DMath::IsNaN(Value))
    {
        Result = NAN_STRING;
        return;
    }
    else if (DMath::IsInf(Value))
    {
        Result.Append(Value >= .0? INF_STRING : NEG_INF_STRING);
        return;
    }

    bool fastMethod;
    cint len = DNumberFormatCore<char8>::GetMaxNeededFloatBufferSize<double>(
        Value, 
        this->MaxDoubleFractionDigits, 
        this->UseGroupSeparator, 
        /*out*/fastMethod
        );
    Result.ReserveExtra(len);
    cint finalLen = DNumberFormatCore<char8>::FloatToString<double>(
        Value, 
        Result.GetDataUnsafe() + Result.Length(), 
        len, 
        fastMethod, 
        this->MinDoubleFractionDigits, 
        this->MaxDoubleFractionDigits, 
        this->DecimalSeparator, 
        this->UseGroupSeparator,
        this->GroupSeparator
        );
    Result.OverwriteLengthUnsafe(Result.Length() + finalLen);
}
