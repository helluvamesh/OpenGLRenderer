
FPlane::FPlane() : 
	A(.0f), 
	B(.0f),
	C(.0f),
	D(.0f)
{
}


FPlane::FPlane(float InA, float InB, float InC, float InD) : 
	A(InA),
	B(InB), 
	C(InC), 
	D(InD)
{
}


FPlane FPlane::FromPointAndNormal(const FVec3& Point, const FVec3& Normal)
{
	return FPlane(Normal.X, Normal.Y, Normal.Z, -(Normal.X * Point.X + Normal.Y * Point.Y + Normal.Z * Point.Z));
}


float FPlane::SignedDistance(const FVec3& Point) const
{
	return this->A * Point.X + this->B * Point.Y + this->C * Point.Z + this->D;
}


void FPlane::Normalize()
{    
	float magnitude = FMath::Sqrt(A * A + B * B + C * C);    
	this->A /= magnitude;    
	this->B /= magnitude;    
	this->C /= magnitude;
	this->D /= magnitude;
}


FPlane FPlane::Normalized()
{    
	float magnitude = FMath::Sqrt(A * A + B * B + C * C);    
	return FPlane(
		this->A / magnitude, 
		this->B / magnitude,  
		this->C / magnitude,
		this->D / magnitude
	);
}