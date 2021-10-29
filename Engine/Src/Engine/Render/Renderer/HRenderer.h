#pragma once

#include "Engine/Render/Scene/HScene.h"
#include "Engine/UI/HWindow3D.h"
#include "Engine/Render/Shader/HPostProcessMaterial.h"
#include "Engine/Render/Texture/HFrameBuffer.h"
#include "Engine/Render/Texture/HFrameBuffer2D.h"
#include "Engine/Render/Mesh/HSceneQuad.h"
#include "Engine/Render/Texture/HRenderTarget2DMS.h"
#include "Engine/Render/Texture/HRenderTarget2D.h"


using DJobDelegate = DMultiCastDelegate<>;
using DResizeDelegate = DMultiCastDelegate<int32, int32>;


class HRenderer : public HObject
{
private:

	static const int32 MinOpenGLMajor = 3;
	static const int32 MinOpenGLMinor = 3;

	int32 Width = 512;
	int32 Height = 512;
	int32 MSAASamples = 4;
	float AlphaToCoverage = .5f;

	int32 NextWidth = 512;
	int32 NextHeight = 512;
	bool ShouldResize = true;

	bool EnableBloom = true;
	int32 BloomDownsample_1 = 4;
	int32 BloomDownsample_2 = 8;
	int32 BloomDownsample_3 = 16;
	int32 BloomBlurIters_1 = 16;
	int32 BloomBlurIters_2 = 16;
	int32 BloomBlurIters_3 = 16;
	
	bool EnableStencil = true;

	int64 MSOnPrevFrame = 0L;
	int64 DeltaMS = 0L;

	shared<HScene> CurrentScene;
	shared<HScene> NextScene;
	bool ToBeShutdown = false;

    shared<HSceneQuad> SceneQuad;

    shared<HRenderTarget2DMS> ColorSceneTexture;
	DRenderTarget2DMSSettings ColorSceneTextureSettings;
    shared<HRenderTarget2DMS> BrightSceneTexture;
	DRenderTarget2DMSSettings BrightSceneTextureSettings;
    shared<HFrameBuffer> SceneFBO;

    shared<HPostProcessShader> BloomShader_Downsample;
    shared<HPostProcessMaterial> BloomMaterial_Downsample;
    shared<HPostProcessShader> BloomShader_Blur;
    shared<HPostProcessMaterial> BloomMaterial_Blur;

    shared<HRenderTarget2D> BloomTexture_0;
	DRenderTarget2DSettings BloomTexture_0_Settings;
    shared<HFrameBuffer2D> BloomFBO_0;

    shared<HRenderTarget2D> BloomTexture_1_A;
	DRenderTarget2DSettings BloomTexture_1_A_Settings;
    shared<HRenderTarget2D> BloomTexture_1_B;
	DRenderTarget2DSettings BloomTexture_1_B_Settings;
    shared<HFrameBuffer2D> BloomFBO_1;

    shared<HRenderTarget2D> BloomTexture_2_A;
	DRenderTarget2DSettings BloomTexture_2_A_Settings;
    shared<HRenderTarget2D> BloomTexture_2_B;
	DRenderTarget2DSettings BloomTexture_2_B_Settings;
    shared<HFrameBuffer2D> BloomFBO_2;
    
    shared<HRenderTarget2D> BloomTexture_3_A;
	DRenderTarget2DSettings BloomTexture_3_A_Settings;
    shared<HRenderTarget2D> BloomTexture_3_B;
	DRenderTarget2DSettings BloomTexture_3_B_Settings;
    shared<HFrameBuffer2D> BloomFBO_3;
	
	shared<HFrameBuffer> StencilBlitBuffer;

    shared<HPostProcessShader> TonemapperShader;
	shared<HPostProcessMaterial> TonemapperMaterial;
	
	bool EnableShadows = true;

public:
	
	DJobDelegate OnStart;
	DJobDelegate OnNextFrame;
	DJobDelegate OnEveryFrame;
	DJobDelegate OnStop;
	DResizeDelegate OnResize;

	static int32 CompareOpenGLVersions(int32 MajorA, int32 MinorA, int32 MajorB, int32 MinorB);
	static void GetMinOpenGLVersion(int32& OutMajor, int32& OutMinor);

	void RenderStart();
	void RenderFrame();
	void RenderStop();
	
	void GetSize(int32& OutWidth, int32& OutHeight);
	void Resize(int32 NewWidth, int32 NewHeight);

	shared<HScene> GetCurrentScene();
	shared<HScene> GetNextScene();
	void SetNextScene(shared<HScene> InNextScene);

	int64 GetDeltaMS();
	double GetDeltaSec();

	uint8 GetStencilValueAt(int32 X, int32 Y);

	int32 GetMSAASamples() const;
	void SetMSAASamples(int32 Value);
	
	float GetAlphaToCoverage() const;
	void SetAlphaToCoverage(float Value);

	int32 GetBloomDownsample_1() const;
	void SetBloomDownsample_1(int32 Value);

	int32 GetBloomDownsample_2() const;
	void SetBloomDownsample_2(int32 Value);
	
	int32 GetBloomDownsample_3() const;
	void SetBloomDownsample_3(int32 Value);
	
	int32 GetBloomBlurIters_1() const;
	void SetBloomBlurIters_1(int32 Value);
	
	int32 GetBloomBlurIters_2() const;
	void SetBloomBlurIters_2(int32 Value);
	
	int32 GetBloomBlurIters_3() const;
	void SetBloomBlurIters_3(int32 Value);

};