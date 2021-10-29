#pragma once


struct FMat3 : public FFloatStruct
{
	float Data[3][3];

	FORCEINLINE FMat3();
	FORCEINLINE FMat3(
		float D00, float D01, float D02,
		float D10, float D11, float D12,
		float D20, float D21, float D22
	);
	FORCEINLINE explicit FMat3(float V);

	FORCEINLINE static FMat3 RotationX(float Value);
	FORCEINLINE static FMat3 RotationY(float Value);
	FORCEINLINE static FMat3 RotationZ(float Value);
	FORCEINLINE static FMat3 Rotation(const FVec3& Axis, float Angle);

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm */

	FORCEINLINE static FMat3 Rotation(const FQuat& Q);

	FORCEINLINE bool operator==(const FMat3& Other) const;
	FORCEINLINE bool operator!=(const FMat3& Other) const;

	FORCEINLINE FMat3 operator*(const FMat3& Other) const;
	FORCEINLINE FMat3& operator*=(const FMat3& Other);

	FORCEINLINE FMat3 operator*(const float Scalar) const;
	FORCEINLINE FMat3& operator*=(const float Scalar);

	FORCEINLINE FMat3 operator+(const FMat3& Other) const;
	FORCEINLINE FMat3& operator+=(const FMat3& Other);

	FORCEINLINE FMat3 operator-(const FMat3& Other) const;
	FORCEINLINE FMat3& operator-=(const FMat3& Other);

	FORCEINLINE bool NearlyEquals(const FMat3& Other, float Difference);

	/* -https://semath.info/src/inverse-cofactor-ex4.html */

	FORCEINLINE float Determinant() const;

	/* -https://semath.info/src/inverse-cofactor-ex4.html */

	/* Matrix * inverseMatrix = identityMatrix */
	FORCEINLINE bool TryInvert(FMat3& OutMat) const;

	FORCEINLINE void Transpose();
	FORCEINLINE FMat3 Transposed() const;

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm */

	/* if the matrix contains scaling, it should be removed first */
	FORCEINLINE void ToAxisAngle(FVec3& OutAxis, float& OutAngle) const;

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ */

	/* if the matrix contains scaling, it should be removed first */
	FORCEINLINE FQuat ToQuat() const;

	FORCEINLINE FVec3 Row(int32 Index) const;
	FORCEINLINE FVec3 Column(int32 Index) const;

	DString ToString() const;

};

/* -https://learnopengl.com/Getting-started/Transformations */

FORCEINLINE FVec3 operator*(const FVec3& Vec, const FMat3& Mat);
FORCEINLINE FVec3& operator*=(FVec3& Vec, const FMat3& Mat);


#include "FMat3.inl"