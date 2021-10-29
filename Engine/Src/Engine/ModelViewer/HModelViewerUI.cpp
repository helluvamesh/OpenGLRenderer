#include "Engine\pch.h"
#include "HModelViewerUI.h"
#include "Engine\Util\SEngine.h"


HModelViewerUI::HModelViewerUI(
    shared<HWindow3D> InWindow, shared<HRenderer> InRenderer, shared<HModelViewer> InViewer, 
    const DModelViewerConfig& Config
) :
    
    UIPos(0, 0),
    UISize(-1, 500),
    LabelOffsetMul(.8f),
    VSpacingMul(.4f),
    PaddingMul(.1f, .1f),
    DropTargetSizeMul(5, 5),
    WideDropTargetSizeMul(10, 5),
    WindowBg(.05f, .05f, .05f, .85f),
    LabelFg(.7f, .7f, .7f, 1),
    FrameBg({.1f, .1f, .1f, .85f}),
    FrameBgHover({.15f, .15f, .15f, 1}),
    FrameBgActive({.15f, .15f, .15f, 1}),
    HeaderBg({.12f, .12f, .12f, .85f}),
    HeaderBgHover({.15f, .15f, .15f, 1}),
    HeaderBgActive({.12f, .12f, .12f, 1}),
    DropTargetBorder(.25f, .5f, 1, .5f),
    DropTargetFg(.25f, .5f, 1, 1),
    DropTargetBaseColorBg(1, 0, 0, .05f),
    DropTargetReflectanceBg(1, .75f, .0, .05f),
    DropTargetNormalBg(.5f, .5f, 1, .05f),
    DropTargetEmissionBg(.5f, 1, .5f, .05f),
    DropTargetAmbientHDRBg(1, 0, 0, .05f),
    ButtonBg(.12f, .12f, .12f, .4f),
    ButtonBgHover(.15f, .15f, .15f, 1),
    ButtonBgActive(.1f, .1f, .1f, 1),
    ButtonPaddingMul(2.0f, .5f),
    InteractiveTextFg(.25f, .5f, 1, 1),
    MenuItemBgHover(.26f, .59f, .98f, .4f),
    MenuItemBgActive(.14f, .29f, .68f, 1),

    MaterialBlendModeTexts({ "Opaque", "Alpha Test" }),
    AxisTexts({ "X", "Y", "Z", "-X", "-Y", "-Z" }),
    ShadowResolutionTexts({ "4096", "2048", "1024", "512", "256", "128" }),
    MSAATexts({ "8", "4", "2" }),

    LeftMouseDown(false),
    RightMouseDown(false),
    UpKeyDown(false),
    DownKeyDown(false),
    LeftKeyDown(false),
    RightKeyDown(false),
    IsCtrlDown(false),
    LeftDragDetected(false),

    CameraStridePerSec(8.0f), 
    CameraMode(EModelViewerCameraMode::WALK),
    UIContext(EModelViewerUIContext::SCENE),
    UIWidth(300),
    HoveredDropTarget(EDropTarget::NONE),
    MaterialIndex(0),
    SunXRotDeg(-45),
    SunYRotDeg(45),
    AmbientAngle(0),
    SwizzleRight(EAxis::X),
    SwizzleUp(EAxis::Y),
    SwizzleBack(EAxis::Z),
    ScaleToMeter(1),
    DirectXNormalmap(false)

{
    int32 materialLimit = 20;
    this->MaterialIndexTexts.Resize(materialLimit);
    for (int32 i = 0; i < materialLimit; ++i)
    {
        this->MaterialIndexTexts[i] = DNumberFormat8::DEFAULT.ToString(i);
    }

    this->Window = InWindow;
    this->Renderer = InRenderer;
    this->Viewer = InViewer;

    this->UIWidth = Config.UIWidth;
}


void HModelViewerUI::Init()
{
    /* SharedThis/WeakThis returns nullptr until the object is fully created */

    this->Window->OnRenderUI.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::Draw);
    this->Window->OnKey.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnKeyEvent);
    this->Window->OnMouseButton.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnMouseButton);
    this->Window->OnMouseMotion.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnMouseMotion);
    this->Window->OnFileDrop.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnFileDrop);
    this->Window->OnScroll.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnScroll);

    this->Renderer->OnEveryFrame.Bind(WeakThisAs<HModelViewerUI>(), &HModelViewerUI::OnEveryFrame);
}


void HModelViewerUI::Draw()
{
    this->HoveredDropTarget = EDropTarget::NONE;

    shared<HStaticMeshNode> meshNode = this->Viewer->GetMeshNode();
    shared<HStaticMesh> mesh = meshNode? meshNode->GetMesh() : nullptr;
    shared<HSurfaceMaterial> material;
    if (mesh && mesh->GetMaterialSlots().IsValidIndex(this->MaterialIndex))
    {
        material = mesh->GetMaterialSlots()[this->MaterialIndex].Material;
    }
    else
    {
        this->MaterialIndex = 0;
        if (mesh && mesh->GetMaterialSlots().IsEmpty() == false)
        {
            material = mesh->GetMaterialSlots()[0].Material;
        }
    }
    
    // Adjust UI size to 3d window (ImGui calls it's main ui element "window", not to be confused with the actual window)
    ImGui::SetNextWindowPos(this->UIPos);

    int32 winWidth, winHeight;
    this->Window->GetSize(/*out*/winWidth, /*out*/winHeight);
    ImGui::SetNextWindowSize(ImVec2((float)this->UIWidth, (float)winHeight));
    
    ImGui::Begin("Details", 0);

    this->UIWidth = (int32)ImGui::GetWindowWidth();

    // Calc sizes relative to line height
    if (this->LabelOffset == .0f)
    {
        float lineHeight = ImGui::GetTextLineHeight();
        this->LabelOffset = lineHeight * this->LabelOffsetMul;
        this->VSpacing = lineHeight * this->VSpacingMul;
        this->Padding = ImVec2(
            lineHeight * this->PaddingMul.x,
            lineHeight * this->PaddingMul.y
        );
        this->ButtonPadding = ImVec2(
            lineHeight * this->ButtonPaddingMul.x,
            lineHeight * this->ButtonPaddingMul.y
        );
        this->DropTargetSize = ImVec2(
            lineHeight * this->DropTargetSizeMul.x,
            lineHeight * this->DropTargetSizeMul.y
        );
        this->WideDropTargetSize = ImVec2(
            lineHeight * this->WideDropTargetSizeMul.x,
            lineHeight * this->WideDropTargetSizeMul.y
        );
        // to align drop target labels to center vertically
        this->DropTargetLabelPadding = ImVec2(this->LabelOffset, (this->DropTargetSize.y - ImGui::GetTextLineHeight()) * .5f);
    }


    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = this->WindowBg;
    style.Colors[ImGuiCol_Text] = this->LabelFg;
    style.Colors[ImGuiCol_HeaderHovered] = this->MenuItemBgHover;
    style.Colors[ImGuiCol_HeaderActive] = this->MenuItemBgActive;
    style.Colors[ImGuiCol_FrameBg] = this->FrameBg;
    style.Colors[ImGuiCol_FrameBgHovered] = this->FrameBgHover;
    style.Colors[ImGuiCol_FrameBgActive] = this->FrameBgActive;
    style.Colors[ImGuiCol_Header] = this->HeaderBg;
    style.Colors[ImGuiCol_HeaderHovered] = this->HeaderBgHover;
    style.Colors[ImGuiCol_HeaderActive] = this->HeaderBgActive;
    style.Colors[ImGuiCol_Button] = this->ButtonBg;
    style.Colors[ImGuiCol_ButtonHovered] = this->ButtonBgHover;
    style.Colors[ImGuiCol_ButtonActive] = this->ButtonBgActive;
    style.Colors[ImGuiCol_Text] = this->LabelFg;
    style.FramePadding = this->Padding;

    // Help
    ImGui::PushID("Help");
    style.Colors[ImGuiCol_Text] = this->InteractiveTextFg;
    ImGui::LabelText("", "Help");
    style.Colors[ImGuiCol_Text] = this->LabelFg;
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Drop resources into the viewport or in the boxes on the sidebar.");
        ImGui::Text("Click on a box to unload.");
        ImGui::Text("Viewport navigation: WASD, CTRL+WASD, arrows, mouse scrolling.");
        ImGui::Text("Camera orientation in Walk mode: right click + drag");
        ImGui::Text("Horizontal model rotation in Turntable mode: left click + drag");
        ImGui::Text("Vertical model rotation in Turntable mode: right click + drag");
        ImGui::Text("Mesh formats: .gltf, .glb, .obj");
        ImGui::Text("Texture formats: .png, .hdr, .tga, .jpg, .psd");
        ImGui::EndTooltip();
    }

    style.Colors[ImGuiCol_Text] = this->LabelFg;
    this->AddVSpacing();
    
    // Context (main switch)
    ImGui::PushID("Context_Scene");
    if (ImGui::RadioButton("Scene", this->UIContext == EModelViewerUIContext::SCENE))
    {
        this->UIContext = EModelViewerUIContext::SCENE;
    }
    ImGui::PushID("Context_Lighting");
    if (ImGui::RadioButton("Lighting", this->UIContext == EModelViewerUIContext::LIGHTING))
    {
        this->UIContext = EModelViewerUIContext::LIGHTING;
    }
    ImGui::PushID("Context_Camera");
    if (ImGui::RadioButton("Camera", this->UIContext == EModelViewerUIContext::CAMERA))
    {
        this->UIContext = EModelViewerUIContext::CAMERA;
    }
    ImGui::PushID("Context_Renderer");
    if (ImGui::RadioButton("Renderer", this->UIContext == EModelViewerUIContext::RENDERER))
    {
        this->UIContext = EModelViewerUIContext::RENDERER;
    }

    this->AddVSpacing();

    switch (this->UIContext)
    {
    // Context - Scene
    case EModelViewerUIContext::SCENE:
    {
        this->HoveredDropTarget = EDropTarget::NONE;

        // Unload Scene
        ImGui::PushID("UnloadScene");
        style.FramePadding = this->ButtonPadding;
        if (ImGui::Button("Unload Scene"))
        {
            this->Renderer->OnNextFrame.Bind([this]() {

                this->Viewer->UnloadScene();
            });
        }
        style.FramePadding = this->Padding;

        this->AddVSpacing();
        
        style.FramePadding = this->Padding;
        
        if (ImGui::CollapsingHeader("Import Settings"))
        { 
            // Swizzle
            ImGui::LabelText("", "Swizzle:");
            this->DrawAxisSelector("SwizzleRight", this->SwizzleRight);
            ImGui::SameLine();
            this->DrawAxisSelector("SwizzleUp", this->SwizzleUp);
            ImGui::SameLine();
            this->DrawAxisSelector("SwizzleBack", this->SwizzleBack);

            // Scale
            ImGui::PushID("ToMeterScale:");
            ImGui::SetNextItemWidth(this->DropTargetSize.x);
            ImGui::DragFloat("", &this->ScaleToMeter, .01f, 0, 1000, "%.3f", 1);
            ImGui::SameLine();
            style.FramePadding = { this->LabelOffset, this->Padding.y };
            ImGui::LabelText("", "Scale To Meter:");
            style.FramePadding = this->Padding;

            // DirectXNormalmap
            ImGui::PushID("DirectXNormalmap");
            ImGui::Checkbox("DirectX Normalmap", &this->DirectXNormalmap);
        }

        this->AddVSpacing(2);

        if (material)
        {
            // Material Index
            style.FramePadding = this->Padding;
            ImGui::SetNextItemWidth(this->DropTargetSize.x);
            ImGui::PushID("MaterialIndex");
            if (ImGui::BeginCombo("", this->MaterialIndexTexts[this->MaterialIndex].CString()))
            {
                if (mesh)
                {
                    int32 len = FMath::Min(this->MaterialIndexTexts.Length(), mesh->GetMaterialSlots().Length());
                    for (int32 i = 0; i < len; ++i)
                    {
                        if (ImGui::Selectable(this->MaterialIndexTexts[i].CString(), false))
                        {
                            this->MaterialIndex = i;
                        }
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            style.FramePadding = { this->LabelOffset, 0 };
            ImGui::LabelText("", "Material Index");

            style.FramePadding = this->Padding;

            this->AddVSpacing();

            // Blend Mode
            ImGui::LabelText("", "Blend Mode:");
            ImGui::PushID("MaterialBlendMode");
            if (ImGui::BeginCombo("", this->MaterialBlendModeTexts[(int32)material->GetBlendMode()].CString()))
            {
                for (int32 i = 0; i < this->MaterialBlendModeTexts.Length(); ++i)
                {
                    if (ImGui::Selectable(this->MaterialBlendModeTexts[i].CString(), false))
                    {
                        material->SetBlendMode((ESurfaceMaterialBlendMode)i);
                    }
                }
                ImGui::EndCombo();
            }

            // Double Sided
            ImGui::PushID("DoubleSided");
            bool doubleSided = material->IsDoubleSided();
            if (ImGui::Checkbox("Double Sided", &doubleSided))
            {
                material->SetDoubleSided(doubleSided);
            }

            style.FramePadding = this->DropTargetLabelPadding;

            // Base Color Texture
            this->DrawTextureDropTarget(
                material, TX("BaseColorTex"), EDropTarget::BASE_COLOR, HModelViewer::EModelViewerTexture::BASE_COLOR,
                "DropTargetBaseColor", "Base Color Texture", this->DropTargetBaseColorBg
            );

            // Reflectance Texture
            this->DrawTextureDropTarget(
                material, TX("ORMTex"), EDropTarget::REFLECTANCE, HModelViewer::EModelViewerTexture::REFLECTANCE,
                "DropTargetReflectance", "AO/Roughness/Metallic", this->DropTargetReflectanceBg
            );

            // Normalmap
            this->DrawTextureDropTarget(
                material, TX("NormalMap"), EDropTarget::NORMAL, HModelViewer::EModelViewerTexture::NORMAL,
                "DropTargetNormal", "Normalmap (Mikk TSpace)", this->DropTargetNormalBg
            );

            // Emission Texture
            this->DrawTextureDropTarget(
                material, TX("EmissionTex"), EDropTarget::EMISSION, HModelViewer::EModelViewerTexture::EMISSION,
                "DropTargetEmission", "Emission Texture", this->DropTargetEmissionBg
            );

            style.FramePadding = this->Padding;

            // Base Color
            ImGui::LabelText("", "Base Color:");
            ImGui::PushID("BaseColor");
            FVec4 baseColor = material->GetVec4Param(TX("BaseColor"));
            if (ImGui::ColorEdit3("", &baseColor.X, ImGuiColorEditFlags_Float))
            {
                material->SetVec4Param(TX("BaseColor"), baseColor);
            }

            // Specular
            this->DrawFloatMaterialParam(material, TX("Specular"), "Specular", "Specular:");

            // Roughness
            this->DrawFloatMaterialParam(material, TX("Roughness"), "Roughness", "Roughness:", -10, 10);

            // Roughness Max
            this->DrawFloatMaterialParam(material, TX("RoughnessMax"), "RoughnessMax", "Roughness Max:", -10, 10);

            // Metallic
            this->DrawFloatMaterialParam(material, TX("Metallic"), "Metallic", "Metallic:");

            // AO
            this->DrawFloatMaterialParam(material, TX("AO"), "AO", "AO:");

            // Opacity
            this->DrawFloatMaterialParam(material, TX("Opacity"), "Opacity", "Opacity:");

            // Emission Intensity
            ImGui::LabelText("", "Emission:");
            ImGui::PushID("Emission");
            FVec4 emission = material->GetVec4Param(TX("Emission"));
            if (ImGui::DragFloat3("", &emission.X, 1, 0, 10000, "%.1f", 1))
            {
                material->SetVec4Param(TX("Emission"), emission);
            }
        }

        break;
    }
    // Context - Camera
    case EModelViewerUIContext::CAMERA:
    {
        shared<HCameraNode> cam = this->Viewer->GetCurrentScene()->GetActiveCamera();
        if (cam)
        {
            ImGui::LabelText("", "Stride:");
            ImGui::PushID("CameraStride");
            float stride = this->GetCameraStride();
            if (ImGui::DragFloat("", &stride, .01f, .001f, 100.0f, "%.3f", 1.0f))
            {
                this->SetCameraStride(stride);
            }
        }

        this->AddVSpacing();

        ImGui::LabelText("", "Camera Rotation Mode:");
        ImGui::PushID("CameraMode_Walk");
        if (ImGui::RadioButton("Walk", this->CameraMode == EModelViewerCameraMode::WALK))
        {
            this->CameraMode = EModelViewerCameraMode::WALK;
        }
        ImGui::PushID("CameraMode_Turntable");
        if (ImGui::RadioButton("Turntable", this->CameraMode == EModelViewerCameraMode::TURN_TABLE))
        {
            this->CameraMode = EModelViewerCameraMode::TURN_TABLE;
        }

        this->AddVSpacing();

        style.FramePadding = this->ButtonPadding;

        if (ImGui::Button("Reset Camera Rotation"))
        {
            this->Viewer->ResetCameraRotation();
        }
        if (ImGui::Button("Reset Camera Translation"))
        {
            this->Viewer->ResetCameraTranslation();
        }
        if (ImGui::Button("Reset Scene Rotation"))
        {
            this->Viewer->ResetModelTransform();
        }

        style.FramePadding = this->Padding;

        break;
    }
    // Context - Lighting
    case EModelViewerUIContext::LIGHTING:
    {
        shared<HDirectionalLightNode> sun = this->Viewer->GetSun();
        if (sun)
        {
            // Sun Color
            ImGui::LabelText("", "Sun Color:");
            ImGui::PushID("SunColor");
            FVec4 sunColor = sun->GetLightColor();
            if (ImGui::ColorEdit3("", &sunColor.X, ImGuiColorEditFlags_Float))
            {
                sun->SetLightColor(sunColor);
            }

            // Sun Intensity
            ImGui::LabelText("", "Sun Intensity:");
            ImGui::PushID("SunIntensity");
            float sunIntensity = sun->GetLightIntensity();
            if (ImGui::DragFloat("", &sunIntensity, .01f, 0, 100, "%.3f", 1))
            {
                sun->SetLightIntensity(sunIntensity);
            }

            // Sun Angle
            ImGui::LabelText("", "Sun Angle X:");
            ImGui::PushID("SunXRotDeg");
            bool sunAngleChanged = ImGui::DragFloat("", &this->SunXRotDeg, 1, -90, 0, "%.0f", 1);

            // Sun Y Rotation
            ImGui::LabelText("", "Sun Angle Y:");
            ImGui::PushID("SunYRotDeg");
            bool sunYRotChanged = ImGui::DragFloat("", &this->SunYRotDeg, 1, -360, 360, "%.0f", 1);

            if (sunAngleChanged || sunYRotChanged)
            {
                this->Viewer->SetSunRotation(this->SunXRotDeg, this->SunYRotDeg);
            }

            // Sun Position
            ImGui::LabelText("", "Sun Location:");
            ImGui::PushID("SunLocation");
            FVec3 sunLoc = sun->GetLocation();
            if (ImGui::DragFloat3("", &sunLoc.X, 1.0f, -1000.0f, 1000.0f, "%.3f", 1.0f))
            {
                sun->SetLocation(sunLoc);
            }

            this->AddVSpacing();

            // Enable Shadows
            bool enableShadows = sun->GetEnableShadows();
            if (ImGui::Checkbox("Shadows", &enableShadows))
            {
                sun->SetEnableShadows(enableShadows);
            }

            // Shadow Cascades
            ImGui::LabelText("", "Shadow Cascades:");
            ImGui::PushID("ShadowCascades");
            int32 cascades = sun->GetShadowCascades();
            if (ImGui::InputInt("", &cascades, 1, 1, 4))
            {
                sun->SetShadowCascades(cascades);
            }

            // Shadow Resolution
            ImGui::LabelText("", "Shadow Resolution:");
            ImGui::PushID("ShadowResolution");
            int32 res = sun->GetShadowResolution();
            if (ImGui::BeginCombo("", DNumberFormat8::DEFAULT.ToString(res).CString()))
            {
                for (int32 i = 0; i < this->ShadowResolutionTexts.Length(); ++i)
                {
                    bool b = false;
                    if (ImGui::Selectable(this->ShadowResolutionTexts[i].CString(), &b))
                    {
                        sun->SetShadowResolution(DString8::ParseInt32(this->ShadowResolutionTexts[i]));
                    }
                }
                ImGui::EndCombo();
            }

            // Shadow Distance
            ImGui::LabelText("", "Shadow Distance:");
            ImGui::PushID("ShadowDistance");
            float shadowDistance = sun->GetShadowDistance();
            if (ImGui::DragFloat("", &shadowDistance, 1.0f, .0f, 1000.0f, "%.0f", 1.0f))
            {
                sun->SetShadowDistance(shadowDistance);
            }

            // Cascade Distribution
            ImGui::LabelText("", "Cascade Distribution:");
            ImGui::PushID("CascadeDistribution");
            float cascadeDistribution = sun->GetCascadeDistribution();
            if (ImGui::DragFloat("", &cascadeDistribution, .001f, .001f, 5.0f, "%.03f", 1.0f))
            {
                sun->SetCascadeDistribution(cascadeDistribution);
            }

            // Shadow Offset
            ImGui::LabelText("", "Shadow Extension:");
            ImGui::PushID("ShadowExtension");
            float shadowExtension = sun->GetShadowExtension();
            if (ImGui::DragFloat("", &shadowExtension, 1.0f, -1000.0f, 1000.0f, "%.3f", 1.0f))
            {
                sun->SetShadowExtension(shadowExtension);
            }

            // Shadow Map Bias
            ImGui::LabelText("", "Shadow Map Bias:");
            int32 cascadeCount = sun->GetShadowCascades();
            for (int32 i = 0; i < cascadeCount; ++i)
            {
                DString8 indexStr = DNumberFormat8::DEFAULT.ToString(i);
                
                ImGui::Text("Min:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(this->DropTargetSize.x);
                ImGui::PushID(("ShadowBiasMin" + indexStr).CString());
                float shadowBiasMin = sun->GetShadowBiasMin(i);
                if (ImGui::DragFloat("", &shadowBiasMin, .001f, .0f, 1000.0f, "%.3f", 1.0f))
                {
                    sun->SetShadowBiasMin(i, shadowBiasMin);
                }

                ImGui::SameLine();
                ImGui::Text("Max:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(this->DropTargetSize.x);
                ImGui::PushID(("ShadowBiasMax" + indexStr).CString());
                float shadowBiasMax = sun->GetShadowBiasMax(i);
                if (ImGui::DragFloat("", &shadowBiasMax, .001f, .0f, 1000.0f, "%.3f", 1.0f))
                {
                    sun->SetShadowBiasMax(i, shadowBiasMax);
                }
            }

            this->AddVSpacing();
        }

        // Ambient HDR
        shared<HTexture2D> ambientHDR = this->Viewer->GetAmbientSourceHDR();
        style.FramePadding = this->DropTargetLabelPadding;
        bool hdrLoaded = false;
        if (ambientHDR)
        {
            hdrLoaded = true;
            style.FramePadding = { 0, 0 };
            // image needs to be flipped on the y axis
            ImGui::PushID("AmbientHDR");
            if (ImGui::ImageButton((void*)(intptr_t)ambientHDR->GetTextureId(), this->WideDropTargetSize))
            {
                this->Renderer->OnNextFrame.Bind([this](){

                    this->Viewer->UnloadAmbientHDR();
                });
            }
        }
        if (hdrLoaded == false)
        {
            style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonActive] = this->DropTargetAmbientHDRBg;
            style.Colors[ImGuiCol_Text] = this->DropTargetFg;
            ImGui::PushID("AmbientHDR");
            ImGui::Button("None", this->WideDropTargetSize);
            style.Colors[ImGuiCol_Text] = this->LabelFg;
        }
        ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(this->DropTargetBorder));
        style.FramePadding = this->DropTargetLabelPadding;
        if (ImGui::IsItemHovered()) this->HoveredDropTarget = EDropTarget::AMBIENT_HDR;
        ImGui::SameLine();
        ImGui::LabelText("", "Ambient HDR");
        
        style.Colors[ImGuiCol_Button] = this->ButtonBg;
        style.Colors[ImGuiCol_ButtonHovered] = this->ButtonBgHover;
        style.Colors[ImGuiCol_ButtonActive] = this->ButtonBgActive;

        shared<HAmbientLightNode> ambientLight = this->Viewer->GetAmbientLight();
        if (ambientLight)
        {
            style.FramePadding = this->Padding;
            
            // Ambient Intensity
            ImGui::LabelText("", "Ambient Intensity:");
            ImGui::PushID("AmbientIntensity");
            float ambientIntensity = ambientLight->GetLightIntensity();
            if (ImGui::DragFloat("", &ambientIntensity, .01f, 0, 100, "%.3f", 1))
            {
                ambientLight->SetLightIntensity(ambientIntensity);
            }
            
            // Ambient Angle
            ImGui::LabelText("", "Ambient Angle:");
            ImGui::PushID("AmbientAngle");
            if (ImGui::DragFloat("", &this->AmbientAngle, 1, -360, 360, "%.0f", 1))
            {
                this->Renderer->OnNextFrame.Bind([this]() {
                    
                    this->Viewer->GenerateIBLMaps(FMath::DegToRad(this->AmbientAngle), /*Log*/false);
                });
            }

            // Ambient Tint
            ImGui::LabelText("", "Ambient Tint:");
            ImGui::PushID("AmbientTint");
            FVec4 ambientTint = ambientLight->GetLightColor();
            if (ImGui::ColorEdit3("", &ambientTint.X, ImGuiColorEditFlags_Float))
            {
                ambientLight->SetLightColor(ambientTint);
            }

            // Down Occlusion
            ImGui::LabelText("", "Down Occlusion:");
            ImGui::PushID("DownOcclusion");
            float downOcclusion = ambientLight->GetDownOcclusion();
            if (ImGui::DragFloat("", &downOcclusion, .01f, 0, 1, "%.3f", 1))
            {
                ambientLight->SetDownOcclusion(downOcclusion);
            }
        }
        break;
    }
    // Context - RENDERER
    case EModelViewerUIContext::RENDERER:
    {
        shared<HRenderer> renderer = this->Viewer->GetRenderer();
        if (renderer)
        {
            ImGui::Text("%.0f FPS", ImGui::GetIO().Framerate);

            this->AddVSpacing();

            ImGui::LabelText("", "MSAA Samples:");
            ImGui::PushID("MSAASamples");
            int32 msaa = renderer->GetMSAASamples();
            if (ImGui::BeginCombo("", DNumberFormat8::DEFAULT.ToString(msaa).CString()))
            {
                for (int32 i = 0; i < this->MSAATexts.Length(); ++i)
                {
                    bool b = false;
                    if (ImGui::Selectable(this->MSAATexts[i].CString(), &b))
                    {
                        renderer->SetMSAASamples(DString8::ParseInt32(this->MSAATexts[i]));
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::LabelText("", "Alpha To Coverage:");

            ImGui::PushID("AlphaToCoverage");
            float alphaToCoverage = renderer->GetAlphaToCoverage();
            if (ImGui::DragFloat("", &alphaToCoverage, .001f, .0f, 1.0f, "%.3f", 1.0f))
            {
                renderer->SetAlphaToCoverage(alphaToCoverage);
            }

            ImGui::LabelText("", "Bloom Downsampling:");
            ImGui::PushID("BloomDownsample_1");
            int32 bloomDownSample_1 = renderer->GetBloomDownsample_1();
            if (ImGui::InputInt("", &bloomDownSample_1, 1))
            {
                renderer->SetBloomDownsample_1(bloomDownSample_1);
            }

            ImGui::PushID("BloomDownsample_2");
            int32 bloomDownSample_2 = renderer->GetBloomDownsample_2();
            if (ImGui::InputInt("", &bloomDownSample_2, 1))
            {
                renderer->SetBloomDownsample_2(bloomDownSample_2);
            }

            ImGui::PushID("BloomDownsample_3");
            int32 bloomDownSample_3 = renderer->GetBloomDownsample_3();
            if (ImGui::InputInt("", &bloomDownSample_3, 1))
            {
                renderer->SetBloomDownsample_3(bloomDownSample_3);
            }

            ImGui::LabelText("", "Bloom Blur Iterations:");

            ImGui::PushID("BloomBlurIters_1");
            int32 bloomBlurIters_1 = renderer->GetBloomBlurIters_1();
            if (ImGui::InputInt("", &bloomBlurIters_1, 1))
            {
                renderer->SetBloomBlurIters_1(bloomBlurIters_1);
            }

            ImGui::PushID("BloomBlurIters_2");
            int32 bloomBlurIters_2 = renderer->GetBloomBlurIters_2();
            if (ImGui::InputInt("", &bloomBlurIters_2, 1))
            {
                renderer->SetBloomBlurIters_2(bloomBlurIters_2);
            }

            ImGui::PushID("BloomBlurIters_3");
            int32 bloomBlurIters_3 = renderer->GetBloomBlurIters_3();
            if (ImGui::InputInt("", &bloomBlurIters_3, 1))
            {
                renderer->SetBloomBlurIters_3(bloomBlurIters_3);
            }
        }
        break;
    }
    }

    ImGui::End();
}


void HModelViewerUI::DrawTextureDropTarget(
    shared<HSurfaceMaterial> Material,
    const DString& TextureParamName, EDropTarget DropTargetType, HModelViewer::EModelViewerTexture TextureType,
    const char* Id, const char* Label, const ImVec4& BackgrondColor
)
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.FramePadding = this->DropTargetLabelPadding;

    bool textureLoaded = false;
    if (Material)
    {
        shared<HTextureBase> texture = Material->GetTextureParam(TextureParamName);
        if (texture)
        {
            textureLoaded = true;
            style.FramePadding = { 0, 0 };
            ImGui::PushID(Id);
            if (ImGui::ImageButton((void*)(intptr_t)texture->GetTextureId(), this->DropTargetSize))
            {
                this->Renderer->OnNextFrame.Bind([this, TextureType](){
                    
                    this->Viewer->UnloadTexture(this->MaterialIndex, TextureType);
                });
            }
        }
    }
    if (textureLoaded == false)
    {
        style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonActive] = BackgrondColor;
        style.Colors[ImGuiCol_Text] = this->DropTargetFg;
        ImGui::PushID(Id);
        ImGui::Button("None", this->DropTargetSize);
        style.Colors[ImGuiCol_Text] = this->LabelFg;
    }
    ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(this->DropTargetBorder));
    style.FramePadding = this->DropTargetLabelPadding;
    if (ImGui::IsItemHovered()) this->HoveredDropTarget = DropTargetType;
    ImGui::SameLine();
    ImGui::LabelText("", Label);
}


void HModelViewerUI::DrawFloatMaterialParam(
    shared<HSurfaceMaterial> Material, const DString& MaterialParamName, const char* Id, const char* Label,
    float Min, float Max, float Step
)
{
    ImGui::LabelText("", Label);
    float v = Material->GetFloatParam(MaterialParamName);
    ImGui::PushID(Id);
    if (ImGui::DragFloat("", &v, Step, Min, Max, "%.3f", 1))
    {
        Material->SetFloatParam(MaterialParamName, v);
    }
}


void HModelViewerUI::DrawAxisSelector(const char* Id, EAxis& Var)
{
    ImGui::GetStyle().FramePadding = this->Padding;
    ImGui::PushID(Id);
    ImGui::SetNextItemWidth(this->DropTargetSize.x);
    if (ImGui::BeginCombo("", this->AxisTexts[(int32)Var].CString()))
    {
        for (int32 i = 0; i < this->AxisTexts.Length(); ++i)
        {
            if (ImGui::Selectable(this->AxisTexts[i].CString(), false))
            {
                Var = (EAxis)i;
            }
        }
        ImGui::EndCombo();
    }
}


void HModelViewerUI::AddVSpacing(float Multiplier)
{
    ImGui::InvisibleButton("", { 1, this->VSpacing * Multiplier });
}


void HModelViewerUI::OnKeyEvent(DKeyEvent Event)
{
    EKeyAction action = Event.GetAction();
    EKeyCode key = Event.GetKey();
    this->IsCtrlDown = Event.IsCtrlDown();
    if (action == EKeyAction::PRESS || action == EKeyAction::RELEASE)
    {
        switch (key)
        {
        case EKeyCode::W:
        case EKeyCode::UP:
        {
            this->UpKeyDown = action == EKeyAction::PRESS;
            break;
        }
        case EKeyCode::S:
        case EKeyCode::DOWN:
        {
            this->DownKeyDown = action == EKeyAction::PRESS;
            break;
        }
        case EKeyCode::A:
        case EKeyCode::LEFT:
        {
            this->LeftKeyDown = action == EKeyAction::PRESS;
            break;
        }
        case EKeyCode::D:
        case EKeyCode::RIGHT:
        {
            this->RightKeyDown = action == EKeyAction::PRESS;
            break;
        }
        }
    }
}


void HModelViewerUI::OnMouseButton(DMouseButtonEvent Event)
{
    switch (Event.GetKey())
    {
    case EMouseButton::LEFT:
    {
        this->LeftMouseDown = Event.GetAction() == EMouseAction::PRESS;
        if (Event.GetAction() == EMouseAction::RELEASE)
        {
            float vx, vy;
            this->Window->GetMousePos(vx, vy);
            if (this->LeftDragDetected == false)
            {
                this->Viewer->Select((int32)vx, (int32)vy);
            }
        }
        else if (Event.GetAction() == EMouseAction::PRESS)
        {
            this->LeftDragDetected = false;
        }
        break;
    }
    case EMouseButton::RIGHT:
    {
        this->RightMouseDown = Event.GetAction() == EMouseAction::PRESS;
        break;
    }
    case EMouseButton::MIDDLE:
    {
        if (Event.GetAction() == EMouseAction::PRESS)
        {
            switch (this->CameraMode)
            {
            case EModelViewerCameraMode::TURN_TABLE:
            {
                this->Viewer->ResetModelTransform();
                break;
            }
            }
        }
        break;
    }
    }
}


void HModelViewerUI::OnMouseMotion(DMouseMotionEvent Event)
{
    if (this->LeftMouseDown)
    {
        this->LeftDragDetected = true;
    }

    if (this->Viewer)
    {
        float dx, dy;
        this->Window->GetMouseDelta(dx, dy);

        switch (this->CameraMode)
        {
        case EModelViewerCameraMode::WALK:
        {
            if (this->RightMouseDown)
            {
                this->Viewer->RotateCameraMouseDelta(dx, dy);
            }
            break;
        }
        case EModelViewerCameraMode::TURN_TABLE:
        {
            if (this->LeftMouseDown)
            {
                this->Viewer->RotateModelHorizontally(dx);
            }
            else if (this->RightMouseDown)
            {
                this->Viewer->RotateModelVertically(dy);
            }
            break;
        }
        }
    }
}


void HModelViewerUI::OnEveryFrame()
{
    float deltaSec = (float)this->Renderer->GetDeltaSec();
    float stride = this->CameraStridePerSec * deltaSec;

    if (this->UpKeyDown)
    {
        if (this->IsCtrlDown)
            this->Viewer->TranslateCameraUp(stride);
        else
            this->Viewer->TranslateCameraForward(stride);
    }
    if (this->DownKeyDown)
    {
        if (this->IsCtrlDown)
            this->Viewer->TranslateCameraUp(-stride);
        else
            this->Viewer->TranslateCameraForward(-stride);
    }
    if (this->LeftKeyDown)
    {
        this->Viewer->TranslateCameraRight(-stride);
    }
    if (this->RightKeyDown)
    {
        this->Viewer->TranslateCameraRight(stride);
    }
}


void HModelViewerUI::OnFileDrop(const DArray<DString>& Filepaths)
{
    if (ImGui::GetIO().WantCaptureMouse == false)  // outside of ui
    {
        if (Filepaths.IsEmpty() == false)
        { 
            const DString& filepath = Filepaths[0];

            this->Renderer->OnNextFrame.Bind([this, filepath](){

                this->Viewer->LoadScene(
                    filepath, this->SwizzleRight, this->SwizzleUp, this->SwizzleBack, this->ScaleToMeter, this->DirectXNormalmap
                );
            });
        }
    }
    else  // inside ui
    { 
        if (Filepaths.IsEmpty() == false)
        {
            const DString& filepath = Filepaths[0];
            
            switch (this->HoveredDropTarget)
            {
            case HModelViewerUI::EDropTarget::BASE_COLOR:
            {
                this->Renderer->OnNextFrame.Bind([this, filepath]() {
                
                    this->Viewer->LoadTexture(this->MaterialIndex, filepath, HModelViewer::EModelViewerTexture::BASE_COLOR);
                });
                break;
            }
            case HModelViewerUI::EDropTarget::REFLECTANCE:
            {
                this->Renderer->OnNextFrame.Bind([this, filepath]() {
                
                    this->Viewer->LoadTexture(this->MaterialIndex, filepath, HModelViewer::EModelViewerTexture::REFLECTANCE);
                });
                break;
            }
            case HModelViewerUI::EDropTarget::NORMAL:
            {
                this->Renderer->OnNextFrame.Bind([this, filepath]() {

                    this->Viewer->LoadTexture(this->MaterialIndex, filepath, HModelViewer::EModelViewerTexture::NORMAL, this->DirectXNormalmap);
                });
                break;
            }
            case HModelViewerUI::EDropTarget::EMISSION:
            {
                this->Renderer->OnNextFrame.Bind([this, filepath]() {

                    this->Viewer->LoadTexture(this->MaterialIndex, filepath, HModelViewer::EModelViewerTexture::EMISSION);
                });
                break;
            }
            case HModelViewerUI::EDropTarget::AMBIENT_HDR:
            {
                this->Renderer->OnNextFrame.Bind([this, filepath]() {

                    this->Viewer->LoadAmbientHDR(filepath, FMath::DegToRad(this->AmbientAngle));
                });
                break;
            }
            }
        }
    }
}


void HModelViewerUI::OnScroll(DScrollEvent Event)
{
    this->Viewer->TranslateCameraForward(this->CameraStridePerSec * Event.GetY() * .5f);
}


float HModelViewerUI::GetCameraStride() const
{
    return this->CameraStridePerSec;
}
void HModelViewerUI::SetCameraStride(float Value)
{
    this->CameraStridePerSec = FMath::Clamp(Value, .001f, 100.0f);
}
