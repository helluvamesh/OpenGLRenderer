#include "Engine\pch.h"
#include "GLMath.h"


// OPEN GL
// - Right handed
// - [-1...1] clip space


FMat4 GLMath::PerspectiveMatrix(float FOV, float AspectRatio, float NearClip, float FarClip)
{
    float tanHalfFov = FMath::Tan(FOV / 2.0f);
    return FMat4 (
        1.0f / (AspectRatio * tanHalfFov), .0f, .0f, .0f,
        .0f, 1.0f / tanHalfFov, .0f, .0f,
        .0f, .0f, -(FarClip + NearClip) / (FarClip - NearClip), -(2.0f * FarClip * NearClip) / (FarClip - NearClip),
        .0f, .0f, -1.0f, 0.0f
    );
}


FMat4 GLMath::OrthographicMatrix(float Left, float Right, float Bottom, float Top, float NearClip, float FarClip)
{
    return FMat4 (
        2.0f / (Right - Left), .0f, .0f, -(Right + Left) / (Right - Left),
        .0f, 2.0f / (Top - Bottom), .0f, -(Top + Bottom) / (Top - Bottom),
        .0f, .0f, -2.0f / (FarClip - NearClip), -(FarClip + NearClip) / (FarClip - NearClip),
        .0f, .0f, .0f, 1.0f
    );
}


FMat4 GLMath::ViewMatrix(const FVec3& CameraForward, const FVec3& CameraUp, const FVec3& CameraPos)
{
    FVec3 toCameraDir = -CameraForward;
    if (toCameraDir == CameraUp) toCameraDir += FVec3(.0000001f);  // same direction results in faulty matrix
    FVec3 cameraRight = CameraUp.Cross(toCameraDir);
    cameraRight.Normalize();

    return FMat4 {

        cameraRight.X, cameraRight.Y, cameraRight.Z, .0f,
        CameraUp.X, CameraUp.Y, CameraUp.Z, .0f,
        toCameraDir.X, toCameraDir.Y, toCameraDir.Z, .0f,
        .0f, .0f, .0f, 1.0f

    } * FMat4 {

        1.0f, .0f, .0f, -CameraPos.X,
        .0f, 1.0f, .0f, -CameraPos.Y,
        .0f, .0f, 1.0f, -CameraPos.Z,
        .0f, .0f, .0f, 1.0f

    };
}


FMat4 GLMath::LookAtMatrix(const FVec3& Start, const FVec3& Target, const FVec3& Up)
{
    FVec3 dir = Target - Start;
    dir.Normalize();
    if (-dir == Up) dir += FVec3(.0000001f);  // opposite direction results in faulty matrix
    FVec3 right = dir.Cross(Up);
    right.Normalize();
    FVec3 viewUp = right.Cross(dir);

    return FMat4 {
        right.X, right.Y, right.Z, -right.Dot(Start),
        viewUp.X, viewUp.Y, viewUp.Z, -viewUp.Dot(Start),
        -dir.X, -dir.Y, -dir.Z, dir.Dot(Start),
        .0f, .0f, .0f, 1.0f
    };
}


FBox GLMath::GetFrustumInWorldSpace(const FMat4& ProjectionMatrix, const FMat4& ViewMatrix)
{
    FBox frustum;

    FMat4 invMat;
    if ((ProjectionMatrix * ViewMatrix).TryInvert(invMat))
    {
        FAxisBox axisBox (-1, -1, -1, 1, 1, 1);
        FBox box = FBox::FromAxisBox(axisBox);
        for (int32 i = 0; i < 8; ++i)
        {
            FVec4 point = FVec4(box.Points[i], 1.0f);
            point = point * invMat;
            point /= point.W;
            frustum.Points[i] = point.XYZ();
        }
    }

    return frustum;
}


// ---------------------- DFrustum ----------------------


void DFrustumPlanes::Populate(const FMat4& View, const FMat4& Projection)
{
    FMat4 matrix = Projection * View;
    auto& m = matrix.Data;

    this->Left = FPlane(
        m[3][0] + m[0][0],
        m[3][1] + m[0][1],
        m[3][2] + m[0][2],
        m[3][3] + m[0][3]
    );
    this->Left.Normalize();

    this->Right = FPlane(
        m[3][0] - m[0][0],
        m[3][1] - m[0][1],
        m[3][2] - m[0][2],
        m[3][3] - m[0][3]
    );
    this->Right.Normalize();

    this->Top = FPlane(
        m[3][0] - m[1][0],
        m[3][1] - m[1][1],
        m[3][2] - m[1][2],
        m[3][3] - m[1][3]
    );
    this->Top.Normalize();

    this->Bottom = FPlane(
        m[3][0] + m[1][0],
        m[3][1] + m[1][1],
        m[3][2] + m[1][2],
        m[3][3] + m[1][3]
    );
    this->Bottom.Normalize();

    this->Near = FPlane(
        m[3][0] + m[2][0],
        m[3][1] + m[2][1],
        m[3][2] + m[2][2],
        m[3][3] + m[2][3]
    );
    this->Near.Normalize();

    this->Far = FPlane(
        m[3][0] - m[2][0],
        m[3][1] - m[2][1],
        m[3][2] - m[2][2],
        m[3][3] - m[2][3]
    );
    this->Far.Normalize();
}


bool DFrustumPlanes::IsBoxOnPlaneInnerSide(const FPlane& Plane, const FBox& Box)
{
    auto& pts = Box.Points;
    return 
        (Plane.SignedDistance(pts[0]) >= .0f) ||
        (Plane.SignedDistance(pts[1]) >= .0f) ||
        (Plane.SignedDistance(pts[2]) >= .0f) ||
        (Plane.SignedDistance(pts[3]) >= .0f) ||
        (Plane.SignedDistance(pts[4]) >= .0f) ||
        (Plane.SignedDistance(pts[5]) >= .0f) ||
        (Plane.SignedDistance(pts[6]) >= .0f) ||
        (Plane.SignedDistance(pts[7]) >= .0f);
}


bool DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(const FPlane& Plane, const FAxisBox& AxisBox)
{
    const FVec3& min = AxisBox.Min;
    const FVec3& max = AxisBox.Max;
    return
        (Plane.SignedDistance(FVec3(min.X, min.Y, min.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(max.X, min.Y, min.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(max.X, max.Y, min.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(min.X, max.Y, min.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(max.X, min.Y, max.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(min.X, min.Y, max.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(min.X, max.Y, max.Z)) >= .0f) ||
        (Plane.SignedDistance(FVec3(max.X, max.Y, max.Z)) >= .0f);
}


bool DFrustumPlanes::Test(const FAxisBox& Box) const
{
    return
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Left, Box) &&
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Right, Box) &&
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Top, Box) &&
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Bottom, Box) &&
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Near, Box) &&
        DFrustumPlanes::IsAxisBoxOnPlaneInnerSide(this->Far, Box);
}


bool DFrustumPlanes::Test(const FBox& Box) const
{
    return
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Left, Box) &&
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Right, Box) &&
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Top, Box) &&
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Bottom, Box) &&
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Near, Box) &&
        DFrustumPlanes::IsBoxOnPlaneInnerSide(this->Far, Box);
}


bool DFrustumPlanes::Test(const FSphere& Sphere) const
{
    float a = FMath::Min(this->Left.SignedDistance(Sphere.Center), this->Right.SignedDistance(Sphere.Center));
    float b = FMath::Min(this->Top.SignedDistance(Sphere.Center), this->Bottom.SignedDistance(Sphere.Center));
    float c = FMath::Min(this->Near.SignedDistance(Sphere.Center), this->Far.SignedDistance(Sphere.Center));
    
    return (FMath::Min(FMath::Min(a, b), c) + Sphere.Radius) > .0f;
}
