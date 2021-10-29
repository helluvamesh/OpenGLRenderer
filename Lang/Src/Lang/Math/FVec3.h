#pragma once


struct FVec3 : public FFloatStruct
{
	static const FVec3 DEFAULT;

	float X, Y, Z;

	FORCEINLINE FVec3();
	FORCEINLINE FVec3(float InX, float InY, float InZ);
	FORCEINLINE explicit FVec3(float Value);
	FORCEINLINE FVec3(FVec2 Vec2, float InZ);

	FORCEINLINE bool operator==(const FVec3& Other) const;
	FORCEINLINE bool operator!=(const FVec3& Other) const;

	FORCEINLINE FVec3 operator+(const FVec3& Other) const;
	FORCEINLINE FVec3& operator+=(const FVec3& Other);

	FORCEINLINE FVec3 operator+(float Scalar) const;
	FORCEINLINE FVec3& operator+=(float Scalar);

	FORCEINLINE FVec3 operator-(const FVec3& Other) const;
	FORCEINLINE FVec3& operator-=(const FVec3& Other);

	FORCEINLINE FVec3 operator-(float Scalar) const;
	FORCEINLINE FVec3& operator-=(float Scalar);

	FORCEINLINE FVec3 operator*(const FVec3& Other) const;
	FORCEINLINE FVec3& operator*=(const FVec3& Other);

	FORCEINLINE FVec3 operator*(float Scalar) const;
	FORCEINLINE FVec3& operator*=(float Scalar);

	FORCEINLINE FVec3 operator/(const FVec3& Other) const;
	FORCEINLINE FVec3& operator/=(const FVec3& Other);

	FORCEINLINE FVec3 operator/(float Scalar) const;
	FORCEINLINE FVec3& operator/=(float Scalar);

	FORCEINLINE FVec3 operator-() const;

	FORCEINLINE bool NearlyEquals(const FVec3& Other, float Difference);

	FORCEINLINE FVec3 Pow(float Exponent) const;
	FORCEINLINE FVec3 Pow(const FVec3& Exponents) const;

	FORCEINLINE void Normalize();
	FORCEINLINE FVec3 Normalized() const;
	FORCEINLINE float Length() const;
	FORCEINLINE float Dot(const FVec3& B) const;
	FORCEINLINE FVec3 Cross(const FVec3& B) const;
	FORCEINLINE float Angle(const FVec3& B) const;
	FORCEINLINE float Distance(const FVec3& B) const;
	FORCEINLINE void Invert();
	FORCEINLINE FVec2 XY() const;

	DString ToString() const;

	FORCEINLINE static FVec3 Lerp(const FVec3& A, const FVec3& B, const float Factor);
};


#include "FVec3.inl"