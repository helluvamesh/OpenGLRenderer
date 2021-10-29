#pragma once


class FMath
{
public:

	static const float PI;

	static const float E;


	FORCEINLINE static float Abs(float X)
	{
		return fabsf(X);
	}
	FORCEINLINE static int32 Abs(int32 X)
	{
		return abs(X);
	}
	FORCEINLINE static int64 Abs(int64 X)
	{
		return abs(X);
	}


	FORCEINLINE static float Acos(float X)
	{
		return acosf(FMath::Clamp(X, -1.0f, 1.0f));
	}


	FORCEINLINE static float Asin(float X)
	{
		return asinf(FMath::Clamp(X, -1.0f, 1.0f));
	}


	FORCEINLINE static float Atan(float X)
	{
		return atanf(X);
	}


	FORCEINLINE static float Atan2(float Y, float X)
	{
		if (Y == .0f && X == .0f)
		{
			return .0f;
		}
		else
		{
			return atan2f(Y, X);
		}
	}


	FORCEINLINE static float Cbrt(float X)
	{
		return cbrtf(fmaxf(X, .0f));
	}


	FORCEINLINE static float Ceil(float X)
	{
		return ceilf(X);
	}


	FORCEINLINE static int32 CeilToInt(float X)
	{
		return lroundf(ceilf(X));
	}


	FORCEINLINE static int64 CeilToInt64(float X)
	{
		return llroundf(ceilf(X));
	}


	FORCEINLINE static float Clamp(float Value, float Min, float Max)
	{
		return fminf(fmaxf(Value, Min), Max);
	}
	FORCEINLINE static int32 Clamp(int32 Value, int32 Min, int32 Max)
	{
		return FMath::Min(FMath::Max(Value, Min), Max);
	}
	FORCEINLINE static int64 Clamp(int64 Value, int64 Min, int64 Max)
	{
		return FMath::Min(FMath::Max(Value, Min), Max);
	}


	FORCEINLINE static float Copysign(float X, float Y)
	{
		return copysignf(X, Y);
	}


	FORCEINLINE static float Cos(float X)
	{
		return cosf(X);
	}


	FORCEINLINE static float DegToRad(float Degrees)
	{
		return Degrees * 0.017453292519943295f;
	}


	FORCEINLINE static float Floor(float X)
	{
		return floorf(X);
	}


	FORCEINLINE static int32 FloorToInt(float X)
	{
		return (int32)floorf(X);
	}


	FORCEINLINE static int64 FloorToInt64(float X)
	{
		return (int64)floorf(X);
	}


	FORCEINLINE static float FMod(float N, float Denom)
	{
		if (Denom != .0f)
		{
			return fmodf(N, Denom);
		}
		else
		{
			return .0f;
		}
	}


	FORCEINLINE static float Frac(float X)
	{
		float f;
		return modff(X, &f);
	}


	FORCEINLINE static bool IsInf(float X)
	{
		return std::isinf(X);
	}


	FORCEINLINE static bool IsNaN(float X)
	{
		return std::isnan(X);
	}


	FORCEINLINE static float Lerp(float A, float B, float Factor)
	{
		// slower safe alternatve for integer: A * (1.0f - Factor) + B * Factor
		return A + Factor * (B - A);
	}


	FORCEINLINE static float Log(float X)
	{
		if (X >= .0f)
		{
			return logf(X);
		}
		else
		{
			return .0f;
		}
	}


	FORCEINLINE static float Log2(float X)
	{
		if (X >= .0f)
		{
			return log2f(X);
		}
		else
		{
			return .0f;
		}
	}


	FORCEINLINE static float Log10(float X)
	{
		if (X >= .0f)
		{
			return log10f(X);
		}
		else
		{
			return .0f;
		}
	}


	FORCEINLINE static float LogN(float N, float X)
	{
		if (N >= .0f && X >= .0f)
		{
			return logf(X) / logf(N);
		}
		else
		{
			return .0f;
		}
	}


	FORCEINLINE static float Max(float A, float B)
	{
		return fmaxf(A, B);
	}
	FORCEINLINE static int32 Max(int32 A, int32 B)
	{
		return A > B || A == B ? A : B;
	}
	FORCEINLINE static int64 Max(int64 A, int64 B)
	{
		return A > B || A == B ? A : B;
	}


	FORCEINLINE static float Min(float A, float B)
	{
		return fminf(A, B);
	}
	FORCEINLINE static int32 Min(int32 A, int32 B)
	{
		return A > B || A == B ? B : A;
	}
	FORCEINLINE static int64 Min(int64 A, int64 B)
	{
		return A > B || A == B ? B : A;
	}


	/* returns fraction part of X */
	FORCEINLINE static float ModF(float X, float& OutIntPart)
	{
		return modff(X, &OutIntPart);
	}


	FORCEINLINE static bool NearlyEquals(float A, float B, float Difference)
	{
		return abs(A - B) <= Difference; 
	}


	FORCEINLINE static float NthRoot(float N, float X)
	{
		return powf(X, 1.0f / N);
	}


	FORCEINLINE static float PlusMinusOneToZeroOne(float X)
	{
		return (X + 1.0f) * 0.5f;
	}


	FORCEINLINE static float Pow(float X, float Y)
	{
		return powf(X, Y);
	}


	FORCEINLINE static float Pow(float X, int32 Y)
	{
		return pow(X, Y);
	}


	FORCEINLINE static float ReverseLerp(float A, float B, float Result)
	{
		float v = B - A;
		if (v != .0f)
		{
			return (Result - A) / v;
		}
		else
		{
			return A;
		}
	}


	FORCEINLINE static float RadToDeg(float Radians)
	{
		return Radians * 57.29577951308232f;
	}


	FORCEINLINE static float Round(float X)
	{
		return roundf(X);
	}


	FORCEINLINE static int32 RoundToInt(float X)
	{
		return lroundf(X);
	}


	FORCEINLINE static int64 RoundToInt64(float X)
	{
		return llroundf(X);
	}


	FORCEINLINE static float Sin(float X)
	{
		return sinf(X);
	}


	FORCEINLINE static float Sqrt(float X)
	{
		return sqrtf(fmaxf(X, .0f));
	}


	FORCEINLINE static float Tan(float X)
	{
		return tanf(X);
	}


	FORCEINLINE static float Truncate(float X)
	{
		return truncf(X);
	}


	FORCEINLINE static float ZeroOneToPlusMinusOne(float X)
	{
		return X * 2.0f - 1.0f;
	}

};