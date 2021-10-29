
FVec3::FVec3() : X(.0f), Y(.0f), Z(.0f) { }

FVec3::FVec3(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) { }

FVec3::FVec3(float Value) : X(Value), Y(Value), Z(Value) { }

FVec3::FVec3(FVec2 Vec2, float InZ): X(Vec2.X), Y(Vec2.Y), Z(InZ) { }


bool FVec3::operator==(const FVec3& Other) const
{
	return this->X == Other.X && this->Y == Other.Y && this->Z == Other.Z;
}
bool FVec3::operator!=(const FVec3& Other) const
{
	return this->X != Other.X || this->Y != Other.Y || this->Z != Other.Z;
}


FVec3 FVec3::operator+(const FVec3& Other) const
{
	return FVec3(this->X + Other.X, this->Y + Other.Y, this->Z + Other.Z);
}
FVec3& FVec3::operator+=(const FVec3& Other)
{
	this->X += Other.X; 
	this->Y += Other.Y; 
	this->Z += Other.Z;
	return *this;
}


FVec3 FVec3::operator+(float Scalar) const
{
	return FVec3(this->X + Scalar, this->Y + Scalar, this->Z + Scalar);
}
FVec3& FVec3::operator+=(float Scalar)
{
	this->X += Scalar;
	this->Y += Scalar;
	this->Z += Scalar;
	return *this;
}


FVec3 FVec3::operator-(const FVec3& Other) const
{
	return FVec3(this->X - Other.X, this->Y - Other.Y, this->Z - Other.Z);
}
FVec3& FVec3::operator-=(const FVec3& Other)
{
	this->X -= Other.X;
	this->Y -= Other.Y;
	this->Z -= Other.Z;
	return *this;
}


FVec3 FVec3::operator-(float Scalar) const
{
	return FVec3(this->X - Scalar, this->Y - Scalar, this->Z - Scalar);
}
FVec3& FVec3::operator-=(float Scalar)
{
	this->X -= Scalar;
	this->Y -= Scalar;
	this->Z -= Scalar;
	return *this;
}


FVec3 FVec3::operator*(const FVec3& Other) const
{
	return FVec3(this->X * Other.X, this->Y * Other.Y, this->Z * Other.Z);
}
FVec3& FVec3::operator*=(const FVec3& Other)
{
	this->X *= Other.X;
	this->Y *= Other.Y;
	this->Z *= Other.Z;
	return *this;
}


FVec3 FVec3::operator*(float Scalar) const
{
	return FVec3(this->X * Scalar, this->Y * Scalar, this->Z * Scalar);
}
FVec3& FVec3::operator*=(float Scalar)
{
	this->X *= Scalar;
	this->Y *= Scalar;
	this->Z *= Scalar;
	return *this;
}


FVec3 FVec3::operator/(const FVec3& Other) const
{
	return FVec3(
		Other.X != .0f? this->X / Other.X : .0f,
		Other.Y != .0f? this->Y / Other.Y : .0f,
		Other.Z != .0f? this->Z / Other.Z : .0f
	);
}
FVec3& FVec3::operator/=(const FVec3& Other)
{
	this->X = Other.X != .0f? this->X / Other.X : .0f;
	this->Y = Other.Y != .0f? this->Y / Other.Y : .0f;
	this->Z = Other.Z != .0f? this->Z / Other.Z : .0f;
	return *this;
}


FVec3 FVec3::operator/(float Scalar) const
{
	if (Scalar != .0f)
		return FVec3(this->X / Scalar, this->Y / Scalar, this->Z / Scalar);
	else
		return FVec3(.0f);
}
FVec3& FVec3::operator/=(float Scalar)
{
	if (Scalar != .0f)
	{
		this->X /= Scalar;
		this->Y /= Scalar;
		this->Z /= Scalar;
	}
	else
	{
		this->X = this->Y = this->Z = .0f;
	}
	return *this;
}


FVec3 FVec3::operator-() const
{
	return FVec3(-this->X, -this->Y, -this->Z);
}


bool FVec3::NearlyEquals(const FVec3& Other, float Difference)
{
	return 
		FMath::Abs(this->X - Other.X) <= Difference && 
		FMath::Abs(this->Y - Other.Y) <= Difference &&
		FMath::Abs(this->Z - Other.Z) <= Difference;
}


FVec3 FVec3::Pow(float Exponent) const
{
	return FVec3(FMath::Pow(this->X, Exponent), FMath::Pow(this->Y, Exponent), FMath::Pow(this->Z, Exponent));
}


FVec3 FVec3::Pow(const FVec3& Exponents) const
{
	return FVec3(FMath::Pow(this->X, Exponents.X), FMath::Pow(this->Y, Exponents.Y), FMath::Pow(this->Z, Exponents.Z));
}


void FVec3::Normalize()
{
	float l = this->Length();
	if (l != .0f)
	{
		this->X /= l;
		this->Y /= l;
		this->Z /= l;
	}
}


FVec3 FVec3::Normalized() const
{
	float l = this->Length();
	if (l != .0f)
	{
		return FVec3(
			this->X / l, 
			this->Y / l, 
			this->Z / l
		);
	}
	return FVec3(*this);
}


float FVec3::Length() const
{
	return FMath::Sqrt(X * X + Y * Y + Z * Z);
}


float FVec3::Dot(const FVec3& B) const
{
	return this->X * B.X + this->Y * B.Y + this->Z * B.Z;
}


FVec3 FVec3::Cross(const FVec3& B) const
{
	return FVec3(
		this->Y * B.Z - this->Z * B.Y,
		this->Z * B.X - this->X * B.Z,
		this->X * B.Y - this->Y * B.X
	);
}


float FVec3::Angle(const FVec3& B) const
{
	return FMath::Acos(this->Dot(B) / (this->Length() * B.Length()));
}


float FVec3::Distance(const FVec3& B) const
{
	return FMath::Sqrt(FMath::Pow(B.X - this->X, 2) + FMath::Pow(B.Y - this->Y, 2) + FMath::Pow(B.Z - this->Z, 2));
}


void FVec3::Invert()
{
	this->X *= -1;
	this->Y *= -1;
	this->Z *= -1;
}


FVec2 FVec3::XY() const
{
	return FVec2(this->X, this->Y);
}


FVec3 FVec3::Lerp(const FVec3& A, const FVec3& B, const float Factor)
{
	return FVec3(
		A.X * (1.0f - Factor) + B.X * Factor,
		A.Y * (1.0f - Factor) + B.Y * Factor,
		A.Z * (1.0f - Factor) + B.Z * Factor
	);
}