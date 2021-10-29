#include "Engine\pch.h"
#include "HIBLConvert.h"
#include "Engine\Render\RenderUtil\GLMath.h"
#include "Engine\Util\SEngine.h"


void HIBLConvert::Load()
{
    this->Loaded = true;

    this->UnitCubeMesh = MakeShared<HStaticMesh>();
    this->UnitCubeMesh->SetFilepath(SEngine::ContentPath + TX("Meshes/UnitCube.asset"));
    this->UnitCubeMesh->Load();

    this->UnitCube = MakeShared<HStaticMeshNode>();
    this->UnitCube->SetMesh(this->UnitCubeMesh);

    // ------------ IRRADIANCE ----------------

    DSurfaceShaderSettings irrShaderSettings;
    irrShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/Environment.vert"),
        SEngine::ContentPath + TX("Shaders/EnvironmentIrradiance.frag")
    };
    irrShaderSettings.Unlit = true;

    this->IrrShader = MakeShared<HSurfaceShader>();
    this->IrrShader->SetName(TX("IrrShader"));
    this->IrrShader->Load(irrShaderSettings);

    DSurfaceMaterialSettings irrMaterialSettings;
    irrMaterialSettings.Shader = this->IrrShader;
    irrMaterialSettings.DoubleSided = true;

    this->IrrMaterial = MakeShared<HSurfaceMaterial>();
    this->IrrMaterial->Load(irrMaterialSettings);

    this->UnitCube->SetMaterialOverride(0, this->IrrMaterial);

    this->IrrTarget = MakeShared<HFrameBuffer>();

    // ------------ REFLECTION ----------------

    DSurfaceShaderSettings refShaderSettings;
    refShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/Environment.vert"),
        SEngine::ContentPath + TX("Shaders/EnvironmentReflection.frag")
    };
    refShaderSettings.Unlit = true;

    this->RefShader = MakeShared<HSurfaceShader>();
    this->RefShader->SetName(TX("RefShader"));
    this->RefShader->Load(refShaderSettings);

    DSurfaceMaterialSettings refMaterialSettings;
    refMaterialSettings.Shader = this->RefShader;
    refMaterialSettings.DoubleSided = true;

    this->RefMaterial = MakeShared<HSurfaceMaterial>();
    this->RefMaterial->Load(refMaterialSettings);

    this->RefTarget = MakeShared<HFrameBuffer>();
}


void HIBLConvert::GenerateLightmaps(
    const DArray<shared<HRenderTargetCube>>& Cubemaps,
    int32 IrradianceDim,
    int32 ReflectionDim,
    DArray<shared<HRenderTargetCube>>& OutIrradianceMaps,
    DArray<shared<HRenderTargetCube>>& OutReflectionMaps,
    bool VFlip
)
{
    if (this->Loaded == false)
    {
        SLog::Error("IBLConvert is not loaded");
        return;
    }

    this->IrrMaterial->SetIntParam(TX("VFlip"), VFlip == false);
    this->RefMaterial->SetIntParam(TX("VFlip"), VFlip == false);

    DCameraRenderData cam;
    cam.ProjectionMatrix = GLMath::PerspectiveMatrix(FMath::DegToRad(90), 1, .1f, 10);

    // ------------ IRRADIANCE ----------------

    OutIrradianceMaps.Reserve(Cubemaps.Length());

    this->IrrTarget->Prepare(IrradianceDim, IrradianceDim, 1, 1);

    this->IrrShader->Use();

    for (int32 i = 0; i < Cubemaps.Length(); i++)
    {
        DRenderTargetCubeSettings irrTextureSettings;
        irrTextureSettings.Format = ERenderTargetCubeFormat::RGB16F;
        irrTextureSettings.Width = IrradianceDim;
        irrTextureSettings.Height = IrradianceDim;
        irrTextureSettings.GenerateMipMaps = false;
        irrTextureSettings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
        shared<HRenderTargetCube> irrTexture = MakeShared<HRenderTargetCube>();
        irrTexture->Load(irrTextureSettings);

        this->IrrTarget->SetColorAttachment(0, irrTexture);

        this->IrrMaterial->SetTextureParam(TX("EnvMap"), Cubemaps[i]);
        this->IrrMaterial->ApplyStateOnShader();

        // left
        this->IrrTarget->AttachCubemapSide(0, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[0];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();

        // right
        this->IrrTarget->AttachCubemapSide(1, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[1];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // top
        this->IrrTarget->AttachCubemapSide(2, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[2];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // bottom
        this->IrrTarget->AttachCubemapSide(3, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[3];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // back
        this->IrrTarget->AttachCubemapSide(4, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[4];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // front
        this->IrrTarget->AttachCubemapSide(5, 0);
        this->IrrTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[5];
        this->IrrMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();

        OutIrradianceMaps.Add(irrTexture);
    }

    this->IrrTarget->BindNone();

    // ------------ REFLECTION ----------------

    this->UnitCube->SetMaterialOverride(0, this->RefMaterial);

    this->RefShader->Use();

    OutReflectionMaps.Reserve(Cubemaps.Length());

    for (int32 i = 0; i < Cubemaps.Length(); i++)
    {
        DRenderTargetCubeSettings refTextureSettings;
        refTextureSettings.Format = ERenderTargetCubeFormat::RGB16F;
        refTextureSettings.Width = ReflectionDim;
        refTextureSettings.Height = ReflectionDim;
        refTextureSettings.FixedMipCount = 4;
        refTextureSettings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
        shared<HRenderTargetCube> refTexture = MakeShared<HRenderTargetCube>();
        refTexture->Load(refTextureSettings);

        this->RefTarget->SetColorAttachment(0, refTexture);

        this->RefMaterial->SetTextureParam(TX("EnvMap"), Cubemaps[i]);

        int32 mipCount = 5;
        for (int32 mip_i = 0; mip_i < mipCount; mip_i++)
        {
            int32 divider = (int32)FMath::Pow(2.0, (float)mip_i);
            int32 width = refTexture->GetWidth() / divider;
            int32 height = refTexture->GetHeight() / divider;
            this->RefTarget->Prepare(width, height, 1, 1);

            this->RefMaterial->SetFloatParam(TX("Roughness"), (float)mip_i / (float)(mipCount - 1));
            this->RefMaterial->ApplyStateOnShader();

            for (int32 side_i = 0; side_i < 6; side_i++)
            {
                this->RefTarget->AttachCubemapSide(side_i, mip_i);
                this->RefTarget->Bind(true);
                cam.ViewMatrix = HRenderTargetCube::CubeSideViews[side_i];
                this->RefMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
                this->UnitCubeMesh->DrawStandalone();
            }
        }

        OutReflectionMaps.Add(refTexture);
    }

    this->RefTarget->BindNone();
}