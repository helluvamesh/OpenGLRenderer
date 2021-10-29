#pragma once

#include "HSurfaceShader.h"
#include "HSurfaceMaterial.h"
#include "..\Scene\HStaticMeshNode.h"
#include "..\Texture\HFrameBuffer.h"
#include "..\Texture\HRenderTargetCube.h"


class HIBLConvert : public HObject
{
private:
    bool Loaded = false;
    shared<HStaticMesh> UnitCubeMesh;
    shared<HStaticMeshNode> UnitCube;
    shared<HSurfaceShader> IrrShader;
    shared<HSurfaceMaterial> IrrMaterial;
    shared<HFrameBuffer> IrrTarget;
    shared<HSurfaceShader> RefShader;
    shared<HSurfaceMaterial> RefMaterial;
    shared<HFrameBuffer> RefTarget;

public:
    void Load();
    void GenerateLightmaps(
        const DArray<shared<HRenderTargetCube>>& Cubemaps,
        int32 IrradianceDim,
        int32 ReflectionDim,
        DArray<shared<HRenderTargetCube>>& OutIrradianceMaps,
        DArray<shared<HRenderTargetCube>>& OutReflectionMaps,
        bool VFlip = false
    );

};