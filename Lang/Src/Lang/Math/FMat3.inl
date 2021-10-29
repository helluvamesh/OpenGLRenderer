
FMat3::FMat3() : 
	Data {
		{ 1.0f, .0f, .0f },
		{ .0f, 1.0f, .0f },
		{ .0f, .0f, 1.0f }
} 
{
}


FMat3::FMat3(
	float D00, float D01, float D02,
	float D10, float D11, float D12,
	float D20, float D21, float D22
) : 
	Data {
		{ D00, D01, D02 },
		{ D10, D11, D12 },
		{ D20, D21, D22 }
}
{
}


FMat3::FMat3(float V) :
	Data{
		{ V, .0f, .0f },
		{ .0f, V, .0f },
		{ .0f, .0f, V }
}
{
}


FMat3 FMat3::RotationX(float Value)
{
	FMat3 mat;
	mat.Data[1][1] = FMath::Cos(Value);
	mat.Data[1][2] = -FMath::Sin(Value);
	mat.Data[2][1] = FMath::Sin(Value);
	mat.Data[2][2] = FMath::Cos(Value);
	return mat;
}


FMat3 FMat3::RotationY(float Value)
{
	FMat3 mat;
	mat.Data[0][0] = FMath::Cos(Value);
	mat.Data[0][2] = FMath::Sin(Value);
	mat.Data[2][0] = -FMath::Sin(Value);
	mat.Data[2][2] = FMath::Cos(Value);
	return mat;
}


FMat3 FMat3::RotationZ(float Value)
{
	FMat3 mat;
	mat.Data[0][0] = FMath::Cos(Value);
	mat.Data[0][1] = -FMath::Sin(Value);
	mat.Data[1][0] = FMath::Sin(Value);
	mat.Data[1][1] = FMath::Cos(Value);
	return mat;
}


FMat3 FMat3::Rotation(const FVec3& Axis, float Angle)
{
	float cosAngle = FMath::Cos(Angle);
	float oneMinusCosAngle = 1.0f - cosAngle;
	float sinAngle = FMath::Sin(Angle);

	return FMat3(
		Axis.X * Axis.X * oneMinusCosAngle + cosAngle,
		Axis.Y * Axis.X * oneMinusCosAngle - Axis.Z * sinAngle,
		Axis.Z * Axis.X * oneMinusCosAngle + Axis.Y * sinAngle,

		Axis.X * Axis.Y * oneMinusCosAngle + Axis.Z * sinAngle,
		Axis.Y * Axis.Y * oneMinusCosAngle + cosAngle,
		Axis.Z * Axis.Y * oneMinusCosAngle - Axis.X * sinAngle,

		Axis.X * Axis.Z * oneMinusCosAngle - Axis.Y * sinAngle,
		Axis.Y * Axis.Z * oneMinusCosAngle + Axis.X * sinAngle,
		Axis.Z * Axis.Z * oneMinusCosAngle + cosAngle
	);
}


FMat3 FMat3::Rotation(const FQuat& Q)
{
	return FMat3(
		1.0f - 2.0f * (Q.Y * Q.Y) - 2.0f * (Q.Z * Q.Z),
		2.0f * Q.X * Q.Y - 2.0f * Q.Z * Q.W,
		2.0f * Q.X * Q.Z + 2.0f * Q.Y * Q.W,

		2.0f * Q.X * Q.Y + 2.0f * Q.Z * Q.W,
		1.0f - 2.0f * (Q.X * Q.X) - 2.0f * (Q.Z * Q.Z),
		2.0f * Q.Y * Q.Z - 2.0f * Q.X * Q.W,

		2.0f * Q.X * Q.Z - 2.0f * Q.Y * Q.W,
		2.0f * Q.Y * Q.Z + 2.0f * Q.X * Q.W,
		1.0f - 2.0f * (Q.X * Q.X) - 2.0f * (Q.Y * Q.Y)
	);
}


bool FMat3::operator==(const FMat3& Other) const
{
	for (int32 row = 0; row < 3; row++)
	{
		for (int32 col = 0; col < 3; col++)
		{
			if (this->Data[row][col] != Other.Data[row][col]) return false;
		}
	}
	return true;
}


bool FMat3::operator!=(const FMat3& Other) const
{
	bool equal = true;
	for (int32 row = 0; row < 3; row++)
	{
		for (int32 col = 0; col < 3; col++)
		{
			if (this->Data[row][col] != Other.Data[row][col])
			{
				equal = false;
				break;
			}
		}
	}
	return !equal;
}


FMat3 FMat3::operator*(const FMat3& Other) const
{
	auto& B = Other.Data;
	return FMat3(
		Data[0][0]*B[0][0]  +  Data[0][1]*B[1][0]  +  Data[0][2]*B[2][0],
		Data[0][0]*B[0][1]  +  Data[0][1]*B[1][1]  +  Data[0][2]*B[2][1],
		Data[0][0]*B[0][2]  +  Data[0][1]*B[1][2]  +  Data[0][2]*B[2][2],

		Data[1][0]*B[0][0]  +  Data[1][1]*B[1][0]  +  Data[1][2]*B[2][0],
		Data[1][0]*B[0][1]  +  Data[1][1]*B[1][1]  +  Data[1][2]*B[2][1],
		Data[1][0]*B[0][2]  +  Data[1][1]*B[1][2]  +  Data[1][2]*B[2][2],

		Data[2][0]*B[0][0]  +  Data[2][1]*B[1][0]  +  Data[2][2]*B[2][0],
		Data[2][0]*B[0][1]  +  Data[2][1]*B[1][1]  +  Data[2][2]*B[2][1],
		Data[2][0]*B[0][2]  +  Data[2][1]*B[1][2]  +  Data[2][2]*B[2][2]
	);
}


FMat3& FMat3::operator*=(const FMat3& Other)
{
	auto& B = Other.Data;
	float _00 = Data[0][0] * B[0][0] + Data[0][1] * B[1][0] + Data[0][2] * B[2][0];
	float _01 = Data[0][0] * B[0][1] + Data[0][1] * B[1][1] + Data[0][2] * B[2][1];
	float _02 = Data[0][0] * B[0][2] + Data[0][1] * B[1][2] + Data[0][2] * B[2][2];

	float _10 = Data[1][0] * B[0][0] + Data[1][1] * B[1][0] + Data[1][2] * B[2][0];
	float _11 = Data[1][0] * B[0][1] + Data[1][1] * B[1][1] + Data[1][2] * B[2][1];
	float _12 = Data[1][0] * B[0][2] + Data[1][1] * B[1][2] + Data[1][2] * B[2][2];

	float _20 = Data[2][0] * B[0][0] + Data[2][1] * B[1][0] + Data[2][2] * B[2][0];
	float _21 = Data[2][0] * B[0][1] + Data[2][1] * B[1][1] + Data[2][2] * B[2][1];
	float _22 = Data[2][0] * B[0][2] + Data[2][1] * B[1][2] + Data[2][2] * B[2][2];

	Data[0][0] = _00;
	Data[0][1] = _01;
	Data[0][2] = _02;

	Data[1][0] = _10;
	Data[1][1] = _11;
	Data[1][2] = _12;

	Data[2][0] = _20;
	Data[2][1] = _21;
	Data[2][2] = _22;

	return *this;
}


FMat3 FMat3::operator*(float Scalar) const
{
	return FMat3(
		Data[0][0] * Scalar,
		Data[0][1] * Scalar,
		Data[0][2] * Scalar,

		Data[1][0] * Scalar,
		Data[1][1] * Scalar,
		Data[1][2] * Scalar,

		Data[2][0] * Scalar,
		Data[2][1] * Scalar,
		Data[2][2] * Scalar
	);
}


FMat3& FMat3::operator*=(float Scalar)
{
	Data[0][0] *= Scalar;
	Data[0][1] *= Scalar;
	Data[0][2] *= Scalar;

	Data[1][0] *= Scalar;
	Data[1][1] *= Scalar;
	Data[1][2] *= Scalar;

	Data[2][0] *= Scalar;
	Data[2][1] *= Scalar;
	Data[2][2] *= Scalar;

	return *this;
}


FMat3 FMat3::operator+(const FMat3& Other) const
{
	auto& B = Other.Data;
	return FMat3(
		Data[0][0] + B[0][0], Data[0][1] + B[0][1], Data[0][2] + B[0][2],
		Data[1][0] + B[1][0], Data[1][1] + B[1][1], Data[1][2] + B[1][2],	
		Data[2][0] + B[2][0], Data[2][1] + B[2][1], Data[2][2] + B[2][2]
	);
}


FMat3& FMat3::operator+=(const FMat3& Other)
{
	auto& B = Other.Data;
	Data[0][0] += B[0][0];
	Data[0][1] += B[0][1];
	Data[0][2] += B[0][2];

	Data[1][0] += B[1][0];
	Data[1][1] += B[1][1];
	Data[1][2] += B[1][2];

	Data[2][0] += B[2][0];
	Data[2][1] += B[2][1];
	Data[2][2] += B[2][2];

	return *this;
}


FMat3 FMat3::operator-(const FMat3& Other) const
{
	auto& B = Other.Data;
	return FMat3(
		Data[0][0] - B[0][0], Data[0][1] - B[0][1], Data[0][2] - B[0][2],
		Data[1][0] - B[1][0], Data[1][1] - B[1][1], Data[1][2] - B[1][2],	
		Data[2][0] - B[2][0], Data[2][1] - B[2][1], Data[2][2] - B[2][2]
	);
}


FMat3& FMat3::operator-=(const FMat3& Other)
{
	auto& B = Other.Data;
	Data[0][0] -= B[0][0];
	Data[0][1] -= B[0][1];
	Data[0][2] -= B[0][2];

	Data[1][0] -= B[1][0];
	Data[1][1] -= B[1][1];
	Data[1][2] -= B[1][2];

	Data[2][0] -= B[2][0];
	Data[2][1] -= B[2][1];
	Data[2][2] -= B[2][2];

	return *this;
}


bool FMat3::NearlyEquals(const FMat3& Other, float Difference)
{
	auto& B = Other.Data;
	return 
		FMath::Abs(Data[0][0] - Data[0][0]) <= Difference &&
		FMath::Abs(Data[0][1] - Data[0][1]) <= Difference &&
		FMath::Abs(Data[0][2] - Data[0][2]) <= Difference &&

		FMath::Abs(Data[1][0] - Data[1][0]) <= Difference &&
		FMath::Abs(Data[1][1] - Data[1][1]) <= Difference &&
		FMath::Abs(Data[1][2] - Data[1][2]) <= Difference &&

		FMath::Abs(Data[2][0] - Data[2][0]) <= Difference &&
		FMath::Abs(Data[2][1] - Data[2][1]) <= Difference &&
		FMath::Abs(Data[2][2] - Data[2][2]) <= Difference;
}


float FMat3::Determinant() const
{
	return
		Data[0][0] * (
			Data[1][1] * Data[2][2]
			- Data[1][2] * Data[2][1] 
			)
		- Data[0][1] * (
			Data[1][0] * Data[2][2]
			- Data[1][2] * Data[2][0]
			)
		+ Data[0][2] * (
			Data[1][0] * Data[2][1]
			- Data[1][1] * Data[2][0]
			);
}


bool FMat3::TryInvert(FMat3& OutMat) const
{
	float det = this->Determinant();
	if (det == .0f)
	{
		return false;
	}
	else
	{
		float s = 1.0f / det;

		OutMat.Data[0][0] = s * (
			Data[1][1] * Data[2][2]
			- Data[1][2] * Data[2][1]);

		OutMat.Data[0][1] = s * (
			- Data[0][1] * Data[2][2]
			+ Data[0][2] * Data[2][1]);

		OutMat.Data[0][2] = s * (
			Data[0][1] * Data[1][2]
			- Data[0][2] * Data[1][1]);

		// -----------------------------------

		OutMat.Data[1][0] = s * (
			- Data[1][0] * Data[2][2]
			+ Data[1][2] * Data[2][0]);

		OutMat.Data[1][1] = s * (
			Data[0][0] * Data[2][2]
			- Data[0][2] * Data[2][0]);

		OutMat.Data[1][2] = s * (
			- Data[0][0] * Data[1][2]
			+ Data[0][2] * Data[1][0]);

		// -----------------------------------

		OutMat.Data[2][0] = s * (
			Data[1][0] * Data[2][1]
			- Data[1][1] * Data[2][0]);

		OutMat.Data[2][1] = s * (
			- Data[0][0] * Data[2][1]
			+ Data[0][1] * Data[2][0]);

		OutMat.Data[2][2] = s * (
			Data[0][0] * Data[1][1]
			- Data[0][1] * Data[1][0]);

		return true;
	}
}


void FMat3::Transpose()
{
	float d00 = Data[0][0]; float d01 = Data[1][0]; float d02 = Data[2][0];
	float d10 = Data[0][1]; float d11 = Data[1][1]; float d12 = Data[2][1];
	float d20 = Data[0][2]; float d21 = Data[1][2]; float d22 = Data[2][2];

	this->Data[0][0] = d00;
	this->Data[0][1] = d01;
	this->Data[0][2] = d02;

	this->Data[1][0] = d10;
	this->Data[1][1] = d11;
	this->Data[1][2] = d12;

	this->Data[2][0] = d20;
	this->Data[2][1] = d21;
	this->Data[2][2] = d22;
}


FMat3 FMat3::Transposed() const
{
	return FMat3(
		Data[0][0], Data[1][0], Data[2][0],
		Data[0][1], Data[1][1], Data[2][1],
		Data[0][2], Data[1][2], Data[2][2]
	);
}


void FMat3::ToAxisAngle(FVec3& OutAxis, float& OutAngle) const
{
	float angle, x, y, z; // variables for result
	float epsilon = 0.01f; // margin to allow for rounding errors
	float epsilon2 = 0.1f; // margin to distinguish between 0 and 180 degrees

	if ((FMath::Abs(Data[0][1] - Data[1][0]) < epsilon)
		&& (FMath::Abs(Data[0][2] - Data[2][0]) < epsilon)
		&& (FMath::Abs(Data[1][2] - Data[2][1]) < epsilon)) 
	{
		// singularity found
		// first check for identity matrix which must have +1 for all terms
		//  in leading diagonal and zero in other terms
		if ((FMath::Abs(Data[0][1] + Data[1][0]) < epsilon2)
			&& (FMath::Abs(Data[0][2] + Data[2][0]) < epsilon2)
			&& (FMath::Abs(Data[1][2] + Data[2][1]) < epsilon2)
			&& (FMath::Abs(Data[0][0] + Data[1][1] + Data[2][2] - 3) < epsilon2))
		{
			// this singularity is identity matrix so angle = 0
			OutAxis.X = 1;
			OutAxis.Y = 0;
			OutAxis.Z = 0;
			OutAngle = 0;
			return; // zero angle, arbitrary axis
		}
		// otherwise this singularity is angle = 180
		angle = FMath::PI;
		float xx = (Data[0][0] + 1) / 2;
		float yy = (Data[1][1] + 1) / 2;
		float zz = (Data[2][2] + 1) / 2;
		float xy = (Data[0][1] + Data[1][0]) / 4;
		float xz = (Data[0][2] + Data[2][0]) / 4;
		float yz = (Data[1][2] + Data[2][1]) / 4;
		if ((xx > yy) && (xx > zz))  // m[0][0] is the largest diagonal term
		{ 
			if (xx < epsilon) 
			{
				x = .0f;
				y = 0.7071f;
				z = 0.7071f;
			}
			else 
			{
				x = FMath::Sqrt(xx);
				y = xy / x;
				z = xz / x;
			}
		}
		else if (yy > zz)  // m[1][1] is the largest diagonal term 
		{ 
			if (yy < epsilon) 
			{
				x = 0.7071f;
				y = .0f;
				z = 0.7071f;
			}
			else
			{
				y = FMath::Sqrt(yy);
				x = xy / y;
				z = yz / y;
			}
		}
		else  // m[2][2] is the largest diagonal term so base result on this
		{ 
			if (zz < epsilon) 
			{
				x = 0.7071f;
				y = 0.7071f;
				z = .0f;
			}
			else
			{
				z = FMath::Sqrt(zz);
				x = xz / z;
				y = yz / z;
			}
		}

		OutAngle = angle;
		OutAxis.X = x;
		OutAxis.Y = y;
		OutAxis.Z = z;
		return; // return 180 deg rotation
	}
	// as we have reached here there are no singularities so we can handle normally
	float s = FMath::Sqrt((Data[2][1] - Data[1][2]) * (Data[2][1] - Data[1][2])
		+ (Data[0][2] - Data[2][0]) * (Data[0][2] - Data[2][0])
		+ (Data[1][0] - Data[0][1]) * (Data[1][0] - Data[0][1])); // used to normalise
	if (FMath::Abs(s) == .0f) s = 1;
	// prevent divide by zero, should not happen if matrix is orthogonal and should be
	// caught by singularity test above, but I've left it in just in case
	angle = FMath::Acos((Data[0][0] + Data[1][1] + Data[2][2] - 1) / 2);
	x = (Data[2][1] - Data[1][2]) / s;
	y = (Data[0][2] - Data[2][0]) / s;
	z = (Data[1][0] - Data[0][1]) / s;

	OutAngle = angle;
	OutAxis.X = x;
	OutAxis.Y = y;
	OutAxis.Z = z;
	return;
}


FQuat FMat3::ToQuat() const
{
	float trace = Data[0][0] + Data[1][1] + Data[2][2];
	if (trace > 0)
	{
		float sq = FMath::Sqrt(trace + 1.0f);
		float s = sq != .0f? 0.5f / sq : 1.0f;
		FQuat q(
			(Data[2][1] - Data[1][2]) * s,
			(Data[0][2] - Data[2][0]) * s,
			(Data[1][0] - Data[0][1]) * s,
			0.25f / s
		);
		q.Normalize();
		return q;
	}
	else
	{
		if (Data[0][0] > Data[1][1] && Data[0][0] > Data[2][2])
		{
			float s = 2.0f * FMath::Sqrt(1.0f + Data[0][0] - Data[1][1] - Data[2][2]);
			if (s == .0f) s = 1.0f;
			FQuat q(
				0.25f * s,
				(Data[0][1] + Data[1][0]) / s,
				(Data[0][2] + Data[2][0]) / s,
				(Data[2][1] - Data[1][2]) / s
			);
			q.Normalize();
			return q;
		}
		else if (Data[1][1] > Data[2][2])
		{
			float s = 2.0f * FMath::Sqrt(1.0f + Data[1][1] - Data[0][0] - Data[2][2]);
			if (s == .0f) s = 1.0f;
			FQuat q(
				(Data[0][1] + Data[1][0]) / s,
				0.25f * s,
				(Data[1][2] + Data[2][1]) / s,
				(Data[0][2] - Data[2][0]) / s
			);
			q.Normalize();
			return q;
		}
		else
		{
			float s = 2.0f * FMath::Sqrt(1.0f + Data[2][2] - Data[0][0] - Data[1][1]);
			if (s == .0f) s = 1.0f;
			FQuat q(
				(Data[0][2] + Data[2][0]) / s,
				(Data[1][2] + Data[2][1]) / s,
				0.25f * s,
				(Data[1][0] - Data[0][1]) / s
			);
			q.Normalize();
			return q;
		}
	}
}


FVec3 FMat3::Row(int32 Index) const
{
	const float* arr = Data[Index];
	return FVec3(arr[0], arr[1], arr[2]);
}


FVec3 FMat3::Column(int32 Index) const
{
	return FVec3(Data[0][Index], Data[1][Index], Data[2][Index]);
}


FVec3 operator*(const FVec3& Vec, const FMat3& Mat)
{
	const auto& m = Mat.Data;
	return FVec3(
		Vec.X * m[0][0] + Vec.Y * m[0][1] + Vec.Z * m[0][2],
		Vec.X * m[1][0] + Vec.Y * m[1][1] + Vec.Z * m[1][2],
		Vec.X * m[2][0] + Vec.Y * m[2][1] + Vec.Z * m[2][2]
	);
}


FVec3& operator*=(FVec3& Vec, const FMat3& Mat)
{
	const auto& m = Mat.Data;
	float x = Vec.X * m[0][0] + Vec.Y * m[0][1] + Vec.Z * m[0][2];
	float y = Vec.X * m[1][0] + Vec.Y * m[1][1] + Vec.Z * m[1][2];
	float z = Vec.X * m[2][0] + Vec.Y * m[2][1] + Vec.Z * m[2][2];
	Vec.X = x;
	Vec.Y = y;
	Vec.Z = z;
	return Vec;
}
