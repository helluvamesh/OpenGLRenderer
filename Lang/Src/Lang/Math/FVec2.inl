
FVec2::FVec2() : X(.0f), Y(.0f) { }

FVec2::FVec2(float InX, float InY) : X(InX), Y(InY) { }

FVec2::FVec2(float Value) : X(Value), Y(Value) { }


bool FVec2::operator==(const FVec2& Other) const
{
	return this->X == Other.X && this->Y == Other.Y;
}
bool FVec2::operator!=(const FVec2& Other) const
{
	return this->X != Other.X || this->Y != Other.Y;
}


FVec2 FVec2::operator+(const FVec2& Other) const
{
	return FVec2(this->X + Other.X, this->Y + Other.Y);
}
FVec2& FVec2::operator+=(const FVec2& Other)
{
	this->X += Other.X;
	this->Y += Other.Y;
	return *this;
}


FVec2 FVec2::operator+(float Scalar) const
{
	return FVec2(this->X + Scalar, this->Y + Scalar);
}
FVec2& FVec2::operator+=(float Scalar)
{
	this->X += Scalar;
	this->Y += Scalar;
	return *this;
}


FVec2 FVec2::operator-(const FVec2& Other) const
{
	return FVec2(this->X - Other.X, this->Y - Other.Y);
}
FVec2& FVec2::operator-=(const FVec2& Other)
{
	this->X -= Other.X;
	this->Y -= Other.Y;
	return *this;
}


FVec2 FVec2::operator-(float Scalar) const
{
	return FVec2(this->X - Scalar, this->Y - Scalar);
}
FVec2& FVec2::operator-=(float Scalar)
{
	this->X -= Scalar;
	this->Y -= Scalar;
	return *this;
}


FVec2 FVec2::operator*(const FVec2& Other) const
{
	return FVec2(this->X * Other.X, this->Y * Other.Y);
}
FVec2& FVec2::operator*=(const FVec2& Other)
{
	this->X *= Other.X;
	this->Y *= Other.Y;
	return *this;
}


FVec2 FVec2::operator*(float Scalar) const
{
	return FVec2(this->X * Scalar, this->Y * Scalar);
}
FVec2& FVec2::operator*=(float Scalar)
{
	this->X *= Scalar;
	this->Y *= Scalar;
	return *this;
}


FVec2 FVec2::operator/(FVec2 Other) const
{
	return FVec2(
		Other.X != .0f? this->X / Other.X : .0f,
		Other.Y != .0f? this->Y / Other.Y : .0f
	);
}
FVec2& FVec2::operator/=(FVec2 Other)
{
	this->X = Other.X != .0f? this->X / Other.X : .0f;
	this->Y = Other.Y != .0f? this->Y / Other.Y : .0f;
	return *this;
}


FVec2 FVec2::operator/(float Scalar) const
{
	if (Scalar != .0f)
		return FVec2(this->X / Scalar, this->Y / Scalar);
	else
		return FVec2(.0f);
}
FVec2& FVec2::operator/=(float Scalar)
{
	if (Scalar != .0f)
	{
		this->X /= Scalar;
		this->Y /= Scalar;
	}
	else
	{
		this->X = this->Y = .0f;
	}
	return *this;
}


FVec2 FVec2::operator-() const
{
	return FVec2(-this->X, -this->Y);
}


bool FVec2::NearlyEquals(const FVec2& Other, float Difference)
{
	return 
		FMath::Abs(this->X - Other.X) <= Difference && 
		FMath::Abs(this->Y - Other.Y) <= Difference;
}


FVec2 FVec2::Pow(float Exponent) const
{
	return FVec2(FMath::Pow(this->X, Exponent), FMath::Pow(this->Y, Exponent));
}


FVec2 FVec2::Pow(FVec2 Exponents) const
{
	return FVec2(FMath::Pow(this->X, Exponents.X), FMath::Pow(this->Y, Exponents.Y));
}


void FVec2::Normalize()
{
	float l = this->Length();
	if (l != .0f)
	{
		this->X /= l;
		this->Y /= l;
	}
}


FVec2 FVec2::Normalized() const
{
	float l = this->Length();
	if (l != .0f)
	{
		return FVec2(
			this->X / l,
			this->Y / l
		);
	}
	return FVec2(*this);
}


float FVec2::Length() const
{
	return FMath::Sqrt(X * X + Y * Y);
}


float FVec2::Dot(const FVec2& B) const
{
	return this->X * B.X + this->Y * B.Y;
}


float FVec2::Angle(const FVec2& B) const
{
	return FMath::Acos(this->Dot(B) / (this->Length() * B.Length()));
}


float FVec2::SignedAngle(const FVec2& B) const
{
	return FMath::Atan2(
		this->X * B.Y - this->Y * B.X, 
		this->X * B.X + this->Y * B.Y
	);
}


float FVec2::Distance(const FVec2& B) const
{
	return FMath::Sqrt(FMath::Pow(B.X - this->X, 2) + FMath::Pow(B.Y - this->Y, 2));
}


void FVec2::Invert()
{
	this->X *= -1;
	this->Y *= -1;
}


FVec2 FVec2::Lerp(const FVec2& A, const FVec2& B, const float Factor)
{
	return FVec2(
		A.X * (1.0f - Factor) + B.X * Factor,
		A.Y * (1.0f - Factor) + B.Y * Factor
	);
}