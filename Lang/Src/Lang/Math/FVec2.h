#pragma once


struct FVec2 : public FFloatStruct
{
	static const FVec2 DEFAULT;

	float X, Y;

	FORCEINLINE FVec2();
	FORCEINLINE FVec2(float InX, float InY);
	FORCEINLINE explicit FVec2(float Value);

	FORCEINLINE bool operator==(const FVec2& Other) const;
	FORCEINLINE bool operator!=(const FVec2& Other) const;

	FORCEINLINE FVec2 operator+(const FVec2& Other) const;
	FORCEINLINE FVec2& operator+=(const FVec2& Other);

	FORCEINLINE FVec2 operator+(float Scalar) const;
	FORCEINLINE FVec2& operator+=(float Scalar);

	FORCEINLINE FVec2 operator-(const FVec2& Other) const;
	FORCEINLINE FVec2& operator-=(const FVec2& Other);

	FORCEINLINE FVec2 operator-(float Scalar) const;
	FORCEINLINE FVec2& operator-=(float Scalar);

	FORCEINLINE FVec2 operator*(const FVec2& Other) const;
	FORCEINLINE FVec2& operator*=(const FVec2& Other);

	FORCEINLINE FVec2 operator*(float Scalar) const;
	FORCEINLINE FVec2& operator*=(float Scalar);

	FORCEINLINE FVec2 operator/(FVec2 Other) const;
	FORCEINLINE FVec2& operator/=(FVec2 Other);

	FORCEINLINE FVec2 operator/(float Scalar) const;
	FORCEINLINE FVec2& operator/=(float Scalar);

	FORCEINLINE FVec2 operator-() const;

	FORCEINLINE bool NearlyEquals(const FVec2& Other, float Difference);

	FORCEINLINE FVec2 Pow(float Exponent) const;
	FORCEINLINE FVec2 Pow(FVec2 Exponents) const;
	
	FORCEINLINE void Normalize();
	FORCEINLINE FVec2 Normalized() const;
	FORCEINLINE float Length() const;
	FORCEINLINE float Dot(const FVec2& B) const;
	FORCEINLINE float Angle(const FVec2& B) const;
	FORCEINLINE float SignedAngle(const FVec2& B) const;
	FORCEINLINE float Distance(const FVec2& B) const;
	FORCEINLINE void Invert();

	FORCEINLINE static FVec2 Lerp(const FVec2& A, const FVec2& B, const float Factor);

	DString ToString() const;
};


#include "FVec2.inl"