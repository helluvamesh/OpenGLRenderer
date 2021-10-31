#pragma once


class DMath
{
public:

	static const double PI;

	static const double E;


	FORCEINLINE static double Abs(double X)
	{
		return abs(X);
	}
	FORCEINLINE static int32 Abs(int32 X)
	{
		return abs(X);
	}
	FORCEINLINE static int64 Abs(int64 X)
	{
		return abs(X);
	}


	FORCEINLINE static double Acos(double X)
	{
		return acos(DMath::Clamp(X, -1.0, 1.0));
	}


	FORCEINLINE static double Asin(double X)
	{
		return asin(DMath::Clamp(X, -1.0, 1.0));
	}

	
	FORCEINLINE static double Atan(double X)
	{
		return atan(X);
	}


	FORCEINLINE static double Atan2(double Y, double X)
	{
		if (Y == .0 && X == .0)
		{
			return .0;
		}
		else
		{
			return atan2(Y, X);
		}
	}


	FORCEINLINE static double Cbrt(double X)
	{
		return cbrt(fmax(X, .0));
	}


	FORCEINLINE static double Ceil(double X)
	{
		return ceil(X);
	}


	FORCEINLINE static int32 CeilToInt(double X)
	{
		return lround(ceil(X));
	}


	FORCEINLINE static int64 CeilToInt64(double X)
	{
		return llround(ceil(X));
	}


	FORCEINLINE static double Clamp(double Value, double Min, double Max)
	{
		return fmin(fmax(Value, Min), Max);
	}
	FORCEINLINE static int32 Clamp(int32 Value, int32 Min, int32 Max)
	{
		return DMath::Min(DMath::Max(Value, Min), Max);
	}
	FORCEINLINE static int64 Clamp(int64 Value, int64 Min, int64 Max)
	{
		return DMath::Min(DMath::Max(Value, Min), Max);
	}


	FORCEINLINE static double Copysign(double X, double Y)
	{
		return copysign(X, Y);
	}


	FORCEINLINE static double Cos(double X)
	{
		return cos(X);
	}


	FORCEINLINE static double DegToRad(double Degrees)
	{
		return Degrees * 0.017453292519943295;
	}


	FORCEINLINE static double Floor(double X)
	{
		return floor(X);
	}


	FORCEINLINE static int32 FloorToInt(double X)
	{
		return (int32)floor(X);
	}


	FORCEINLINE static int64 FloorToInt64(double X)
	{
		return (int64)floor(X);
	}


	FORCEINLINE static double FMod(double N, double Denom)
	{
		if (Denom != .0)
		{
			return fmod(N, Denom);
		}
		else
		{
			return .0;
		}
	}


	FORCEINLINE static double Frac(double X)
	{
		double f;
		return modf(X, &f);
	}
	

	FORCEINLINE static bool IsInf(double X)
	{
		return std::isinf(X);
	}


	FORCEINLINE static bool IsNaN(double X)
	{
		return std::isnan(X);
	}


	FORCEINLINE static double Lerp(double A, double B, double Factor)
	{
		// slower safe alternatve for integer: A * (1.0f - Factor) + B * Factor
		return A + Factor * (B - A);
	}


	FORCEINLINE static double Log(double X)
	{
		if (X >= .0)
		{
			return log(X);
		}
		else
		{
			return .0;
		}
	}


	FORCEINLINE static double Log2(double X)
	{
		if (X >= .0)
		{
			return log2(X);
		}
		else
		{
			return .0;
		}
	}


	FORCEINLINE static double Log10(double X)
	{
		if (X >= .0)
		{
			return log10(X);
		}
		else
		{
			return .0;
		}
	}


	FORCEINLINE static double LogN(double N, double X)
	{
		if (N >= .0 && X >= .0)
		{
			return log(X) / log(N);
		}
		else
		{
			return .0;
		}
	}


	FORCEINLINE static double Max(double A, double B)
	{
		return fmax(A, B);
	}
	FORCEINLINE static int32 Max(int32 A, int32 B)
	{
		return A > B || A == B ? A : B;
	}
	FORCEINLINE static int64 Max(int64 A, int64 B)
	{
		return A > B || A == B ? A : B;
	}


	FORCEINLINE static double Min(double A, double B)
	{
		return fmin(A, B);
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
	FORCEINLINE static double ModF(double X, double& OutIntPart)
	{
		return modf(X, &OutIntPart);
	}

	FORCEINLINE static bool NearlyEquals(double A, double B, double Difference)
	{
		return abs(A - B) <= Difference; 
	}


	FORCEINLINE static double NthRoot(double N, double X)
	{
		return pow(X, 1.0 / N);
	}


	FORCEINLINE static double PlusMinusOneToZeroOne(double X)
	{
		return (X + 1.0) * 0.5;
	}


	FORCEINLINE static double Pow(double X, double Y)
	{
		return pow(X, Y);
	}


	FORCEINLINE static double ReverseLerp(double A, double B, double Result)
	{
		double v = B - A;
		if (v != .0)
		{
			return (Result - A) / v;
		}
		else
		{
			return A;
		}
	}


	FORCEINLINE static double RadToDeg(double Radians)
	{
		return Radians * 57.29577951308232;
	}


	FORCEINLINE static double Round(double X)
	{
		return round(X);
	}


	FORCEINLINE static int32 RoundToInt(double X)
	{
		return lround(X);
	}


	FORCEINLINE static int64 RoundToInt64(double X)
	{
		return llround(X);
	}


	FORCEINLINE static double Sin(double X)
	{
		return sin(X);
	}


	FORCEINLINE static double Sqrt(double X)
	{
		return sqrt(fmax(X, .0));
	}


	FORCEINLINE static double Tan(double X)
	{
		return tan(X);
	}


	FORCEINLINE static double Truncate(double X)
	{
		return trunc(X);
	}


	FORCEINLINE static double ZeroOneToPlusMinusOne(double X)
	{
		return X * 2.0 - 1.0;
	}

};