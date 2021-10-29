#pragma once


class GLMath
{
public:

    // -https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl

    /* AspectRatio = Width / Height */
    static FMat4 PerspectiveMatrix(float FOV, float AspectRatio, float NearClip, float FarClip);
	static FMat4 OrthographicMatrix(float Left, float Right, float Bottom, float Top, float NearClip, float FarClip);
    static FMat4 ViewMatrix(const FVec3& CameraForward, const FVec3& CameraUp, const FVec3& CameraPos);
    
    // -https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
    
    static FMat4 LookAtMatrix(const FVec3& Start, const FVec3& Target, const FVec3& Up);

    static FBox GetFrustumInWorldSpace(const FMat4& ProjectionMatrix, const FMat4& ViewMatrix);
};


struct DFrustumPlanes
{
public:
    FPlane Left;
    FPlane Right;
    FPlane Top;
    FPlane Bottom;
    FPlane Near;
    FPlane Far;

    // -https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    
    void Populate(const FMat4& View, const FMat4& Projection);
    
private:
    // on the border is inside
    FORCEINLINE static bool IsBoxOnPlaneInnerSide(const FPlane& Plane, const FBox& Box);
    // on the border is inside
    FORCEINLINE static bool IsAxisBoxOnPlaneInnerSide(const FPlane& Plane, const FAxisBox& AxisBox);
    
public:
    bool Test(const FAxisBox& Box) const;
    bool Test(const FBox& Box) const;
    bool Test(const FSphere& Sphere) const;
};
