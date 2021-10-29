
FQuat::FQuat() : X(.0f), Y(.0f), Z(.0f), W(1.0f) { }


FQuat::FQuat(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) { }


FQuat FQuat::FromAxisAngle(const FVec3& Axis, float Angle)
{
    float halfAngle = Angle * .5f;
    float sinHalfAngle = FMath::Sin(halfAngle);

    return FQuat (
        sinHalfAngle * Axis.X,
        sinHalfAngle * Axis.Y,
        sinHalfAngle * Axis.Z,
        FMath::Cos(halfAngle)
    );
}


FQuat FQuat::FromEuler(const FVec3& Eul)
{
    float cy = FMath::Cos(Eul.Z * .5f);
    float sy = FMath::Sin(Eul.Z * .5f);
    float cp = FMath::Cos(Eul.Y * .5f);
    float sp = FMath::Sin(Eul.Y * .5f);
    float cr = FMath::Cos(Eul.X * .5f);
    float sr = FMath::Sin(Eul.X * .5f);

    return FQuat(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
}


FQuat FQuat::FromEuler(float EulerX, float EulerY, float EulerZ)
{
    float cy = FMath::Cos(EulerZ * .5f);
    float sy = FMath::Sin(EulerZ * .5f);
    float cp = FMath::Cos(EulerY * .5f);
    float sp = FMath::Sin(EulerY * .5f);
    float cr = FMath::Cos(EulerX * .5f);
    float sr = FMath::Sin(EulerX * .5f);

    return FQuat(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
}


FQuat FQuat::operator*(const FQuat& Other) const
{
    FQuat q(
        this->X * Other.W + this->Y * Other.Z - this->Z * Other.Y + this->W * Other.X,
        - this->X * Other.Z + this->Y * Other.W + this->Z * Other.X + this->W * Other.Y,
        this->X * Other.Y - this->Y * Other.X + this->Z * Other.W + this->W * Other.Z,
        - this->X * Other.X - this->Y * Other.Y - this->Z * Other.Z + this->W * Other.W
    );
    q.Normalize();
    return q;
}


FQuat& FQuat::operator*=(const FQuat& Other)
{
    float x =   this->X * Other.W + this->Y * Other.Z - this->Z * Other.Y + this->W * Other.X;
    float y = - this->X * Other.Z + this->Y * Other.W + this->Z * Other.X + this->W * Other.Y;
    float z =   this->X * Other.Y - this->Y * Other.X + this->Z * Other.W + this->W * Other.Z;
    float w = - this->X * Other.X - this->Y * Other.Y - this->Z * Other.Z + this->W * Other.W;

    this->X = x;
    this->Y = y;
    this->Z = z;
    this->W = w;

    this->Normalize();

    return *this;
}


FQuat FQuat::operator-(const FQuat& Other) const
{
    FQuat q = (*this) * FQuat(-Other.X, -Other.Y, -Other.Z, Other.W);
    q.Normalize();
    return q;
}


FQuat& FQuat::operator-=(const FQuat& Other)
{
    (*this) *= FQuat(-Other.X, -Other.Y, -Other.Z, Other.W);
    this->Normalize();
    return *this;
}


bool FQuat::operator==(const FQuat& Other) const
{
    return this->X == Other.X && this->Y == Other.Y && this->Z * Other.Z && this->W == Other.W;
}


bool FQuat::operator!=(const FQuat& Other) const
{
    return this->X != Other.X || this->Y != Other.Y || this->Z != Other.Z || this->W != Other.W;
}


bool FQuat::NearlyEquals(const FQuat& Other, float Difference)
{
    return 
        FMath::Abs(this->X - Other.X) <= Difference && 
        FMath::Abs(this->Y - Other.Y) <= Difference && 
        FMath::Abs(this->Z - Other.Z) <= Difference && 
        FMath::Abs(this->W - Other.W) <= Difference ||
        FMath::Abs(this->X + Other.X) <= Difference && 
        FMath::Abs(this->Y + Other.Y) <= Difference && 
        FMath::Abs(this->Z + Other.Z) <= Difference && 
        FMath::Abs(this->W + Other.W) <= Difference;
}


FQuat FQuat::Inverse() const
{
    return FQuat(
        - this->X,
        - this->Y,
        - this->Z,
        this->W
    );
}


void FQuat::Invert()
{
    this->X *= -1;
    this->Y *= -1;
    this->Z *= -1;
}


/* devide components by Quaternion length/norm */
void FQuat::Normalize()
{
    float v = FMath::Sqrt(X * X + Y * Y + Z * Z + W * W);
    if (v != .0f)
    {
        this->X /= v;
        this->Y /= v;
        this->Z /= v;
        this->W /= v;
    }
    else
    {
        this->X = this->Y = this->Z = .0f;
        this->W = 1.0f;
    }
}


/* devide components by quaternion length/norm */
FQuat FQuat::Normalized() const
{
    float v = FMath::Sqrt(X * X + Y * Y + Z * Z + W * W);
    if (v != .0f)
    {
        return FQuat(
            this->X / v,
            this->Y / v,
            this->Z / v,
            this->W / v
        );
    }
    else
    {
        return FQuat();
    }
}


FQuat FQuat::SLerp(const FQuat& A, const FQuat& B, float Factor)
{
    const float epsilon = 1e-6f;

    float cosOmega = A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;

    bool flip = false;

    if (cosOmega < 0.0f)
    {
        flip = true;
        cosOmega = -cosOmega;
    }

    float s1, s2;

    if (cosOmega > (1.0f - epsilon))
    {
        // Too close, do straight linear interpolation.
        s1 = 1.0f - Factor;
        s2 = (flip) ? -Factor : Factor;
    }
    else
    {
        float omega = FMath::Acos(cosOmega);
        float invSinOmega = 1 / FMath::Sin(omega);

        s1 = FMath::Sin((1.0f - Factor) * omega) * invSinOmega;
        s2 = (flip)? 
            -FMath::Sin(Factor * omega) * invSinOmega : 
            FMath::Sin(Factor * omega) * invSinOmega;
    }

    FQuat q (
        s1 * A.X + s2 * B.X,
        s1 * A.Y + s2 * B.Y,
        s1 * A.Z + s2 * B.Z,
        s1 * A.W + s2 * B.W
    );
    q.Normalize();
    return q;
}


FVec3 FQuat::ToEuler() const
{
    float y = 2.0f * (this->W * this->Y - this->Z * this->X);

    return FVec3(
        FMath::Atan2(
            2.0f * (this->W * this->X + this->Y * this->Z), 
            1.0f - 2.0f * (this->X * this->X + this->Y * this->Y)
        ),
        FMath::Abs(y) >= 1.0f?
        FMath::Copysign(FMath::PI * .5f, y) :
        FMath::Asin(y)
        ,
        FMath::Atan2(
            2.0f * (this->W * this->Z + this->X * this->Y), 
            1.0f - 2.0f * (this->Y * this->Y + this->Z * this->Z)
        )
    );
}


void FQuat::ToAxisAngle(FVec3& OutAxis, float& OutAngle) const
{
    if (this->W > 1)  // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
    {
        FQuat q = this->Normalized();

        OutAngle = 2 * FMath::Acos(q.W);
        float s = FMath::Sqrt(1 - q.W * q.W);  // assuming quaternion normalised then w is less than 1, so term always positive.
        if (s < 0.001f)  // test to avoid divide by zero, s is always positive due to sqrt
        {
            // if s close to zero then direction of axis not important
            // if it is important that axis is normalised then replace with x=1; y=z=0;
            OutAxis = FVec3(q.X, q.Y, q.Z);
        }
        else
        {
            // normalise axis
            OutAxis = FVec3(q.X / s, q.Y / s, q.Z / s);
        }
    }
    else
    {
        OutAngle = 2 * FMath::Acos(this->W);
        float s = FMath::Sqrt(1 - this->W * this->W);
        if (s < 0.001f)
        {
            OutAxis = FVec3(this->X, this->Y, this->Z);
        }
        else
        {
            OutAxis = FVec3(this->X / s, this->Y / s, this->Z / s);
        }
    }
}


FVec3 operator*(const FVec3& V, const FQuat& Q)
{	
    return FVec3(
        V.X *(Q.X*Q.X+Q.W*Q.W - Q.Y*Q.Y - Q.Z*Q.Z) + V.Y*(2.f*Q.X*Q.Y - 2.f*Q.W*Q.Z) + V.Z*(2.f*Q.X*Q.Z + 2.f*Q.W*Q.Y),
        V.X*(2.f*Q.W*Q.Z + 2.f*Q.X*Q.Y) + V.Y*(Q.W*Q.W - Q.X*Q.X+ Q.Y*Q.Y - Q.Z*Q.Z)+ V.Z*(-2.f*Q.W*Q.X + 2.f*Q.Y*Q.Z),
        V.X*(-2.f*Q.W*Q.Y + 2.f*Q.X*Q.Z) + V.Y*(2.f*Q.W*Q.X + 2.f*Q.Y*Q.Z)+ V.Z*(Q.W*Q.W - Q.X*Q.X - Q.Y*Q.Y + Q.Z*Q.Z)
    );
}
