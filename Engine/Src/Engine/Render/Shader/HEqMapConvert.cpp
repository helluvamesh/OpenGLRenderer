#include "Engine\pch.h"
#include "HEqMapConvert.h"
#include "Engine\Util\SEngine.h"
#include "Engine\Render\RenderUtil\GLMath.h"


void HEqMapConvert::Load()
{
    this->Loaded = true;

    DSurfaceShaderSettings eqShaderSettings;
    eqShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/Environment.vert"),
        SEngine::ContentPath + TX("Shaders/EnvironmentEq.frag")
    };
    eqShaderSettings.Unlit = true;

    this->EqShader = MakeShared<HSurfaceShader>();
    this->EqShader->SetName(TX("EqShader"));
    this->EqShader->Load(eqShaderSettings);

    DSurfaceMaterialSettings eqMaterialSettings;
    eqMaterialSettings.Shader = this->EqShader;
    eqMaterialSettings.DoubleSided = true;

    this->EqMaterial = MakeShared<HSurfaceMaterial>();
    this->EqMaterial->Load(eqMaterialSettings);

    this->UnitCubeMesh = MakeShared<HStaticMesh>();
    UnitCubeMesh->SetFilepath(SEngine::ContentPath + TX("Meshes/UnitCube.asset"));
    UnitCubeMesh->Load();

    this->UnitCube = MakeShared<HStaticMeshNode>();
    this->UnitCube->SetMesh(this->UnitCubeMesh);

    this->UnitCube->SetMaterialOverride(0, this->EqMaterial);

    this->CubeTarget = MakeShared<HFrameBuffer>();
}


void HEqMapConvert::EquirectangularToCubemap(
    const DArray<shared<HTexture2D>>& EqMaps,
    int32 Dimension,
    ERenderTargetCubeFormat Format,
    DArray<shared<HRenderTargetCube>>& OutCubemaps,
    float UOffset,
    bool VFlip
)
{
    if (this->Loaded == false) 
    {
        SLog::Error("EqMapConvert is not loaded");
        return;
    }

    this->EqMaterial->SetIntParam(TX("VFlip"), VFlip);

    DCameraRenderData cam;
    cam.ProjectionMatrix = GLMath::PerspectiveMatrix(FMath::DegToRad(90), 1, .5f, 10);

    this->CubeTarget->Prepare(Dimension, Dimension, 1, 1);

    OutCubemaps.Reserve(EqMaps.Length());

    this->EqShader->Use();

    for (int32 i = 0; i < EqMaps.Length(); i++)
    {
        DRenderTargetCubeSettings cubemapSettings;
        cubemapSettings.Format = Format;
        cubemapSettings.Width = Dimension;
        cubemapSettings.Height = Dimension;
        cubemapSettings.GenerateMipMaps = false;
        cubemapSettings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
        shared<HRenderTargetCube> cubemap = MakeShared<HRenderTargetCube>();
        cubemap->Load(cubemapSettings);

        this->CubeTarget->SetColorAttachment(0, cubemap);
        this->EqMaterial->SetTextureParam(TX("EnvMap"), EqMaps[i]);
        this->EqMaterial->SetFloatParam(TX("UOffset"), UOffset);

        this->EqMaterial->ApplyStateOnShader();
        
        // left
        this->CubeTarget->AttachCubemapSide(0, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[0];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // right
        this->CubeTarget->AttachCubemapSide(1, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[1];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // top
        this->CubeTarget->AttachCubemapSide(2, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[2];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // bottom
        this->CubeTarget->AttachCubemapSide(3, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[3];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // back
        this->CubeTarget->AttachCubemapSide(4, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[4];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();
        
        // front
        this->CubeTarget->AttachCubemapSide(5, 0);
        this->CubeTarget->Bind(true);
        cam.ViewMatrix = HRenderTargetCube::CubeSideViews[5];
        this->EqMaterial->ApplyNodeStateOnShader(cam, this->UnitCube->GetRenderData());
        this->UnitCubeMesh->DrawStandalone();

        OutCubemaps.Add(cubemap);
    }

    this->CubeTarget->BindNone();
}
