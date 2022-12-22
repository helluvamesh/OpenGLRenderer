#include "Engine/pch.h"
#include "HModelViewer.h"
#include "Engine/Render/Shader/HEqMapConvert.h"
#include "Engine/Render/Shader/HIBLConvert.h"
#include "Engine/Render/Texture/HTexture2D.h"
#include "Engine/Render/Texture/HTextureCube.h"
#include "Engine/Util/SEngine.h"
#include "Engine\Asset\GLTF\SGltfImporter.h"
#include "Engine\Asset\Wavefront\DWavefrontParser.h"
#include "Engine\Render\Scene\HLineWorkNode.h"
#include "Engine\Render\Mesh\DLineWorkData.h"


HModelViewer::HModelViewer(shared<HRenderer> InRenderer) : 

    MeshExtensions({ TX("asset"), TX("obj"), TX("glb"), TX("gltf") }),
    TextureExtensions({ TX("asset"), TX("png"), TX("tga"), TX("jpg"), TX("psd") }),
    HDRExtensions({ TX("hdr") })

{
    SLog::Message(TX("Starting Model Viewer"));

    this->Renderer = InRenderer;
    this->EqMapConverter = MakeShared<HEqMapConvert>();
    this->EqMapConverter->Load();
    this->IBLConverter = MakeShared<HIBLConvert>();
    this->IBLConverter->Load();

    // ------------------------- SCENE ---------------------------
    
    this->CurrentScene = MakeShared<HScene>();
    this->CurrentScene->Init();

    this->CameraRoot = MakeShared<HSceneNode>();
    this->CurrentScene->GetRoot()->AddChild(this->CameraRoot);

    DCameraSettings camSettings;
    shared<HCameraNode> cam = MakeShared<HCameraNode>(camSettings);
    cam->SetFarClip(10000.0f);
    this->CurrentScene->SetActiveCamera(cam);
    this->CameraRoot->AddChild(cam);
    cam->SetLocation(FVec3(.0f, 1.0f, 5.0f));

    this->ModelRoot = MakeShared<HSceneNode>();
    this->ModelRoot->SetName(TX("ModelRoot"));
    this->CurrentScene->GetRoot()->AddChild(this->ModelRoot);
    
    DShadowDepthShaderSettings generalShadowDepthShaderSettings;
    generalShadowDepthShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/GeneralShadowDepth.vert"),
        SEngine::ContentPath + TX("Shaders/GeneralShadowDepth.geom"),
        SEngine::ContentPath + TX("Shaders/GeneralShadowDepth.frag")
    };
    shared<HShadowDepthShader> generalShadowDepthShader = MakeShared<HShadowDepthShader>();
    generalShadowDepthShader->SetName(TX("GeneralShadowDepthShader"));
    generalShadowDepthShader->Load(generalShadowDepthShaderSettings);

    DSurfaceShaderSettings generalShaderSettings;
    generalShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/General.vert"),
        SEngine::ContentPath + TX("Shaders/General.frag")
    };
    generalShaderSettings.ShadowDepthShader = generalShadowDepthShader;
    this->GeneralShader = MakeShared<HSurfaceShader>();
    this->GeneralShader->SetName(TX("GeneralShader"));
    this->GeneralShader->Load(generalShaderSettings);

    DSurfaceMaterialSettings defaultMaterialSettings;
    defaultMaterialSettings.Shader = this->GeneralShader;

    this->LightRoot = MakeShared<HSceneNode>();
    this->CurrentScene->GetRoot()->AddChild(this->LightRoot);

    DDirectionalLightSettings sunSettings;
    this->Sun = MakeShared<HDirectionalLightNode>(sunSettings);
    this->Sun->SetLocation(FVec3(30, 30, 30));
    this->Sun->SetRotation(FQuat::FromEuler(FMath::DegToRad(-45), FMath::DegToRad(45), 0));
    this->Sun->SetLightIntensity(3);
    this->Sun->SetLightColor(FVec4(1, .9f, .8f, 1));
    this->Sun->SetName(TX("Sun"));
    this->Sun->SetEnableShadows(true);
    this->LightRoot->AddChild(this->Sun);

    DAmbientLightSettings ambientLightSettings;
    this->AmbientLight = MakeShared<HAmbientLightNode>(ambientLightSettings);
    this->AmbientLight->SetName(TX("AmbientLight"));
    this->CurrentScene->AddLightToLightGroup(0, this->Sun);
    this->CurrentScene->AddLightToLightGroup(0, this->AmbientLight);
    this->LightRoot->AddChild(AmbientLight);

    this->Renderer->SetNextScene(this->CurrentScene);


    // selection bracket
    this->GizmoRoot = MakeShared<HSceneNode>();
    this->CurrentScene->GetRoot()->AddChild(this->GizmoRoot);

    DSurfaceShaderSettings lineWorkShaderSettings;
    lineWorkShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/LineWork.vert"),
        SEngine::ContentPath + TX("Shaders/LineWork.frag")
    };
    lineWorkShaderSettings.Unlit = true;
    this->LineWorkShader = MakeShared<HSurfaceShader>();
    this->LineWorkShader->SetName(TX("LineWorkShader"));
    this->LineWorkShader->Load(lineWorkShaderSettings);

    DSurfaceMaterialSettings lineWorkMaterialSettings;
    lineWorkMaterialSettings.Shader = this->LineWorkShader;
    shared<HSurfaceMaterial> lineWorkMaterial = MakeShared<HSurfaceMaterial>();
    lineWorkMaterial->SetName(TX("LineWorkMaterial"));
    lineWorkMaterial->Load(lineWorkMaterialSettings);

    DLineStrokeData lineStrokeData;
    FAxisBox ab (-1,-1,-1, 1,1,1);
    DLineStrokeData::BracketsFromBounds(ab, /*out*/lineStrokeData);
    lineStrokeData.LineWidth = 1.5f;
    
    DLineWorkData lineWorkData;
    lineWorkData.Stroke = lineStrokeData;
    lineWorkData.UpdateBounds();

    shared<HLineWork> lineWork = MakeShared<HLineWork>();
    lineWork->Load(lineWorkData);
    lineWork->SetMaterial(lineWorkMaterial);
    lineWorkMaterial->SetVec4Param(TX("Color"), FVec4(1, .5, 0, 1));
    
    this->SelectionBracket = MakeShared<HLineWorkNode>();
    this->SelectionBracket->SetLineWork(lineWork);
    this->SelectionBracket->SetVisible(false);
    this->GizmoRoot->AddChild(this->SelectionBracket);
}


shared<HScene> HModelViewer::GetCurrentScene()
{
    return this->CurrentScene;
}


shared<HStaticMeshNode> HModelViewer::GetMeshNode()
{
    return this->CurrentMeshNode;
}


shared<HSurfaceMaterial> HModelViewer::GetMaterial(int32 MaterialIndex)
{
    shared<HStaticMeshNode> meshNode = this->GetMeshNode();
    if (meshNode == nullptr || meshNode->GetMesh() == nullptr) return nullptr;

    const DArray<DStaticMeshMaterialSlot>& slots = meshNode->GetMesh()->GetMaterialSlots();
    if (slots.IsValidIndex(MaterialIndex) == false) return nullptr;

    return slots[MaterialIndex].Material;
}


shared<HDirectionalLightNode> HModelViewer::GetSun()
{
    return this->Sun;
}


shared<HAmbientLightNode> HModelViewer::GetAmbientLight()
{
    return this->AmbientLight;
}


shared<HCameraNode> HModelViewer::GetCamera()
{
    return this->CurrentScene->GetActiveCamera();
}


shared<HTexture2D> HModelViewer::GetAmbientSourceHDR()
{
    return this->AmbientSourceHDR;
}


void HModelViewer::LoadScene(const DString& MeshFilepath, EAxis Right, EAxis Up, EAxis Back, float ScaleToMeter, bool FlipNormalmapGreen)
{   
    DString extension = SPath::GetExtension(MeshFilepath).ToLower();
    
    if (this->MeshExtensions.Contains(extension) == false) return;

    this->UnloadScene();
    
    // -------------------- LOAD MESH ----------------------

    double start = SClock::GetEpochSec();
    
    bool success = false;
    shared<HStaticMesh> mesh;
    if (extension == TX("asset"))
    {
        mesh = MakeShared<HStaticMesh>();
        mesh->SetFilepath(MeshFilepath);
        mesh->Load();
        
        if (mesh == nullptr || mesh->IsLoaded() == false) return;

        int32 materialCount = mesh->GetMaterialSlots().Length();
        for (int32 i = 0; i < materialCount; ++i)
        {
            DSurfaceMaterialSettings materialSettings;
            materialSettings.Shader = this->GeneralShader;
            shared<HSurfaceMaterial> material = MakeShared<HSurfaceMaterial>();
            material->Load(materialSettings);
            mesh->SetMaterialInSlot(i, material);
        }

        shared<HStaticMeshNode> meshNode = MakeShared<HStaticMeshNode>();
        meshNode->SetMesh(mesh);
        this->ModelRoot->AddChild(meshNode);
        meshNode->SetName(TX("StaticMeshNode"));

        this->CurrentScene->AddMeshToLightGroup(0, meshNode);

        success = true;
    }
    else if (extension == TX("obj"))
    {
        DString8 wavefrontStr;
        if (STextFile::ReadUTF8File(MeshFilepath, /*out*/wavefrontStr) == false) return;

        DWavefrontParser wavefront;
        DWavefrontParser::DImportOptions options;
        options.Right = Right;
        options.Up = Up;
        options.Back = Back;
        options.ScaleToMeter = ScaleToMeter;

        success = wavefront.ParseScene(wavefrontStr, options, this->ModelRoot, this->GeneralShader);

        if (success)
        {
            this->CurrentScene->ForEachNode(this->ModelRoot, [this](const shared<HSceneNode>& node) {

                if (node->Type == ESceneNodeType::STATIC_MESH)
                {
                    this->CurrentScene->AddMeshToLightGroup(0, StaticCast<HStaticMeshNode>(node));
                }
            });
        }
    }
    else if (extension == TX("gltf") || extension == TX("glb"))
    {
        SGltfImporter::DImportOptions options;
        options.ScaleToMeter = ScaleToMeter;
        options.FlipNormalmapGreen = FlipNormalmapGreen;
        
        if (extension == TX("gltf"))
            success = SGltfImporter::ReadSceneFromGLTF(MeshFilepath, this->GeneralShader, this->ModelRoot, options);
        else
            success = SGltfImporter::ReadSceneFromGLB(MeshFilepath, this->GeneralShader, this->ModelRoot, options);
        if (success)
        {
            this->CurrentScene->ForEachNode(this->ModelRoot, [this](const shared<HSceneNode>& node) {
                
                if (node->Type == ESceneNodeType::STATIC_MESH)
                {
                    this->CurrentScene->AddMeshToLightGroup(0, StaticCast<HStaticMeshNode>(node));
                }
            });
        }
    }
    
    if (success)
    {
        SLog::Message(TX("Loaded scene: ") + MeshFilepath);
    }
    else
    {
        SLog::Message(TX("Failed to load scene: ") + MeshFilepath);
    }

    // ----------- TRY TO POSITION CAMERA TO SEE ALL MESHES ------------

    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;

    HCameraNode* camera = this->CurrentScene->GetActiveCameraPtr();

    // get bounding box of scene
    FAxisBox bbox;
    this->CurrentScene->ForEachNode(this->ModelRoot.Ptr(), [&bbox](HSceneNode* node) {

        if (node->Type == ESceneNodeType::STATIC_MESH)
        {
            HStaticMeshNode* meshNode = static_cast<HStaticMeshNode*>(node);
            bbox.Extend(meshNode->GetBoundingBox().ToAxisBox());
        }
    });

    FVec3 dims = bbox.Dimensions();
    this->ModelRoot->SetLocation(this->ModelRoot->GetWorldLocation() - bbox.Center());
    float camOff_Z = dims.Z * .5f + FMath::Max(dims.X, dims.Y);
    camera->SetLocation(FVec3(.0f, .0f, camOff_Z));
    camera->SetRotation(FQuat());
}


void HModelViewer::UnloadScene()
{
    this->ModelRoot->ClearChildren();
    for (int32 i = 0; i < HScene::LIGHT_GROUP_COUNT; ++i)
    {
        this->CurrentScene->RemoveAllMeshesFromLightingGroup(i);
    }
    this->UpdateSelectionBracket();
    this->ModelRoot->SetLocationAndRotation(FVec3(), FQuat());
}


void HModelViewer::LoadTexture(int32 MaterialIndex, const DString& TextureFilepath, EModelViewerTexture TextureType, bool FlipGreen)
{
    DString extension = SPath::GetExtension(TextureFilepath).ToLower();

    if (this->TextureExtensions.Contains(extension) == false) return;

    shared<HSurfaceMaterial> material = this->GetMaterial(MaterialIndex);
    if (material == nullptr) return;

    bool success = false;
    shared<HTexture2D> texture;
    if (extension == TX("asset"))
    {
        texture = MakeShared<HTexture2D>();
        texture->SetFilepath(TextureFilepath);
        texture->Load();
        success = texture->IsLoaded();
    }
    else
    {
        switch (TextureType)
        {
        case EModelViewerTexture::BASE_COLOR:
        {
            DBitmapImage imageData;
            DTexture2DSettings settings;
            if (imageData.Load(TextureFilepath, 4, 255))
            {
                texture = MakeShared<HTexture2D>();
                settings.Format = ETexture2DFormat::RGBA8;
                settings.sRGB = true;
                texture->Load(imageData, settings);
                success = true;
            }
            break;
        }
        case EModelViewerTexture::REFLECTANCE:
        {
            DBitmapImage imageData;
            if (imageData.Load(TextureFilepath, 3, 255))
            {
                texture = MakeShared<HTexture2D>();
                DTexture2DSettings settings;
                settings.Format = ETexture2DFormat::RGB8;
                settings.sRGB = false;
                texture->Load(imageData, settings);
                success = true;
            }
            break;
        }
        case EModelViewerTexture::NORMAL:
        {
            DBitmapImage imageData;
            DTexture2DSettings settings;
            if (imageData.Load(TextureFilepath, 3, 255))
            {
                if (FlipGreen)
                {
                    imageData.InvertChannel(1);
                }
                texture = MakeShared<HTexture2D>();
                settings.Format = ETexture2DFormat::RGB8;
                settings.sRGB = false;
                texture->Load(imageData, settings);
                success = true;
            }
            break;
        }
        case EModelViewerTexture::EMISSION:
        {
            DBitmapImage imageData;
            DTexture2DSettings settings;
            if (imageData.Load(TextureFilepath, 3, 255))
            {
                texture = MakeShared<HTexture2D>();
                settings.Format = ETexture2DFormat::RGB8;
                settings.sRGB = false;
                texture->Load(imageData, settings);
                success = true;
            }
            break;
        }
        }
    }

    if (success)
    {
        switch (TextureType)
        {
        case EModelViewerTexture::BASE_COLOR:
        {
            material->SetTextureParam(TX("BaseColorTex"), texture);
            material->SetIntParam(TX("UseBaseColorTex"), 1);
            break;
        }
        case EModelViewerTexture::REFLECTANCE:
        {
            material->SetTextureParam(TX("ORMTex"), texture);
            material->SetIntParam(TX("UseORMTex"), 1);
            material->SetFloatParam(TX("Roughness"), .0f);
            material->SetFloatParam(TX("AO"), .0f);
            break;
        }
        case EModelViewerTexture::NORMAL:
        {
            material->SetTextureParam(TX("NormalMap"), texture);
            material->SetIntParam(TX("UseNormalMap"), 1);
            break;
        }
        case EModelViewerTexture::EMISSION:
        {
            material->SetTextureParam(TX("EmissionTex"), texture);
            material->SetIntParam(TX("UseEmissionTex"), 1);
            break;
        }
        }

        SLog::Message(TX("Loaded texture: ") + TextureFilepath);
    }
    else
    {
        SLog::Message(TX("Failed to load texture: ") + TextureFilepath);
    }
}


void HModelViewer::UnloadTexture(int32 MaterialIndex, EModelViewerTexture TextureType)
{
    shared<HSurfaceMaterial> material = this->GetMaterial(MaterialIndex);
    if (material)
    {
        switch (TextureType)
        {
        case EModelViewerTexture::BASE_COLOR:
        {
            material->SetTextureParam(TX("BaseColorTex"), nullptr);
            material->SetIntParam(TX("UseBaseColorTex"), 0);
            break;
        }
        case EModelViewerTexture::REFLECTANCE:
        {
            material->SetTextureParam(TX("ORMTex"), nullptr);
            material->SetIntParam(TX("UseORMTex"), 0);
            material->SetFloatParam(TX("Roughness"), .5f);
            material->SetFloatParam(TX("AO"), 1.0f);
            break;
        }
        case EModelViewerTexture::NORMAL:
        {
            material->SetTextureParam(TX("NormalMap"), nullptr);
            material->SetIntParam(TX("UseNormalMap"), 0);
            break;
        }
        case EModelViewerTexture::EMISSION:
        {
            material->SetTextureParam(TX("EmissionTex"), nullptr);
            material->SetIntParam(TX("UseEmissionTex"), 0);
            break;
        }
        }
    }
}


void HModelViewer::LoadAmbientHDR(const DString& HDRFilepath, float UOffset)
{
    DString extension = SPath::GetExtension(HDRFilepath).ToLower();
    if (this->HDRExtensions.Contains(extension) == false) return;
    
    this->UnloadAmbientHDR();

    // can't be loaded from .asset files 
    // ambient cubemap rotation requires regenerating irradiance/reflecton maps

    DBitmapImageHDR imageData;
    DTexture2DSettings settings;
    if (imageData.Load(HDRFilepath, 3, 1.0f))
    {
        this->AmbientSourceHDR = MakeShared<HTexture2D>();
        settings.Format = ETexture2DFormat::RGB16F;
        this->AmbientSourceHDR->Load(imageData, settings);

        SLog::Message(TX("Loaded HDR: ") + HDRFilepath);
        this->GenerateIBLMaps(UOffset, /*Log*/true);

        this->CurrentScene->SetSkyboxTexture(this->AmbientCubemap);
    }
    else
    {
        SLog::Message(TX("Failed to load HDR") + HDRFilepath);
    }
}


void HModelViewer::GenerateIBLMaps(float UOffset, bool Log)
{
    if (this->AmbientSourceHDR == nullptr) return;

    DArray<shared<HTexture2D>> eqMaps (1);
    eqMaps.Add(this->AmbientSourceHDR);

    int32 cubemapSideRes = this->AmbientSourceHDR->GetWidth() / 4;
    cubemapSideRes = FMath::Clamp(cubemapSideRes, 128, 1024);

    DArray<shared<HRenderTargetCube>> cubemaps;
    this->EqMapConverter->EquirectangularToCubemap(eqMaps, cubemapSideRes, ERenderTargetCubeFormat::RGB16F, /*out*/cubemaps, UOffset);
    
    if (cubemaps.IsEmpty())
    {
        if (Log)
        {
            SLog::Message(TX("Failed to Generated Cubemaps from Equirectangular HDR"));
        }
        return;
    }

    this->AmbientCubemap = cubemaps[0];

    DArray<shared<HRenderTargetCube>> irradianceMaps;
    DArray<shared<HRenderTargetCube>> reflectionMaps;
    this->IBLConverter->GenerateLightmaps(cubemaps, 32, 128, /*out*/irradianceMaps, /*out*/reflectionMaps);

    if (irradianceMaps.IsEmpty() == false && reflectionMaps.IsEmpty() == false)
    {
        this->AmbientLight->SetIrradianceMap(irradianceMaps[0]);
        this->AmbientLight->SetReflectionMap(reflectionMaps[0]);

        if (Log)
        {
            SLog::Message(TX("Generated Cubemaps from Equirectangular HDR"));
        }
    }
    else if (Log)
    {
        SLog::Message(TX("Failed to Generated Cubemaps from Equirectangular HDR"));
    }
}


void HModelViewer::UnloadAmbientHDR()
{
    this->AmbientSourceHDR = nullptr;
    this->AmbientLight->SetIrradianceMap(nullptr);
    this->AmbientLight->SetReflectionMap(nullptr);
    this->CurrentScene->SetSkyboxTexture(nullptr);
}


void HModelViewer::ResetModelTransform()
{
    if (this->CurrentScene == nullptr || this->ModelRoot == nullptr) return;

    this->ModelRoot->SetRotation(FQuat());
}


void HModelViewer::ResetCameraTranslation()
{
    if (this->CurrentScene == nullptr && this->CurrentScene->GetActiveCamera()) return;
    
    this->CurrentScene->GetActiveCamera()->SetLocation(FVec3());
}


void HModelViewer::ResetCameraRotation()
{
    if (this->CurrentScene == nullptr && this->CurrentScene->GetActiveCamera()) return;

    this->CurrentScene->GetActiveCamera()->SetRotation(FQuat());
}


void HModelViewer::RotateModelHorizontally(float Value)
{
    if (this->CurrentScene == nullptr || this->ModelRoot == nullptr) return;

    this->ModelRoot->WorldRotate(FQuat::FromEuler(.0f, FMath::DegToRad(Value), .0f));
}


void HModelViewer::RotateModelVertically(float Value)
{
    if (this->CurrentScene == nullptr || this->ModelRoot == nullptr) return;

    this->ModelRoot->WorldRotate(FQuat::FromEuler(FMath::DegToRad(Value), .0f, .0f));
}


void HModelViewer::RotateCameraMouseDelta(float Dx, float Dy)
{
    int32 width, height;
    this->Renderer->GetSize(/*out*/width, /*out*/height);

    float speed = 2.0f;

    if (width != 0) Dx = Dx / width;
    if (height != 0) Dy = Dy / height;

    this->RotateCamera(-Dx * speed, -Dy * speed);
}


void HModelViewer::RotateCamera(float H, float V)
{
    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;
    
    shared<HCameraNode> cam = this->CurrentScene->GetActiveCamera();

    FQuat hRot = cam->GetWorldRotation().Inverse();
    hRot *= FQuat::FromAxisAngle(HSceneNode::UP, H);
    hRot *= cam->GetWorldRotation();
    hRot = cam->GetRotation() * hRot;

    FQuat vRot = FQuat::FromAxisAngle(HSceneNode::RIGHT, V);

    // interp: GetRotation() -> hRot, Quat() -> vRot

    cam->SetRotation(hRot * vRot);
}


void HModelViewer::TranslateModel(const FVec3& Vec)
{
    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;

    this->ModelRoot->Translate(Vec);
}


void HModelViewer::TranslateCameraForward(float Value)
{
    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;

    shared<HCameraNode> cam = this->CurrentScene->GetActiveCamera();
    cam->Translate(cam->CalcForward() * Value);
}


void HModelViewer::TranslateCameraRight(float Value)
{
    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;

    shared<HCameraNode> cam = this->CurrentScene->GetActiveCamera();
    cam->Translate(cam->CalcRight() * Value);
}


void HModelViewer::TranslateCameraUp(float Value)
{
    if (this->CurrentScene == nullptr || this->CurrentScene->GetActiveCamera() == nullptr) return;

    shared<HCameraNode> cam = this->CurrentScene->GetActiveCamera();
    cam->Translate(cam->CalcUp() * Value);
}


void HModelViewer::SetSunRotation(float RotXDeg, float RotYDeg)
{
    if (this->Sun == nullptr) return;

    this->Sun->SetRotation(
        FQuat::SLerp(
            FQuat::FromEuler(FMath::DegToRad(0.0f), FMath::DegToRad(RotYDeg), .0f), 
            FQuat::FromEuler(FMath::DegToRad(-90.0f), FMath::DegToRad(RotYDeg), .0f), 
            FMath::ReverseLerp(.0f, -90.0f, RotXDeg)
        )
    );
}


void HModelViewer::Select(int32 ScreenX, int32 ScreenY)
{
    this->Renderer->OnNextFrame.Bind([this, ScreenX, ScreenY]() {
        
        uint8 stencilValue = this->Renderer->GetStencilValueAt(ScreenX, ScreenY);
        if (stencilValue > 0)
        {
            HCameraNode* cam = this->CurrentScene->GetActiveCameraPtr();
            if (cam)
            {
                this->CurrentScene->ForEachNodeWithBreak(this->ModelRoot, [this, stencilValue, cam](const shared<HSceneNode>& node) {

                    if (node->Type != ESceneNodeType::STATIC_MESH) return false;

                    const DFrustumPlanes& frustum = cam->GetCameraData().FrustumPlanes;
                    shared<HStaticMeshNode> meshNode = StaticCast<HStaticMeshNode>(node);
                    if (
                        meshNode->IsVisible() &&
                        frustum.Test(meshNode->GetBoundingSphere()) &&
                        frustum.Test(meshNode->GetBoundingBox()) &&
                        stencilValue == meshNode->GetLastStencilValue()
                    )
                    {
                        this->CurrentMeshNode = meshNode;
                        meshNode->AddChild(this->SelectionBracket);
                        return true;  // break
                    }

                    return false;
                });
            }
            else
            {
                this->CurrentMeshNode = nullptr;
            }
        }
        else  // deselect all
        {
            this->CurrentMeshNode = nullptr;
        }
    
        this->UpdateSelectionBracket();
    });
}


void HModelViewer::UpdateSelectionBracket()
{
    if (this->CurrentMeshNode)
    {
        shared<HStaticMeshNode> meshNode = this->CurrentMeshNode;
        shared<HStaticMesh> mesh = meshNode->GetMesh();
        if (mesh)
        {
            DLineWorkData lineWorkData;
            DLineStrokeData::BracketsFromBounds( 
                mesh->GetBoundingBox(), 
                /*out*/lineWorkData.Stroke
            );
            lineWorkData.Stroke.LineWidth = 3;
            lineWorkData.UpdateBounds();

            this->SelectionBracket->GetLineWork()->UpdateData(lineWorkData);
            this->SelectionBracket->SetWorldTransform(
                meshNode->GetWorldLocation(),
                meshNode->GetWorldRotation(),
                meshNode->GetWorldScale()
            );
            this->SelectionBracket->SetVisible(true);
            return;
        }
    }

    this->SelectionBracket->SetVisible(false);
}


shared<HRenderer> HModelViewer::GetRenderer() const
{
    return this->Renderer;
}


REFLECT(DModelViewerConfig)
FIELD(MinOpenGLVersionMajor)
FIELD(MinOpenGLVersionMinor)
FIELD(WindowWidth)
FIELD(WindowHeight)
FIELD(UIWidth)
REFLECT_END()
