#pragma once


class SCounterUtil
{
public:

	FORCEINLINE static cint Min(cint A, cint B)
	{
		return (A <= B)? A : B;
	}
	FORCEINLINE static cint Max(cint A, cint B)
	{
		return (A >= B)? A : B;
	}
	FORCEINLINE static cint Clamp(cint Value, cint MinV, cint MaxV)
	{
		return Min(Max(Value, MinV), MaxV);
	}


	FORCEINLINE static bool RangeCheck(cint Start, cint Stop, cint Size)
	{
		return Start >= 0 && Stop >= Start && Stop <= Size;
	}
	FORCEINLINE static bool RangeError(cint Start, cint Stop, cint Size)
	{
		return Start < 0 || Stop < Start || Stop > Size;
	}

	
	FORCEINLINE static cint Abs(cint V)
	{
		return abs(V);
	}


	FORCEINLINE static double Pow(double Base, cint Exponent)
	{
		return pow(Base, Exponent);
	}
	FORCEINLINE static double Pow(double Base, double Exponent)
	{
		return powl(Base, Exponent);
	}


	template<typename T>
	FORCEINLINE static cint CStringLength(const T* CString) 
	{
		cint count = 0;
		T c = CString[0];
		while ((c = *(CString++)) != '\0')
		{
			count++;
		}
		return count;
	}


	template<typename T>
	FORCEINLINE static cint PrimitiveCArrayHashCode(const T* Buffer, cint Length)
	{
		return Abs((cint)std::_Hash_array_representation(Buffer, Length));
	}


	FORCEINLINE static cint size_t_to_cint(size_t Value)
	{
		size_t maxValue = static_cast<size_t>(CINT_MAX);
		return static_cast<cint>(Value <= maxValue? Value : maxValue);
	}

};
