#pragma once


struct FQuat : public FFloatStruct
{
	float X, Y, Z, W;

	FORCEINLINE FQuat();
	FORCEINLINE FQuat(float InX, float InY, float InZ, float InW);
	
	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm */

	FORCEINLINE static FQuat FromAxisAngle(const FVec3& Axis, float Angle);

	/* -https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles */

	FORCEINLINE static FQuat FromEuler(const FVec3& Eul);
	FORCEINLINE static FQuat FromEuler(float EulerX, float EulerY, float EulerZ);

	/* -https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm */

	FORCEINLINE FQuat operator*(const FQuat& Other) const;
	FORCEINLINE FQuat& operator*=(const FQuat& Other);

	FORCEINLINE FQuat operator-(const FQuat& Other) const;
	FORCEINLINE FQuat& operator-=(const FQuat& Other);

	FORCEINLINE bool operator==(const FQuat& Other) const;
	FORCEINLINE bool operator!=(const FQuat& Other) const;

	FORCEINLINE bool NearlyEquals(const FQuat& Other, float Difference);

	FORCEINLINE FQuat Inverse() const;
	FORCEINLINE void Invert();
	FORCEINLINE void Normalize();
	FORCEINLINE FQuat Normalized() const;

	/* -https://github.com/microsoft/referencesource/blob/master/System.Numerics/System/Numerics/Quaternion.cs */
	
	FORCEINLINE static FQuat SLerp(const FQuat& A, const FQuat& B, float Factor);
	
	/* -https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles */
	
	FORCEINLINE FVec3 ToEuler() const;

	/* -https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/ */
	
	FORCEINLINE void ToAxisAngle(FVec3& OutAxis, float& OutAngle) const;

	DString ToString() const;
	
};


// -https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/derivations/vectors/index.htm

FORCEINLINE FVec3 operator*(const FVec3& V, const FQuat& Q);


#include "FQuat.inl"