
FMat4::FMat4() : 
	Data {
		{ 1.0f, .0f, .0f, .0f },
		{ .0f, 1.0f, .0f, .0f },
		{ .0f, .0f, 1.0f, .0f },
		{ .0f, .0f, .0f, 1.0f }
	} 
{
}


FMat4::FMat4(
	float D00, float D01, float D02, float D03,
	float D10, float D11, float D12, float D13,
	float D20, float D21, float D22, float D23,
	float D30, float D31, float D32, float D33
) : 
	Data {
		{ D00, D01, D02, D03 },
		{ D10, D11, D12, D13 },
		{ D20, D21, D22, D23 },
		{ D30, D31, D32, D33 }
	}
{
}


FMat4::FMat4(float V) :
	Data{
		{ V, .0f, .0f, .0f },
		{ .0f, V, .0f, .0f },
		{ .0f, .0f, V, .0f },
		{ .0f, .0f, .0f, V }
	}
{
}


FMat4 FMat4::Translation(const FVec3& Vec)
{
	FMat4 mat;
	mat.Data[0][3] = Vec.X;
	mat.Data[1][3] = Vec.Y;
	mat.Data[2][3] = Vec.Z;
	return mat;
}


FMat4 FMat4::Scaling(const FVec3& Vec)
{
	FMat4 mat;
	mat.Data[0][0] = Vec.X,
	mat.Data[1][1] = Vec.Y;
	mat.Data[2][2] = Vec.Z;
	return mat;
}


FMat4 FMat4::RotationX(float Value)
{
	FMat4 mat;
	mat.Data[1][1] = FMath::Cos(Value);
	mat.Data[1][2] = -FMath::Sin(Value);
	mat.Data[2][1] = FMath::Sin(Value);
	mat.Data[2][2] = FMath::Cos(Value);
	return mat;
}


FMat4 FMat4::RotationY(float Value)
{
	FMat4 mat;
	mat.Data[0][0] = FMath::Cos(Value);
	mat.Data[0][2] = FMath::Sin(Value);
	mat.Data[2][0] = -FMath::Sin(Value);
	mat.Data[2][2] = FMath::Cos(Value);
	return mat;
}


FMat4 FMat4::RotationZ(float Value)
{
	FMat4 mat;
	mat.Data[0][0] = FMath::Cos(Value);
	mat.Data[0][1] = -FMath::Sin(Value);
	mat.Data[1][0] = FMath::Sin(Value);
	mat.Data[1][1] = FMath::Cos(Value);
	return mat;
}


FMat4 FMat4::Rotation(const FVec3& Axis, float Angle)
{
	float cosAngle = FMath::Cos(Angle);
	float oneMinusCosAngle = 1.0f - cosAngle;
	float sinAngle = FMath::Sin(Angle);

	return FMat4(
		Axis.X * Axis.X * oneMinusCosAngle + cosAngle,
		Axis.Y * Axis.X * oneMinusCosAngle - Axis.Z * sinAngle,
		Axis.Z * Axis.X * oneMinusCosAngle + Axis.Y * sinAngle,
		.0f,

		Axis.X * Axis.Y * oneMinusCosAngle + Axis.Z * sinAngle,
		Axis.Y * Axis.Y * oneMinusCosAngle + cosAngle,
		Axis.Z * Axis.Y * oneMinusCosAngle - Axis.X * sinAngle,
		.0f,

		Axis.X * Axis.Z * oneMinusCosAngle - Axis.Y * sinAngle,
		Axis.Y * Axis.Z * oneMinusCosAngle + Axis.X * sinAngle,
		Axis.Z * Axis.Z * oneMinusCosAngle + cosAngle,
		.0f,

		.0f,
		.0f,
		.0f,
		1.0f
	);
}


FMat4 FMat4::Rotation(const FQuat& Q)
{
	return FMat4(
		1.0f - 2.0f * (Q.Y * Q.Y) - 2.0f * (Q.Z * Q.Z),
		2.0f * Q.X * Q.Y - 2.0f * Q.Z * Q.W,
		2.0f * Q.X * Q.Z + 2.0f * Q.Y * Q.W,
		.0f,

		2.0f * Q.X * Q.Y + 2.0f * Q.Z * Q.W,
		1.0f - 2.0f * (Q.X * Q.X) - 2.0f * (Q.Z * Q.Z),
		2.0f * Q.Y * Q.Z - 2.0f * Q.X * Q.W,
		.0f,

		2.0f * Q.X * Q.Z - 2.0f * Q.Y * Q.W,
		2.0f * Q.Y * Q.Z + 2.0f * Q.X * Q.W,
		1.0f - 2.0f * (Q.X * Q.X) - 2.0f * (Q.Y * Q.Y),
		.0f,

		.0f,
		.0f,
		.0f,
		1.0f
	);
}


FMat4 FMat4::FromMat3(const FMat3& Mat)
{
	auto& B = Mat.Data;
	return FMat4(
		B[0][0], B[0][1], B[0][2], .0f,
		B[1][0], B[1][1], B[1][2], .0f,
		B[2][0], B[2][1], B[2][2], .0f,
		.0f, .0f, .0f, 1.0f
	);
}


bool FMat4::operator==(const FMat4& Other) const
{
	for (int32 row = 0; row < 4; row++)
	{
		for (int32 col = 0; col < 4; col++)
		{
			if (this->Data[row][col] != Other.Data[row][col]) return false;
		}
	}
	return true;
}


bool FMat4::operator!=(const FMat4& Other) const
{
	bool equal = true;
	for (int32 row = 0; row < 4; row++)
	{
		for (int32 col = 0; col < 4; col++)
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


FMat4 FMat4::operator*(const FMat4& Other) const
{
	auto& B = Other.Data;
	return FMat4(
		Data[0][0]*B[0][0]  +  Data[0][1]*B[1][0]  +  Data[0][2]*B[2][0]  +  Data[0][3]*B[3][0],
		Data[0][0]*B[0][1]  +  Data[0][1]*B[1][1]  +  Data[0][2]*B[2][1]  +  Data[0][3]*B[3][1],
		Data[0][0]*B[0][2]  +  Data[0][1]*B[1][2]  +  Data[0][2]*B[2][2]  +  Data[0][3]*B[3][2],
		Data[0][0]*B[0][3]  +  Data[0][1]*B[1][3]  +  Data[0][2]*B[2][3]  +  Data[0][3]*B[3][3],

		Data[1][0]*B[0][0]  +  Data[1][1]*B[1][0]  +  Data[1][2]*B[2][0]  +  Data[1][3]*B[3][0],
		Data[1][0]*B[0][1]  +  Data[1][1]*B[1][1]  +  Data[1][2]*B[2][1]  +  Data[1][3]*B[3][1],
		Data[1][0]*B[0][2]  +  Data[1][1]*B[1][2]  +  Data[1][2]*B[2][2]  +  Data[1][3]*B[3][2],
		Data[1][0]*B[0][3]  +  Data[1][1]*B[1][3]  +  Data[1][2]*B[2][3]  +  Data[1][3]*B[3][3],

		Data[2][0]*B[0][0]  +  Data[2][1]*B[1][0]  +  Data[2][2]*B[2][0]  +  Data[2][3]*B[3][0],
		Data[2][0]*B[0][1]  +  Data[2][1]*B[1][1]  +  Data[2][2]*B[2][1]  +  Data[2][3]*B[3][1],
		Data[2][0]*B[0][2]  +  Data[2][1]*B[1][2]  +  Data[2][2]*B[2][2]  +  Data[2][3]*B[3][2],
		Data[2][0]*B[0][3]  +  Data[2][1]*B[1][3]  +  Data[2][2]*B[2][3]  +  Data[2][3]*B[3][3],

		Data[3][0]*B[0][0]  +  Data[3][1]*B[1][0]  +  Data[3][2]*B[2][0]  +  Data[3][3]*B[3][0],
		Data[3][0]*B[0][1]  +  Data[3][1]*B[1][1]  +  Data[3][2]*B[2][1]  +  Data[3][3]*B[3][1],
		Data[3][0]*B[0][2]  +  Data[3][1]*B[1][2]  +  Data[3][2]*B[2][2]  +  Data[3][3]*B[3][2],
		Data[3][0]*B[0][3]  +  Data[3][1]*B[1][3]  +  Data[3][2]*B[2][3]  +  Data[3][3]*B[3][3]
	);
}


FMat4& FMat4::operator*=(const FMat4& Other)
{
	auto& B = Other.Data;
	float _00 = Data[0][0] * B[0][0] + Data[0][1] * B[1][0] + Data[0][2] * B[2][0] + Data[0][3] * B[3][0];
	float _01 = Data[0][0] * B[0][1] + Data[0][1] * B[1][1] + Data[0][2] * B[2][1] + Data[0][3] * B[3][1];
	float _02 = Data[0][0] * B[0][2] + Data[0][1] * B[1][2] + Data[0][2] * B[2][2] + Data[0][3] * B[3][2];
	float _03 = Data[0][0] * B[0][3] + Data[0][1] * B[1][3] + Data[0][2] * B[2][3] + Data[0][3] * B[3][3];

	float _10 = Data[1][0] * B[0][0] + Data[1][1] * B[1][0] + Data[1][2] * B[2][0] + Data[1][3] * B[3][0];
	float _11 = Data[1][0] * B[0][1] + Data[1][1] * B[1][1] + Data[1][2] * B[2][1] + Data[1][3] * B[3][1];
	float _12 = Data[1][0] * B[0][2] + Data[1][1] * B[1][2] + Data[1][2] * B[2][2] + Data[1][3] * B[3][2];
	float _13 = Data[1][0] * B[0][3] + Data[1][1] * B[1][3] + Data[1][2] * B[2][3] + Data[1][3] * B[3][3];

	float _20 = Data[2][0] * B[0][0] + Data[2][1] * B[1][0] + Data[2][2] * B[2][0] + Data[2][3] * B[3][0];
	float _21 = Data[2][0] * B[0][1] + Data[2][1] * B[1][1] + Data[2][2] * B[2][1] + Data[2][3] * B[3][1];
	float _22 = Data[2][0] * B[0][2] + Data[2][1] * B[1][2] + Data[2][2] * B[2][2] + Data[2][3] * B[3][2];
	float _23 = Data[2][0] * B[0][3] + Data[2][1] * B[1][3] + Data[2][2] * B[2][3] + Data[2][3] * B[3][3];

	float _30 = Data[3][0] * B[0][0] + Data[3][1] * B[1][0] + Data[3][2] * B[2][0] + Data[3][3] * B[3][0];
	float _31 = Data[3][0] * B[0][1] + Data[3][1] * B[1][1] + Data[3][2] * B[2][1] + Data[3][3] * B[3][1];
	float _32 = Data[3][0] * B[0][2] + Data[3][1] * B[1][2] + Data[3][2] * B[2][2] + Data[3][3] * B[3][2];
	float _33 = Data[3][0] * B[0][3] + Data[3][1] * B[1][3] + Data[3][2] * B[2][3] + Data[3][3] * B[3][3];

	Data[0][0] = _00;
	Data[0][1] = _01;
	Data[0][2] = _02;
	Data[0][3] = _03;

	Data[1][0] = _10;
	Data[1][1] = _11;
	Data[1][2] = _12;
	Data[1][3] = _13;

	Data[2][0] = _20;
	Data[2][1] = _21;
	Data[2][2] = _22;
	Data[2][3] = _23;

	Data[3][0] = _30;
	Data[3][1] = _31;
	Data[3][2] = _32;
	Data[3][3] = _33;

	return *this;
}


FMat4 FMat4::operator*(float Scalar) const
{
	return FMat4(
		Data[0][0] * Scalar,
		Data[0][1] * Scalar,
		Data[0][2] * Scalar,
		Data[0][3] * Scalar,

		Data[1][0] * Scalar,
		Data[1][1] * Scalar,
		Data[1][2] * Scalar,
		Data[1][3] * Scalar,

		Data[2][0] * Scalar,
		Data[2][1] * Scalar,
		Data[2][2] * Scalar,
		Data[2][3] * Scalar,

		Data[3][0] * Scalar,
		Data[3][1] * Scalar,
		Data[3][2] * Scalar,
		Data[3][3] * Scalar
	);
}


FMat4& FMat4::operator*=(float Scalar)
{
	Data[0][0] *= Scalar;
	Data[0][1] *= Scalar;
	Data[0][2] *= Scalar;
	Data[0][3] *= Scalar;

	Data[1][0] *= Scalar;
	Data[1][1] *= Scalar;
	Data[1][2] *= Scalar;
	Data[1][3] *= Scalar;

	Data[2][0] *= Scalar;
	Data[2][1] *= Scalar;
	Data[2][2] *= Scalar;
	Data[2][3] *= Scalar;

	Data[3][0] *= Scalar;
	Data[3][1] *= Scalar;
	Data[3][2] *= Scalar;
	Data[3][3] *= Scalar;

	return *this;
}


FMat4 FMat4::operator+(const FMat4& Other) const
{
	auto& B = Other.Data;
	return FMat4(
		Data[0][0] + B[0][0], Data[0][1] + B[0][1], Data[0][2] + B[0][2], Data[0][3] + B[0][3],	
		Data[1][0] + B[1][0], Data[1][1] + B[1][1], Data[1][2] + B[1][2], Data[1][3] + B[1][3],	
		Data[2][0] + B[2][0], Data[2][1] + B[2][1], Data[2][2] + B[2][2], Data[2][3] + B[2][3],	
		Data[3][0] + B[3][0], Data[3][1] + B[3][1], Data[3][2] + B[3][2], Data[3][3] + B[3][3]	
	);
}


FMat4& FMat4::operator+=(const FMat4& Other)
{
	auto& B = Other.Data;
	Data[0][0] += B[0][0];
	Data[0][1] += B[0][1];
	Data[0][2] += B[0][2];
	Data[0][3] += B[0][3];

	Data[1][0] += B[1][0];
	Data[1][1] += B[1][1];
	Data[1][2] += B[1][2];
	Data[1][3] += B[1][3];

	Data[2][0] += B[2][0];
	Data[2][1] += B[2][1];
	Data[2][2] += B[2][2];
	Data[2][3] += B[2][3];

	Data[3][0] += B[3][0];
	Data[3][1] += B[3][1];
	Data[3][2] += B[3][2];
	Data[3][3] += B[3][3];
	return *this;
}


FMat4 FMat4::operator-(const FMat4& Other) const
{
	auto& B = Other.Data;
	return FMat4(
		Data[0][0] - B[0][0], Data[0][1] - B[0][1], Data[0][2] - B[0][2], Data[0][3] - B[0][3],	
		Data[1][0] - B[1][0], Data[1][1] - B[1][1], Data[1][2] - B[1][2], Data[1][3] - B[1][3],	
		Data[2][0] - B[2][0], Data[2][1] - B[2][1], Data[2][2] - B[2][2], Data[2][3] - B[2][3],	
		Data[3][0] - B[3][0], Data[3][1] - B[3][1], Data[3][2] - B[3][2], Data[3][3] - B[3][3]	
	);
}


FMat4& FMat4::operator-=(const FMat4& Other)
{
	auto& B = Other.Data;
	Data[0][0] -= B[0][0];
	Data[0][1] -= B[0][1];
	Data[0][2] -= B[0][2];
	Data[0][3] -= B[0][3];

	Data[1][0] -= B[1][0];
	Data[1][1] -= B[1][1];
	Data[1][2] -= B[1][2];
	Data[1][3] -= B[1][3];

	Data[2][0] -= B[2][0];
	Data[2][1] -= B[2][1];
	Data[2][2] -= B[2][2];
	Data[2][3] -= B[2][3];

	Data[3][0] -= B[3][0];
	Data[3][1] -= B[3][1];
	Data[3][2] -= B[3][2];
	Data[3][3] -= B[3][3];
	return *this;
}


bool FMat4::NearlyEquals(const FMat4& Other, float Difference)
{
	auto& B = Other.Data;
	return 
		FMath::Abs(Data[0][0] - Data[0][0]) <= Difference &&
		FMath::Abs(Data[0][1] - Data[0][1]) <= Difference &&
		FMath::Abs(Data[0][2] - Data[0][2]) <= Difference &&
		FMath::Abs(Data[0][3] - Data[0][3]) <= Difference &&

		FMath::Abs(Data[1][0] - Data[1][0]) <= Difference &&
		FMath::Abs(Data[1][1] - Data[1][1]) <= Difference &&
		FMath::Abs(Data[1][2] - Data[1][2]) <= Difference &&
		FMath::Abs(Data[1][3] - Data[1][3]) <= Difference &&

		FMath::Abs(Data[2][0] - Data[2][0]) <= Difference &&
		FMath::Abs(Data[2][1] - Data[2][1]) <= Difference &&
		FMath::Abs(Data[2][2] - Data[2][2]) <= Difference &&
		FMath::Abs(Data[2][3] - Data[2][3]) <= Difference &&

		FMath::Abs(Data[3][0] - Data[3][0]) <= Difference &&
		FMath::Abs(Data[3][1] - Data[3][1]) <= Difference &&
		FMath::Abs(Data[3][2] - Data[3][2]) <= Difference &&
		FMath::Abs(Data[3][3] - Data[3][3]) <= Difference;
}


FVec3 FMat4::ToTranslation() const
{
	return FVec3(
		Data[0][3],
		Data[1][3],
		Data[2][3]
	);
}


FVec3 FMat4::ToScale() const
{
	return FVec3(
		FMath::Sqrt(Data[0][0] * Data[0][0] + Data[0][1] * Data[0][1] + Data[0][2] * Data[0][2]),  // magnitude
		FMath::Sqrt(Data[1][0] * Data[1][0] + Data[1][1] * Data[1][1] + Data[1][2] * Data[1][2]),
		FMath::Sqrt(Data[2][0] * Data[2][0] + Data[2][1] * Data[2][1] + Data[2][2] * Data[2][2])
	);
}


float FMat4::Determinant() const
{
	return	
		Data[0][0] * (
			Data[1][1] * Data[2][2] * Data[3][3] 
			+ Data[1][2] * Data[2][3] * Data[3][1]
			+ Data[1][3] * Data[2][1] * Data[3][2]
			- Data[1][3] * Data[2][2] * Data[3][1]
			- Data[1][2] * Data[2][1] * Data[3][3]
			- Data[1][1] * Data[2][3] * Data[3][2] 
			)
		- Data[1][0] * (
			Data[0][1] * Data[2][2] * Data[3][3]
			+ Data[0][2] * Data[2][3] * Data[3][1]
			+ Data[0][3] * Data[2][1] * Data[3][2]
			- Data[0][3] * Data[2][2] * Data[3][1]
			- Data[0][2] * Data[2][1] * Data[3][3]
			- Data[0][1] * Data[2][3] * Data[3][2]
			)
		+ Data[2][0] * (
			Data[0][1] * Data[1][2] * Data[3][3]
			+ Data[0][2] * Data[1][3] * Data[3][1]
			+ Data[0][3] * Data[1][1] * Data[3][2]
			- Data[0][3] * Data[1][2] * Data[3][1]
			- Data[0][2] * Data[1][1] * Data[3][3]
			- Data[0][1] * Data[1][3] * Data[3][2]
			)
		- Data[3][0] * (
			Data[0][1] * Data[1][2] * Data[2][3]
			+ Data[0][2] * Data[1][3] * Data[2][1]
			+ Data[0][3] * Data[1][1] * Data[2][2]
			- Data[0][3] * Data[1][2] * Data[2][1]
			- Data[0][2] * Data[1][1] * Data[2][3]
			- Data[0][1] * Data[1][3] * Data[2][2]
			);
}


bool FMat4::TryInvert(FMat4& OutMat) const
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
			Data[1][1] * Data[2][2] * Data[3][3]
			+ Data[1][2] * Data[2][3] * Data[3][1]
			+ Data[1][3] * Data[2][1] * Data[3][2]
			- Data[1][3] * Data[2][2] * Data[3][1]
			- Data[1][2] * Data[2][1] * Data[3][3]
			- Data[1][1] * Data[2][3] * Data[3][2]);

		OutMat.Data[0][1] = s * (
			- Data[0][1] * Data[2][2] * Data[3][3]
			- Data[0][2] * Data[2][3] * Data[3][1]
			- Data[0][3] * Data[2][1] * Data[3][2]
			+ Data[0][3] * Data[2][2] * Data[3][1]
			+ Data[0][2] * Data[2][1] * Data[3][3]
			+ Data[0][1] * Data[2][3] * Data[3][2]);

		OutMat.Data[0][2] = s * (
			Data[0][1] * Data[1][2] * Data[3][3]
			+ Data[0][2] * Data[1][3] * Data[3][1]
			+ Data[0][3] * Data[1][1] * Data[3][2]
			- Data[0][3] * Data[1][2] * Data[3][1]
			- Data[0][2] * Data[1][1] * Data[3][3]
			- Data[0][1] * Data[1][3] * Data[3][2]);

		OutMat.Data[0][3] = s * (
			- Data[0][1] * Data[1][2] * Data[2][3]
			- Data[0][2] * Data[1][3] * Data[2][1]
			- Data[0][3] * Data[1][1] * Data[2][2]
			+ Data[0][3] * Data[1][2] * Data[2][1]
			+ Data[0][2] * Data[1][1] * Data[2][3]
			+ Data[0][1] * Data[1][3] * Data[2][2]);

		// -----------------------------------

		OutMat.Data[1][0] = s * (
			- Data[1][0] * Data[2][2] * Data[3][3]
			- Data[1][2] * Data[2][3] * Data[3][0]
			- Data[1][3] * Data[2][0] * Data[3][2]
			+ Data[1][3] * Data[2][2] * Data[3][0]
			+ Data[1][2] * Data[2][0] * Data[3][3]
			+ Data[1][0] * Data[2][3] * Data[3][2]);

		OutMat.Data[1][1] = s * (
			Data[0][0] * Data[2][2] * Data[3][3]
			+ Data[0][2] * Data[2][3] * Data[3][0]
			+ Data[0][3] * Data[2][0] * Data[3][2]
			- Data[0][3] * Data[2][2] * Data[3][0]
			- Data[0][2] * Data[2][0] * Data[3][3]
			- Data[0][0] * Data[2][3] * Data[3][2]);

		OutMat.Data[1][2] = s * (
			- Data[0][0] * Data[1][2] * Data[3][3]
			- Data[0][2] * Data[1][3] * Data[3][0]
			- Data[0][3] * Data[1][0] * Data[3][2]
			+ Data[0][3] * Data[1][2] * Data[3][0]
			+ Data[0][2] * Data[1][0] * Data[3][3]
			+ Data[0][0] * Data[1][3] * Data[3][2]);

		OutMat.Data[1][3] = s * (
			Data[0][0] * Data[1][2] * Data[2][3]
			+ Data[0][2] * Data[1][3] * Data[2][0]
			+ Data[0][3] * Data[1][0] * Data[2][2]
			- Data[0][3] * Data[1][2] * Data[2][0]
			- Data[0][2] * Data[1][0] * Data[2][3]
			- Data[0][0] * Data[1][3] * Data[2][2]);

		// -----------------------------------

		OutMat.Data[2][0] = s * (
			Data[1][0] * Data[2][1] * Data[3][3]
			+ Data[1][1] * Data[2][3] * Data[3][0]
			+ Data[1][3] * Data[2][0] * Data[3][1]
			- Data[1][3] * Data[2][1] * Data[3][0]
			- Data[1][1] * Data[2][0] * Data[3][3]
			- Data[1][0] * Data[2][3] * Data[3][1]);

		OutMat.Data[2][1] = s * (
			- Data[0][0] * Data[2][1] * Data[3][3]
			- Data[0][1] * Data[2][3] * Data[3][0]
			- Data[0][3] * Data[2][0] * Data[3][1]
			+ Data[0][3] * Data[2][1] * Data[3][0]
			+ Data[0][1] * Data[2][0] * Data[3][3]
			+ Data[0][0] * Data[2][3] * Data[3][1]);

		OutMat.Data[2][2] = s * (
			Data[0][0] * Data[1][1] * Data[3][3]
			+ Data[0][1] * Data[1][3] * Data[3][0]
			+ Data[0][3] * Data[1][0] * Data[3][1]
			- Data[0][3] * Data[1][1] * Data[3][0]
			- Data[0][1] * Data[1][0] * Data[3][3]
			- Data[0][0] * Data[1][3] * Data[3][1]);

		OutMat.Data[2][3] = s * (
			- Data[0][0] * Data[1][1] * Data[2][3]
			- Data[0][1] * Data[1][3] * Data[2][0]
			- Data[0][3] * Data[1][0] * Data[2][1]
			+ Data[0][3] * Data[1][1] * Data[2][0]
			+ Data[0][1] * Data[1][0] * Data[2][3]
			+ Data[0][0] * Data[1][3] * Data[2][1]);

		// -----------------------------------

		OutMat.Data[3][0] = s * (
			- Data[1][0] * Data[2][1] * Data[3][2]
			- Data[1][1] * Data[2][2] * Data[3][0]
			- Data[1][2] * Data[2][0] * Data[3][1]
			+ Data[1][2] * Data[2][1] * Data[3][0]
			+ Data[1][1] * Data[2][0] * Data[3][2]
			+ Data[1][0] * Data[2][2] * Data[3][1]);

		OutMat.Data[3][1] = s * (
			Data[0][0] * Data[2][1] * Data[3][2]
			+ Data[0][1] * Data[2][2] * Data[3][0]
			+ Data[0][2] * Data[2][0] * Data[3][1]
			- Data[0][2] * Data[2][1] * Data[3][0]
			- Data[0][1] * Data[2][0] * Data[3][2]
			- Data[0][0] * Data[2][2] * Data[3][1]);

		OutMat.Data[3][2] = s * (
			- Data[0][0] * Data[1][1] * Data[3][2]
			- Data[0][1] * Data[1][2] * Data[3][0]
			- Data[0][2] * Data[1][0] * Data[3][1]
			+ Data[0][2] * Data[1][1] * Data[3][0]
			+ Data[0][1] * Data[1][0] * Data[3][2]
			+ Data[0][0] * Data[1][2] * Data[3][1]);

		OutMat.Data[3][3] = s * (
			Data[0][0] * Data[1][1] * Data[2][2]
			+ Data[0][1] * Data[1][2] * Data[2][0]
			+ Data[0][2] * Data[1][0] * Data[2][1]
			- Data[0][2] * Data[1][1] * Data[2][0]
			- Data[0][1] * Data[1][0] * Data[2][2]
			- Data[0][0] * Data[1][2] * Data[2][1]);

		return true;
	}
}


void FMat4::Transpose()
{
	float d00 = Data[0][0]; float d01 = Data[1][0]; float d02 = Data[2][0]; float d03 = Data[3][0];
	float d10 = Data[0][1]; float d11 = Data[1][1]; float d12 = Data[2][1]; float d13 = Data[3][1];
	float d20 = Data[0][2]; float d21 = Data[1][2]; float d22 = Data[2][2]; float d23 = Data[3][2];
	float d30 = Data[0][3]; float d31 = Data[1][3]; float d32 = Data[2][3]; float d33 = Data[3][3];

	this->Data[0][0] = d00;
	this->Data[0][1] = d01;
	this->Data[0][2] = d02;
	this->Data[0][3] = d03;

	this->Data[1][0] = d10;
	this->Data[1][1] = d11;
	this->Data[1][2] = d12;
	this->Data[1][3] = d13;

	this->Data[2][0] = d20;
	this->Data[2][1] = d21;
	this->Data[2][2] = d22;
	this->Data[2][3] = d23;

	this->Data[3][0] = d30;
	this->Data[3][1] = d31;
	this->Data[3][2] = d32;
	this->Data[3][3] = d33;
}


FMat4 FMat4::Transposed() const
{
	return FMat4(
		Data[0][0], Data[1][0], Data[2][0], Data[3][0],
		Data[0][1], Data[1][1], Data[2][1], Data[3][1],
		Data[0][2], Data[1][2], Data[2][2], Data[3][2],
		Data[0][3], Data[1][3], Data[2][3], Data[3][3]
	);
}


FVec3 FMat4::TransformPos(const FVec3& Vec) const
{
	return FVec3(
		Vec.X * Data[0][0] + Vec.Y * Data[0][1] + Vec.Z * Data[0][2] + Data[0][3],
		Vec.X * Data[1][0] + Vec.Y * Data[1][1] + Vec.Z * Data[1][2] + Data[1][3],
		Vec.X * Data[2][0] + Vec.Y * Data[2][1] + Vec.Z * Data[2][2] + Data[2][3]
	);
}


FVec3 FMat4::TransformDir(const FVec3& Vec) const
{
	FVec3 dir(
		Vec.X * Data[0][0] + Vec.Y * Data[0][1] + Vec.Z * Data[0][2],
		Vec.X * Data[1][0] + Vec.Y * Data[1][1] + Vec.Z * Data[1][2],
		Vec.X * Data[2][0] + Vec.Y * Data[2][1] + Vec.Z * Data[2][2]
	);
	dir.Normalize();
	return dir;
}


void FMat4::ToAxisAngle(FVec3& OutAxis, float& OutAngle) const
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


FQuat FMat4::ToQuat() const
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


FVec3 FMat4::RemoveScaling()
{
	FVec3 scale = this->ToScale();

	float s = scale.X != .0f? scale.X : 1.0f;
	this->Data[0][0] /= s;
	this->Data[0][1] /= s;
	this->Data[0][2] /= s;

	s = scale.Y != .0f ? scale.Y : 1.0f;
	this->Data[1][0] /= s;
	this->Data[1][1] /= s;
	this->Data[1][2] /= s;

	s = scale.Z != .0f ? scale.Z : 1.0f;
	this->Data[2][0] /= s;
	this->Data[2][1] /= s;
	this->Data[2][2] /= s;

	return scale;
}


FMat4 FMat4::ScalingRemoved() const
{
	FVec3 scale = this->ToScale();
	FMat4 m = *this;

	float s = scale.X != .0f ? scale.X : 1.0f;
	m.Data[0][0] /= s;
	m.Data[0][1] /= s;
	m.Data[0][2] /= s;

	s = scale.Y != .0f ? scale.Y : 1.0f;
	m.Data[1][0] /= s;
	m.Data[1][1] /= s;
	m.Data[1][2] /= s;

	s = scale.Z != .0f ? scale.Z : 1.0f;
	m.Data[2][0] /= s;
	m.Data[2][1] /= s;
	m.Data[2][2] /= s;

	return m;
}


void FMat4::Decompose(FVec3& OutLoc, FQuat& OutRot, FVec3& OutScale)
{
	OutLoc = this->ToTranslation();
	FMat4 copy = *this;
	OutScale = copy.RemoveScaling();
	FVec3 axis;
	float angle;
	copy.ToAxisAngle(axis, angle);
	OutRot = FQuat::FromAxisAngle(axis, angle);
}


FMat3 FMat4::ToMat3() const
{
	return FMat3(
		Data[0][0], Data[0][1], Data[0][2],
		Data[1][0], Data[1][1], Data[1][2],
		Data[2][0], Data[2][1], Data[2][2]
	);
}


FVec4 FMat4::Row(int32 Index) const
{
	const float* arr = Data[Index];
	return FVec4(arr[0], arr[1], arr[2], arr[3]);
}


FVec4 FMat4::Column(int32 Index) const
{
	return FVec4(Data[0][Index], Data[1][Index], Data[2][Index], Data[3][Index]);
}


FVec4 operator*(const FVec4& Vec, const FMat4& Mat)
{
	auto& m = Mat.Data;
	return FVec4(
		Vec.X * m[0][0] + Vec.Y * m[0][1] + Vec.Z * m[0][2] + Vec.W * m[0][3],
		Vec.X * m[1][0] + Vec.Y * m[1][1] + Vec.Z * m[1][2] + Vec.W * m[1][3],
		Vec.X * m[2][0] + Vec.Y * m[2][1] + Vec.Z * m[2][2] + Vec.W * m[2][3],
		Vec.X * m[3][0] + Vec.Y * m[3][1] + Vec.Z * m[3][2] + Vec.W * m[3][3]
	);
}


FVec4& operator*=(FVec4& Vec, const FMat4& Mat)
{
	auto& m = Mat.Data;
	float x = Vec.X * m[0][0] + Vec.Y * m[0][1] + Vec.Z * m[0][2] + Vec.W * m[0][3];
	float y = Vec.X * m[1][0] + Vec.Y * m[1][1] + Vec.Z * m[1][2] + Vec.W * m[1][3];
	float z = Vec.X * m[2][0] + Vec.Y * m[2][1] + Vec.Z * m[2][2] + Vec.W * m[2][3];
	float w = Vec.X * m[3][0] + Vec.Y * m[3][1] + Vec.Z * m[3][2] + Vec.W * m[3][3];
	Vec.X = x;
	Vec.Y = y;
	Vec.Z = z;
	Vec.W = w;
	return Vec;
}
