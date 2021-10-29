#pragma once


struct FVec4 : public FFloatStruct
{
	float X, Y, Z, W;

	FORCEINLINE FVec4();
	FORCEINLINE FVec4(float InX, float InY, float InZ, float InW);
	FORCEINLINE explicit FVec4(float Value);
	FORCEINLINE FVec4(const FVec3& InVec3, float InValue);

	FORCEINLINE bool operator==(const FVec4& Other) const;
	FORCEINLINE bool operator!=(const FVec4& Other) const;

	FORCEINLINE FVec4 operator+(const FVec4& Other) const;
	FORCEINLINE FVec4& operator+=(const FVec4& Other);

	FORCEINLINE FVec4 operator+(float Scalar) const;
	FORCEINLINE FVec4& operator+=(float Scalar);

	FORCEINLINE FVec4 operator-(const FVec4& Other) const;
	FORCEINLINE FVec4& operator-=(const FVec4& Other);

	FORCEINLINE FVec4 operator-(float Scalar) const;
	FORCEINLINE FVec4& operator-=(float Scalar);

	FORCEINLINE FVec4 operator*(const FVec4& Other) const;
	FORCEINLINE FVec4& operator*=(const FVec4& Other);

	FORCEINLINE FVec4 operator*(float Scalar) const;
	FORCEINLINE FVec4& operator*=(float Scalar);

	FORCEINLINE FVec4 operator/(const FVec4& Other) const;
	FORCEINLINE FVec4& operator/=(const FVec4& Other);

	FORCEINLINE FVec4 operator/(float Scalar) const;
	FORCEINLINE FVec4& operator/=(float Scalar);

	FORCEINLINE FVec4 operator-() const;

	FORCEINLINE bool NearlyEquals(const FVec4& Other, float Difference);

	FORCEINLINE FVec4 Pow(float Exponent) const;
	FORCEINLINE FVec4 Pow(const FVec4& Exponents) const;

	FORCEINLINE void Normalize();
	FORCEINLINE FVec4 Normalized() const;
	FORCEINLINE float Length() const;
	FORCEINLINE float Dot(const FVec4& B) const;
	FORCEINLINE FVec4 Cross(const FVec4& B) const;
	FORCEINLINE float Angle(const FVec4& B) const;
	FORCEINLINE float Distance(const FVec4& B) const;
	FORCEINLINE void Invert();
	FORCEINLINE FVec2 XY() const;
	FORCEINLINE FVec3 XYZ() const;

	DString ToString() const;

	FORCEINLINE static FVec4 Lerp(const FVec4& A, const FVec4& B, const float Factor);
};


#include "FVec4.inl"