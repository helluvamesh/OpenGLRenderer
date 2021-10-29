#pragma once


struct FMat4 : public FFloatStruct
{
	static const FMat4 DEFAULT;

	float Data[4][4];

	FORCEINLINE FMat4();
	FORCEINLINE FMat4(
		float D00, float D01, float D02, float D03,
		float D10, float D11, float D12, float D13,
		float D20, float D21, float D22, float D23,
		float D30, float D31, float D32, float D33
	);
	FORCEINLINE explicit FMat4(float V);
	
	/* -https://learnopengl.com/Getting-started/Transformations */

	FORCEINLINE static FMat4 Translation(const FVec3& Vec);
	FORCEINLINE static FMat4 Scaling(const FVec3& Vec);
	FORCEINLINE static FMat4 RotationX(float Value);
	FORCEINLINE static FMat4 RotationY(float Value);
	FORCEINLINE static FMat4 RotationZ(float Value);
	FORCEINLINE static FMat4 Rotation(const FVec3& Axis, float Angle);

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm */
	
	FORCEINLINE static FMat4 Rotation(const FQuat& Q);

	FORCEINLINE static FMat4 FromMat3(const FMat3& Mat);

	FORCEINLINE bool operator==(const FMat4& Other) const;
	FORCEINLINE bool operator!=(const FMat4& Other) const;

	/* -https://learnopengl.com/Getting-started/Transformations */
	
	FORCEINLINE FMat4 operator*(const FMat4& Other) const;
	FORCEINLINE FMat4& operator*=(const FMat4& Other);

	FORCEINLINE FMat4 operator*(const float Scalar) const;
	FORCEINLINE FMat4& operator*=(const float Scalar);

	FORCEINLINE FMat4 operator+(const FMat4& Other) const;
	FORCEINLINE FMat4& operator+=(const FMat4& Other);

	FORCEINLINE FMat4 operator-(const FMat4& Other) const;
	FORCEINLINE FMat4& operator-=(const FMat4& Other);

	FORCEINLINE bool NearlyEquals(const FMat4& Other, float Difference);

	FORCEINLINE FVec3 ToTranslation() const;
	FORCEINLINE FVec3 ToScale() const;

	/* -https://semath.info/src/inverse-cofactor-ex4.html */

	FORCEINLINE float Determinant() const;

	/* -https://semath.info/src/inverse-cofactor-ex4.html */

	/* Matrix * inverseMatrix = identityMatrix */
	FORCEINLINE bool TryInvert(FMat4& OutMat) const;

	FORCEINLINE void Transpose();
	FORCEINLINE FMat4 Transposed() const;

	FORCEINLINE FVec3 TransformPos(const FVec3& Vec) const;

	/* ignore translation of matrix when multiplying */
	FORCEINLINE FVec3 TransformDir(const FVec3& Vec) const;

	FORCEINLINE FVec3 RemoveScaling();
	FORCEINLINE FMat4 ScalingRemoved() const;
	FORCEINLINE void Decompose(FVec3& OutLoc, FQuat& OutRot, FVec3& OutScale);

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm */

	/* if the matrix contains scaling, it should be removed first */
	FORCEINLINE void ToAxisAngle(FVec3& OutAxis, float& OutAngle) const;

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/ */

	/* if the matrix contains scaling, it should be removed first */
	FORCEINLINE FQuat ToQuat() const;

	FORCEINLINE FMat3 ToMat3() const;

	FORCEINLINE FVec4 Row(int32 Index) const;
	FORCEINLINE FVec4 Column(int32 Index) const;

	DString ToString() const;
};


FORCEINLINE FVec4 operator*(const FVec4& Vec, const FMat4& Mat);
FORCEINLINE FVec4& operator*=(FVec4& Vec, const FMat4& Mat);


#include "FMat4.inl"