#include "Engine/pch.h"
#include "HRenderer.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Engine/Render/RenderUtil/GLUtil.h"
#include "Engine/Util/SEngine.h"


int32 HRenderer::CompareOpenGLVersions(int32 MajorA, int32 MinorA, int32 MajorB, int32 MinorB)
{
    int32 A = MajorA * 10 + MinorA;
    int32 B = MajorB * 10 + MinorB;
    return A - B;
}


void HRenderer::GetMinOpenGLVersion(int32& OutMajor, int32& OutMinor)
{
    OutMajor = MinOpenGLMajor;
    OutMinor = MinOpenGLMinor;
}


void HRenderer::RenderStart()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    // Backface Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // MSAA
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glCheck(glSampleCoverage(this->AlphaToCoverage, false));
    
    // ---------------------------------------------------------

    this->SceneQuad = MakeShared<HSceneQuad>();

    // ---------------------- SCENE ----------------------------

    this->ColorSceneTextureSettings.Format = ERenderTarget2DMSFormat::RGBA16F;
    this->ColorSceneTextureSettings.Width = this->Width;
    this->ColorSceneTextureSettings.Height = this->Height;
    this->ColorSceneTextureSettings.Samples = this->MSAASamples;
    this->ColorSceneTextureSettings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->ColorSceneTexture = MakeShared<HRenderTarget2DMS>();
    this->ColorSceneTexture->Load(this->ColorSceneTextureSettings);

    this->BrightSceneTextureSettings.Format = ERenderTarget2DMSFormat::RGB16F;
    this->BrightSceneTextureSettings.Width = this->Width;
    this->BrightSceneTextureSettings.Height = this->Height;
    this->BrightSceneTextureSettings.Samples = this->MSAASamples;
    this->BrightSceneTextureSettings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BrightSceneTexture = MakeShared<HRenderTarget2DMS>();
    this->BrightSceneTexture->Load(this->BrightSceneTextureSettings);

    this->SceneFBO = MakeShared<HFrameBuffer>();
    this->SceneFBO->SetColorAttachment(0, this->ColorSceneTexture);
    this->SceneFBO->SetColorAttachment(1, this->BrightSceneTexture);
    this->SceneFBO->SetName(TX("sceneFBO"));

    // ------------------------ BLOOM SHADERS --------------------------

    DPostProcessShaderSettings bloomShader_DownSample_Settings;
    bloomShader_DownSample_Settings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/PostProcess.vert"),
        SEngine::ContentPath + TX("Shaders/BloomDownsample.frag")
    };

    this->BloomShader_Downsample = MakeShared<HPostProcessShader>();
    this->BloomShader_Downsample->SetName(TX("BloomShader_Downsample"));
    this->BloomShader_Downsample->Load(bloomShader_DownSample_Settings);

    DPostProcessMaterialSettings bloomShader_Downsample_Settings;
    bloomShader_Downsample_Settings.Shader = this->BloomShader_Downsample;

    this->BloomMaterial_Downsample = MakeShared<HPostProcessMaterial>();
    this->BloomMaterial_Downsample->SetName(TX("BloomMaterial_Downsample"));
    this->BloomMaterial_Downsample->Load(bloomShader_Downsample_Settings);
    this->BloomMaterial_Downsample->SetTextureParam(TX("HDRBrightBuffer"), this->BrightSceneTexture);
    this->BloomMaterial_Downsample->SetFloatParam(TX("Samples"), (float)this->MSAASamples);

    DPostProcessShaderSettings bloomShader_Blur_Settings;
    bloomShader_Blur_Settings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/PostProcess.vert"),
        SEngine::ContentPath + TX("Shaders/BloomBlur.frag")
    };

    this->BloomShader_Blur = MakeShared<HPostProcessShader>();
    this->BloomShader_Blur->SetName(TX("BloomShader_Blur"));
    this->BloomShader_Blur->Load(bloomShader_Blur_Settings);

    DPostProcessMaterialSettings bloomMaterial_Blur_Settings;
    bloomMaterial_Blur_Settings.Shader = this->BloomShader_Blur;

    this->BloomMaterial_Blur = MakeShared<HPostProcessMaterial>();
    this->BloomMaterial_Blur->SetName(TX("BloomMaterial_Blur"));
    this->BloomMaterial_Blur->Load(bloomMaterial_Blur_Settings);

    // -------------------------- BLOOM 0 ----------------------------

    this->BloomTexture_0_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_0_Settings.Width = this->Width;
    this->BloomTexture_0_Settings.Height = this->Height;
    this->BloomTexture_0_Settings.Anisotropy = false;
    this->BloomTexture_0_Settings.GenerateMipMaps = false;
    this->BloomTexture_0_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_0 = MakeShared<HRenderTarget2D>();
    this->BloomTexture_0->Load(this->BloomTexture_0_Settings);

    this->BloomFBO_0 = MakeShared<HFrameBuffer2D>();
    this->BloomFBO_0->SetName(TX("bloomFBO_0"));
    this->BloomFBO_0->SetColorAttachment(0, this->BloomTexture_0);

    // -------------------------- BLOOM 1 ----------------------------

    this->BloomTexture_1_A_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_1_A_Settings.Width = this->Width / this->BloomDownsample_1;
    this->BloomTexture_1_A_Settings.Height = this->Height / this->BloomDownsample_1;
    this->BloomTexture_1_A_Settings.Anisotropy = false;
    this->BloomTexture_1_A_Settings.GenerateMipMaps = false;
    this->BloomTexture_1_A_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_1_A = MakeShared<HRenderTarget2D>();
    this->BloomTexture_1_A->Load(this->BloomTexture_1_A_Settings);

    this->BloomTexture_1_B_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_1_B_Settings.Width = this->Width / this->BloomDownsample_1;
    this->BloomTexture_1_B_Settings.Height = this->Height / this->BloomDownsample_1;
    this->BloomTexture_1_B_Settings.Anisotropy = false;
    this->BloomTexture_1_B_Settings.GenerateMipMaps = false;
    this->BloomTexture_1_B_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_1_B = MakeShared<HRenderTarget2D>();
    this->BloomTexture_1_B->Load(this->BloomTexture_1_B_Settings);

    this->BloomFBO_1 = MakeShared<HFrameBuffer2D>();
    this->BloomFBO_1->SetName(TX("bloomFBO_1"));

    // -------------------------- BLOOM 2 ----------------------------

    this->BloomTexture_2_A_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_2_A_Settings.Width = this->Width / this->BloomDownsample_2;
    this->BloomTexture_2_A_Settings.Height = this->Height / this->BloomDownsample_2;
    this->BloomTexture_2_A_Settings.Anisotropy = false;
    this->BloomTexture_2_A_Settings.GenerateMipMaps = false;
    this->BloomTexture_2_A_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_2_A = MakeShared<HRenderTarget2D>();
    this->BloomTexture_2_A->Load(this->BloomTexture_2_A_Settings);

    this->BloomTexture_2_B_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_2_B_Settings.Width = this->Width / this->BloomDownsample_2;
    this->BloomTexture_2_B_Settings.Height = this->Height / this->BloomDownsample_2;
    this->BloomTexture_2_B_Settings.Anisotropy = false;
    this->BloomTexture_2_B_Settings.GenerateMipMaps = false;
    this->BloomTexture_2_B_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_2_B = MakeShared<HRenderTarget2D>();
    this->BloomTexture_2_B->Load(this->BloomTexture_2_B_Settings);

    this->BloomFBO_2 = MakeShared<HFrameBuffer2D>();
    BloomFBO_2->SetName(TX("bloomFBO_2"));

    // -------------------------- BLOOM 3 ----------------------------

    this->BloomTexture_3_A_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_3_A_Settings.Width = this->Width / this->BloomDownsample_3;
    this->BloomTexture_3_A_Settings.Height = this->Height / this->BloomDownsample_3;
    this->BloomTexture_3_A_Settings.Anisotropy = false;
    this->BloomTexture_3_A_Settings.GenerateMipMaps = false;
    this->BloomTexture_3_A_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_3_A = MakeShared<HRenderTarget2D>();
    this->BloomTexture_3_A->Load(this->BloomTexture_3_A_Settings);

    this->BloomTexture_3_B_Settings.Format = ERenderTarget2DFormat::RGBA16F;
    this->BloomTexture_3_B_Settings.Width = this->Width / this->BloomDownsample_3;
    this->BloomTexture_3_B_Settings.Height = this->Height / this->BloomDownsample_3;
    this->BloomTexture_3_B_Settings.Anisotropy = false;
    this->BloomTexture_3_B_Settings.GenerateMipMaps = false;
    this->BloomTexture_3_B_Settings.Wrap = ETextureWrap::CLAMP_TO_EDGE;
    this->BloomTexture_3_B = MakeShared<HRenderTarget2D>();
    this->BloomTexture_3_B->Load(BloomTexture_3_B_Settings);

    this->BloomFBO_3 = MakeShared<HFrameBuffer2D>();
    this->BloomFBO_3->SetName(TX("bloomFBO_3"));

    // --------------------- STENCIL BLIT BUFFER ------------------------

    this->StencilBlitBuffer = MakeShared<HFrameBuffer>();
    this->StencilBlitBuffer->SetName(TX("stencilBlitFBO"));

    // -------------------------- TONEMAPPER ----------------------------

    DPostProcessShaderSettings tonemapperShaderSettings;
    tonemapperShaderSettings.SourceFilepaths = {
        SEngine::ContentPath + TX("Shaders/PostProcess.vert"),
        SEngine::ContentPath + TX("Shaders/Tonemapper.frag")
    };

    this->TonemapperShader = MakeShared<HPostProcessShader>();
    this->TonemapperShader->SetName(TX("TonemapperShader"));
    this->TonemapperShader->Load(tonemapperShaderSettings);

    DPostProcessMaterialSettings tonemapperMaterialSettings;
    tonemapperMaterialSettings.Shader = this->TonemapperShader;

    this->TonemapperMaterial = MakeShared<HPostProcessMaterial>();
    this->TonemapperMaterial->SetName(TX("TonemapperMaterial"));
    this->TonemapperMaterial->Load(tonemapperMaterialSettings);
    this->TonemapperMaterial->SetTextureParam(TX("HDRBuffer"), this->ColorSceneTexture);
    this->TonemapperMaterial->SetTextureParam(TX("BloomBuffer_1"), this->BloomTexture_1_B);
    this->TonemapperMaterial->SetTextureParam(TX("BloomBuffer_2"), this->BloomTexture_2_B);
    this->TonemapperMaterial->SetTextureParam(TX("BloomBuffer_3"), this->BloomTexture_3_B);
    this->TonemapperMaterial->SetFloatParam(TX("Samples"), (float)this->MSAASamples);
    this->TonemapperMaterial->SetFloatParam(TX("BloomIntensity1"), .5f);
    this->TonemapperMaterial->SetFloatParam(TX("BloomIntensity2"), .5f);
    this->TonemapperMaterial->SetFloatParam(TX("BloomIntensity3"), .5f);
    
    this->MSOnPrevFrame = SClock::GetEpochMS();
    
    this->OnStart.Call();
}


void HRenderer::RenderFrame()
{
    /*______________________________________________________________

        SCENE MODIFICATION
    ______________________________________________________________*/

    int64 currentTime = SClock::GetEpochMS();
    this->DeltaMS = currentTime - this->MSOnPrevFrame;
    this->MSOnPrevFrame = currentTime;

    if (this->NextScene)
    {
        this->CurrentScene = this->NextScene;
        this->NextScene = nullptr;
        this->CurrentScene->Load();
    }
    else if (this->ToBeShutdown)
    {
        this->RenderStop();
    }

    if ((this->CurrentScene && this->CurrentScene->GetActiveCamera()) == false)
    {
        return;
    }

    this->OnEveryFrame.Call();
    this->OnNextFrame.Call();
    this->OnNextFrame.UnbindAllUnsafe();
    this->CurrentScene->GetTimer()->Update();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    /*______________________________________________________________

        RESIZING
    ______________________________________________________________*/

    if (this->ShouldResize)
    {
        this->ShouldResize = false;
        
        this->Width = this->NextWidth;
        this->Height = this->NextHeight;
        glCheck(glViewport(0, 0, this->Width, this->Height));

        this->ColorSceneTextureSettings.Width = this->Width;
        this->ColorSceneTextureSettings.Height = this->Height;
        this->ColorSceneTextureSettings.Samples = this->MSAASamples;
        this->ColorSceneTexture->Unload();
        this->ColorSceneTexture->Load(this->ColorSceneTextureSettings);

        this->BrightSceneTextureSettings.Width = this->Width;
        this->BrightSceneTextureSettings.Height = this->Height;
        this->BrightSceneTextureSettings.Samples = this->MSAASamples;
        this->BrightSceneTexture->Unload();
        this->BrightSceneTexture->Load(this->BrightSceneTextureSettings);

        if (this->EnableBloom)
        {
            int32 bloomWidth_1 = this->Width / this->BloomDownsample_1;
            int32 bloomHeight_1 = this->Height / this->BloomDownsample_1;

            int32 bloomWidth_2 = this->Width / this->BloomDownsample_2;
            int32 bloomHeight_2 = this->Height / this->BloomDownsample_2;

            int32 bloomWidth_3 = this->Width / this->BloomDownsample_3;
            int32 bloomHeight_3 = this->Height / this->BloomDownsample_3;

            this->BloomTexture_0_Settings.Width = bloomWidth_1;
            this->BloomTexture_0_Settings.Height = bloomHeight_1;
            this->BloomTexture_0->Unload();
            this->BloomTexture_0->Load(this->BloomTexture_0_Settings);

            this->BloomTexture_1_A_Settings.Width = bloomWidth_1;
            this->BloomTexture_1_A_Settings.Height = bloomHeight_1;
            this->BloomTexture_1_A->Unload();
            this->BloomTexture_1_A->Load(this->BloomTexture_1_A_Settings);

            this->BloomTexture_1_B_Settings.Width = bloomWidth_1;
            this->BloomTexture_1_B_Settings.Height = bloomHeight_1;
            this->BloomTexture_1_B->Unload();
            this->BloomTexture_1_B->Load(this->BloomTexture_1_B_Settings);

            this->BloomTexture_2_A_Settings.Width = bloomWidth_2;
            this->BloomTexture_2_A_Settings.Height = bloomHeight_2;
            this->BloomTexture_2_A->Unload();
            this->BloomTexture_2_A->Load(this->BloomTexture_2_A_Settings);

            this->BloomTexture_2_B_Settings.Width = bloomWidth_2;
            this->BloomTexture_2_B_Settings.Height = bloomHeight_2;
            this->BloomTexture_2_B->Unload();
            this->BloomTexture_2_B->Load(this->BloomTexture_2_B_Settings);

            this->BloomTexture_3_A_Settings.Width = bloomWidth_3;
            this->BloomTexture_3_A_Settings.Height = bloomHeight_3;
            this->BloomTexture_3_A->Unload();
            this->BloomTexture_3_A->Load(this->BloomTexture_3_A_Settings);

            this->BloomTexture_3_B_Settings.Width = bloomWidth_3;
            this->BloomTexture_3_B_Settings.Height = bloomHeight_3;
            this->BloomTexture_3_B->Unload();
            this->BloomTexture_3_B->Load(this->BloomTexture_3_B_Settings);

            this->BloomFBO_0->Prepare(bloomWidth_1, bloomHeight_1, 1);
            this->BloomFBO_1->Prepare(bloomWidth_1, bloomHeight_1, 1);
            this->BloomFBO_2->Prepare(bloomWidth_2, bloomHeight_2, 1);
            this->BloomFBO_3->Prepare(bloomWidth_3, bloomHeight_3, 1);
        }

        if (this->EnableStencil)
        {
            this->StencilBlitBuffer->Prepare(this->Width, this->Height, 0, 1, true);
        }

        this->SceneFBO->Prepare(this->Width, this->Height, 2, this->ColorSceneTexture->GetSamples(), this->EnableStencil);
        this->SceneFBO->AttachTextures();

        shared<HCameraNode> cam = this->CurrentScene->GetActiveCamera();
        cam->SetWidth(this->Width);
        cam->SetHeight(this->Height);
        
        this->OnResize.Call(this->Width, this->Height);
    }
    
    if (this->EnableStencil)
    {
        glCheck(glDisable(GL_STENCIL_TEST));
    }
    
    /*______________________________________________________________
    
        SHADOW PASS
    ______________________________________________________________*/

    // render shadows depths
    if (this->EnableShadows)
    {
        this->CurrentScene->RenderShadowDepths();
    }

    /*______________________________________________________________

        MAIN PASS
    ______________________________________________________________*/

    if (this->EnableStencil)
    {
        glCheck(glStencilMask(0xFF));
        glCheck(glEnable(GL_STENCIL_TEST));
        glCheck(glClearStencil(0));
        glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    }
    else
    {
        glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    // gather scene data
    HScene::DSceneRenderData sceneRenderData;
    this->CurrentScene->PopulateRenderData(sceneRenderData);

    // render scene with multisampling
    this->SceneFBO->Bind(true);
    this->CurrentScene->Draw(this->EnableStencil, sceneRenderData, this->EnableShadows);
    this->SceneFBO->BindNone();

    if (this->EnableStencil)
    {
        glCheck(glDisable(GL_STENCIL_TEST));
    }

    /*______________________________________________________________

        POST PROCESSING PASS
    ______________________________________________________________*/

    // -------------------------- BLOOM -----------------------------

    if (this->EnableBloom)
    {
        // initial bloom texture
        this->BloomFBO_0->SetColorAttachment(0, this->BloomTexture_0);
        this->BloomFBO_0->AttachTextures();
        this->BloomFBO_0->Bind();
        this->BloomShader_Downsample->Use();
        this->BloomMaterial_Downsample->ApplyStateOnShader();
        this->SceneQuad->Draw();
        this->BloomFBO_0->BindNone();

        // bloom 1
        for (int32 i = 0; i < this->BloomBlurIters_1; i++)
        {
            bool even = i % 2 == 0;
            // blur one bloom texture
            if (i != 0)
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), even? this->BloomTexture_1_A : this->BloomTexture_1_B);
            else
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), this->BloomTexture_0);
            this->BloomMaterial_Blur->SetIntParam(TX("Horizontal"), even);
            // export blured texture into other bloom texture
            this->BloomFBO_1->SetColorAttachment(0, even? this->BloomTexture_1_B : this->BloomTexture_1_A);
            this->BloomFBO_1->AttachTextures();
            // do blur
            this->BloomFBO_1->Bind();
            this->BloomShader_Blur->Use();
            this->BloomMaterial_Blur->ApplyStateOnShader();
            this->SceneQuad->Draw();
            this->BloomFBO_1->BindNone();
        }
        // bloom 2
        for (int32 i = 0; i < this->BloomBlurIters_2; i++)
        {
            bool even = i % 2 == 0;
            // blur one bloom texture
            if (i != 0)
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), even? this->BloomTexture_2_A : this->BloomTexture_2_B);
            else
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), this->BloomTexture_1_B);
            this->BloomMaterial_Blur->SetIntParam(TX("Horizontal"), even);
            // export blured texture into other bloom texture
            this->BloomFBO_2->SetColorAttachment(0, even? this->BloomTexture_2_B : this->BloomTexture_2_A);
            this->BloomFBO_2->AttachTextures();
            // do blur
            this->BloomFBO_2->Bind();
            this->BloomShader_Blur->Use();
            this->BloomMaterial_Blur->ApplyStateOnShader();
            this->SceneQuad->Draw();
            this->BloomFBO_2->BindNone();
        }
        // bloom 3
        for (int32 i = 0; i < this->BloomBlurIters_3; i++)
        {
            bool even = i % 2 == 0;
            // blur one bloom texture
            if (i != 0)
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), even? this->BloomTexture_3_A : this->BloomTexture_3_B);
            else
                this->BloomMaterial_Blur->SetTextureParam(TX("HDRBrightBuffer"), this->BloomTexture_2_B);
            this->BloomMaterial_Blur->SetIntParam(TX("Horizontal"), even);
            // export blured texture into other bloom texture
            this->BloomFBO_3->SetColorAttachment(0, even? this->BloomTexture_3_B : this->BloomTexture_3_A);
            this->BloomFBO_3->AttachTextures();
            // do blur
            this->BloomFBO_3->Bind();
            this->BloomShader_Blur->Use();
            this->BloomMaterial_Blur->ApplyStateOnShader();
            this->SceneQuad->Draw();
            this->BloomFBO_3->BindNone();
        }
    }
    glViewport(0, 0, this->Width, this->Height);

    // ------------------------ TONEMAPPER ---------------------------

    this->TonemapperShader->Use();
    this->TonemapperMaterial->ApplyStateOnShader();
    this->SceneQuad->Draw();
}


void HRenderer::RenderStop()
{
    this->CurrentScene = nullptr;
    this->NextScene = nullptr;
    this->ColorSceneTexture = nullptr;
    this->BrightSceneTexture = nullptr;
    this->SceneFBO = nullptr;
    this->BloomShader_Downsample = nullptr;
    this->BloomMaterial_Downsample = nullptr;
    this->BloomShader_Blur = nullptr;
    this->BloomMaterial_Blur = nullptr;
    this->BloomTexture_0 = nullptr;
    this->BloomFBO_0 = nullptr;
    this->BloomTexture_1_A = nullptr;
    this->BloomTexture_1_B = nullptr;
    this->BloomFBO_1 = nullptr;
    this->BloomTexture_2_A = nullptr;
    this->BloomTexture_2_B = nullptr;
    this->BloomFBO_2 = nullptr;
    this->BloomTexture_3_A = nullptr;
    this->BloomTexture_3_B = nullptr;
    this->BloomFBO_3 = nullptr;
    this->StencilBlitBuffer = nullptr;
    this->TonemapperShader = nullptr;
    this->TonemapperMaterial = nullptr;

    this->OnStop.Call();
}


void HRenderer::GetSize(int32& OutWidth, int32& OutHeight)
{
    OutWidth = this->Width;
    OutHeight = this->Height;
}


void HRenderer::Resize(int32 NewWidth, int32 NewHeight)
{
    this->NextWidth = NewWidth;
    this->NextHeight = NewHeight;
    this->ShouldResize = true;
}


shared<HScene> HRenderer::GetCurrentScene()
{
    return this->CurrentScene;
}


shared<HScene> HRenderer::GetNextScene()
{
    return this->NextScene;
}


void HRenderer::SetNextScene(shared<HScene> InNextScene)
{
    this->NextScene = InNextScene;
}


int64 HRenderer::GetDeltaMS()
{
    return this->DeltaMS;
}


double HRenderer::GetDeltaSec()
{
    return this->DeltaMS / 1000.0;
}


uint8 HRenderer::GetStencilValueAt(int32 X, int32 Y)
{
    this->SceneFBO->BlitTo(this->StencilBlitBuffer, false, false, true, HFrameBuffer::EBlitFilter::NEAREST);
    this->StencilBlitBuffer->Bind(false);
    uint8 value = 0;
    glCheck(glReadPixels(X, this->Height - Y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &value));
    this->StencilBlitBuffer->BindNone();
    return value;
}


int32 HRenderer::GetMSAASamples() const
{
    return this->MSAASamples;
}
void HRenderer::SetMSAASamples(int32 Value)
{
    this->MSAASamples = FMath::Clamp(Value, 2, 8);
    this->BloomMaterial_Downsample->SetFloatParam(TX("Samples"), (float)this->MSAASamples);
    this->TonemapperMaterial->SetFloatParam(TX("Samples"), (float)this->MSAASamples);
    this->ShouldResize = true;
}

float HRenderer::GetAlphaToCoverage() const
{
    return this->AlphaToCoverage;
}
void HRenderer::SetAlphaToCoverage(float Value)
{
    this->AlphaToCoverage = FMath::Clamp(Value, .0f, 1.0f);
    glCheck(glSampleCoverage(this->AlphaToCoverage, false));
}

int32 HRenderer::GetBloomDownsample_1() const
{
    return this->BloomDownsample_1;
}
void HRenderer::SetBloomDownsample_1(int32 Value)
{
    this->BloomDownsample_1 = FMath::Clamp(Value, 1, 100);
    this->ShouldResize = true;
}

int32 HRenderer::GetBloomDownsample_2() const
{
    return this->BloomDownsample_2;
}
void HRenderer::SetBloomDownsample_2(int32 Value)
{
    this->BloomDownsample_2 = FMath::Clamp(Value, 1, 100);
    this->ShouldResize = true;
}

int32 HRenderer::GetBloomDownsample_3() const
{
    return this->BloomDownsample_3;
}
void HRenderer::SetBloomDownsample_3(int32 Value)
{
    this->BloomDownsample_3 = FMath::Clamp(Value, 1, 100);
    this->ShouldResize = true;
}

int32 HRenderer::GetBloomBlurIters_1() const
{
    return this->BloomBlurIters_1;
}
void HRenderer::SetBloomBlurIters_1(int32 Value)
{
    this->BloomBlurIters_1 = FMath::Clamp(Value, 1, 100);
}

int32 HRenderer::GetBloomBlurIters_2() const
{
    return this->BloomBlurIters_2;
}
void HRenderer::SetBloomBlurIters_2(int32 Value)
{
    this->BloomBlurIters_2 = FMath::Clamp(Value, 1, 100);
}

int32 HRenderer::GetBloomBlurIters_3() const
{
    return this->BloomBlurIters_3;
}
void HRenderer::SetBloomBlurIters_3(int32 Value)
{
    this->BloomBlurIters_3 = FMath::Clamp(Value, 1, 100);
}
