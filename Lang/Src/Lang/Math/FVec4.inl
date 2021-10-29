
FVec4::FVec4() : X(.0f), Y(.0f), Z(.0f), W(.0f) { }

FVec4::FVec4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }

FVec4::FVec4(float Value) : X(Value), Y(Value), Z(Value), W(Value) { }

FVec4::FVec4(const FVec3& InVec3, float InW) : X(InVec3.X), Y(InVec3.Y), Z(InVec3.Z), W(InW) { }


bool FVec4::operator==(const FVec4& Other) const
{
	return this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z && this->W == Other.W;
}
bool FVec4::operator!=(const FVec4& Other) const
{
	return this->X != Other.X || this->Y != Other.Y || this->Z != Other.Z || this->W != Other.W;
}


FVec4 FVec4::operator+(const FVec4& Other) const
{
	return FVec4(this->X + Other.X, this->Y + Other.Y, this->Z + Other.Z, this->W + Other.W);
}
FVec4& FVec4::operator+=(const FVec4& Other)
{
	this->X += Other.X;
	this->Y += Other.Y; 
	this->Z += Other.Z;
	this->W += Other.W;
	return *this;
}


FVec4 FVec4::operator+(float Scalar) const
{
	return FVec4(this->X + Scalar, this->Y + Scalar, this->Z + Scalar, this->W + Scalar);
}
FVec4& FVec4::operator+=(float Scalar)
{
	this->X += Scalar;
	this->Y += Scalar;
	this->Z += Scalar;
	this->W += Scalar;
	return *this;
}


FVec4 FVec4::operator-(const FVec4& Other) const
{
	return FVec4(this->X - Other.X, this->Y - Other.Y, this->Z - Other.Z, this->W - Other.W);
}
FVec4& FVec4::operator-=(const FVec4& Other)
{
	this->X -= Other.X;
	this->Y -= Other.Y;
	this->Z -= Other.Z;
	this->W -= Other.W;
	return *this;
}


FVec4 FVec4::operator-(float Scalar) const
{
	return FVec4(this->X - Scalar, this->Y - Scalar, this->Z - Scalar, this->W - Scalar);
}
FVec4& FVec4::operator-=(float Scalar)
{
	this->X -= Scalar;
	this->Y -= Scalar;
	this->Z -= Scalar;
	this->W -= Scalar;
	return *this;
}


FVec4 FVec4::operator*(const FVec4& Other) const
{
	return FVec4(this->X * Other.X, this->Y * Other.Y, this->Z * Other.Z, this->W * Other.W);
}
FVec4& FVec4::operator*=(const FVec4& Other)
{
	this->X *= Other.X;
	this->Y *= Other.Y;
	this->Z *= Other.Z;
	this->W *= Other.W;
	return *this;
}


FVec4 FVec4::operator*(float Scalar) const
{
	return FVec4(this->X * Scalar, this->Y * Scalar, this->Z * Scalar, this->W * Scalar);
}
FVec4& FVec4::operator*=(float Scalar)
{
	this->X *= Scalar;
	this->Y *= Scalar;
	this->Z *= Scalar;
	this->W *= Scalar;
	return *this;
}


FVec4 FVec4::operator/(const FVec4& Other) const
{
	return FVec4(
		Other.X != .0f? this->X / Other.X : .0f,
		Other.Y != .0f? this->Y / Other.Y : .0f,
		Other.Z != .0f? this->Z / Other.Z : .0f,
		Other.W != .0f? this->W / Other.W : .0f
	);
}
FVec4& FVec4::operator/=(const FVec4& Other)
{
	this->X = Other.X != .0f? this->X / Other.X : .0f;
	this->Y = Other.Y != .0f? this->Y / Other.Y : .0f;
	this->Z = Other.Z != .0f? this->Z / Other.Z : .0f;
	this->W = Other.W != .0f? this->W / Other.W : .0f;
	return *this;
}


FVec4 FVec4::operator/(float Scalar) const
{
	if (Scalar != .0f)
		return FVec4(this->X / Scalar, this->Y / Scalar, this->Z / Scalar, this->W / Scalar);
	else
		return FVec4(.0f);
}
FVec4& FVec4::operator/=(float Scalar)
{
	if (Scalar != .0f)
	{
		this->X /= Scalar;
		this->Y /= Scalar;
		this->Z /= Scalar;
		this->W /= Scalar;
	}
	else
	{
		this->X = this->Y = this->Z = this->W = .0f;
	}
	return *this;
}


FVec4 FVec4::operator-() const
{
	return FVec4(-this->X, -this->Y, -this->Z, this->W);
}


bool FVec4::NearlyEquals(const FVec4& Other, float Difference)
{
	return 
		FMath::Abs(this->X - Other.X) <= Difference && 
		FMath::Abs(this->Y - Other.Y) <= Difference &&
		FMath::Abs(this->Z - Other.Z) <= Difference &&
		FMath::Abs(this->W - Other.W) <= Difference;
}


FVec4 FVec4::Pow(float Exponent) const
{
	return FVec4(FMath::Pow(this->X, Exponent), FMath::Pow(this->Y, Exponent), FMath::Pow(this->Z, Exponent), FMath::Pow(this->W, Exponent));
}


FVec4 FVec4::Pow(const FVec4& Exponents) const
{
	return FVec4(FMath::Pow(this->X, Exponents.X), FMath::Pow(this->Y, Exponents.Y), FMath::Pow(this->Z, Exponents.Z), FMath::Pow(this->W, Exponents.W));
}


void FVec4::Normalize()
{
	float l = this->Length();
	if (l != .0f)
	{
		this->X /= l;
		this->Y /= l;
		this->Z /= l;
		this->W /= l;
	}
}


FVec4 FVec4::Normalized() const
{
	float l = this->Length();
	if (l != .0f)
	{
		return FVec4(
			this->X / l, 
			this->Y / l, 
			this->Z / l,
			this->W / l
		);
	}
	return FVec4(*this);
}


float FVec4::Length() const
{
	return FMath::Sqrt(X * X + Y * Y + Z * Z);
}


float FVec4::Dot(const FVec4& B) const
{
	return this->X * B.X + this->Y * B.Y + this->Z * B.Z;
}


FVec4 FVec4::Cross(const FVec4& B) const
{
	return FVec4(
		this->Y * B.Z - this->Z * B.Y,
		this->Z * B.X - this->X * B.Z,
		this->X * B.Y - this->Y * B.X,
		.0f
	);
}


float FVec4::Angle(const FVec4& B) const
{
	return FMath::Acos(this->Dot(B) / (this->Length() * B.Length()));
}


float FVec4::Distance(const FVec4& B) const
{
	return FMath::Sqrt(FMath::Pow(B.X - this->X, 2) + FMath::Pow(B.Y - this->Y, 2) + FMath::Pow(B.Z - this->Z, 2));
}


void FVec4::Invert()
{
	this->X *= -1;
	this->Y *= -1;
	this->Z *= -1;
}


FVec2 FVec4::XY() const
{
	return FVec2(this->X, this->Y);
}


FVec3 FVec4::XYZ() const
{
	return FVec3(this->X, this->Y, this->Z);
}


FVec4 FVec4::Lerp(const FVec4& A, const FVec4& B, const float Factor)
{
	return FVec4(
		A.X * (1.0f - Factor) + B.X * Factor,
		A.Y * (1.0f - Factor) + B.Y * Factor,
		A.Z * (1.0f - Factor) + B.Z * Factor,
		A.W * (1.0f - Factor) + B.W * Factor
	);
}