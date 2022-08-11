#pragma once

#include "Engine/Render/Scene/HScene.h"
#include "Engine/Render/Shader/HSurfaceShader.h"
#include "Engine/Render/Scene/HSceneNode.h"
#include "Engine/UI/HWindow3D.h"
#include "Engine/Render/Texture/HTextureBase.h"
#include "Engine/Render/Texture/HRenderTargetCube.h"
#include "Engine/Render/Renderer/HRenderer.h"
#include "Engine/Render/Shader/HEqMapConvert.h"
#include "Engine/Render/Shader/HIBLConvert.h"
#include "Engine/Render/Scene/HLightNode.h"
#include "Engine\Render\Scene\HLineWorkNode.h"


struct DModelViewerConfig : public DStruct
{
	REFLECT_H(DModelViewerConfig, DStruct)
public:
	int32 MinOpenGLVersionMajor = 4;
	int32 MinOpenGLVersionMinor = 0;
	int32 WindowWidth = 800;
	int32 WindowHeight = 600;
	int32 UIWidth = 300;
};


struct DIBLEnvironment
{
	shared<HTextureCubeBase> IrradianceMap;
	shared<HTextureCubeBase> ReflectionMap;
};


class HModelViewer : public HObject
{
private:

	shared<HRenderer> Renderer;

	shared<HEqMapConvert> EqMapConverter;
	shared<HIBLConvert> IBLConverter;

	shared<HSurfaceShader> GeneralShader;
	shared<HDirectionalLightNode> Sun;
	shared<HAmbientLightNode> AmbientLight;
	shared<HTexture2D> AmbientSourceHDR;
	shared<HRenderTargetCube> AmbientCubemap;
	shared<HSurfaceShader> LineWorkShader;

	shared<HScene> CurrentScene;
	shared<HSceneNode> ModelRoot;
	shared<HSceneNode> CameraRoot;
	shared<HSceneNode> LightRoot;
	shared<HSceneNode> GizmoRoot;
	shared<HLineWorkNode> SelectionBracket;
	
	const DSet<DString> MeshExtensions;
	const DSet<DString> TextureExtensions;
	const DSet<DString> HDRExtensions;

	weak<HStaticMeshNode> CurrentMeshNode;
	
public:

	HModelViewer(shared<HRenderer> InRenderer);

public:

	enum class EModelViewerTexture : uint8 { BASE_COLOR, REFLECTANCE, NORMAL, EMISSION };
	enum class ECameraTranslateAxis : uint8 { X, Y, Z, NEG_X, NEG_Y, NEG_Z };

	shared<HScene> GetCurrentScene();

	shared<HStaticMeshNode> GetMeshNode();
	shared<HSurfaceMaterial> GetMaterial(int32 MaterialIndex);
	shared<HDirectionalLightNode> GetSun();
	shared<HAmbientLightNode> GetAmbientLight();
	shared<HCameraNode> GetCamera();
	shared<HTexture2D> GetAmbientSourceHDR();
	
	void LoadScene(const DString& MeshFilepath, EAxis Left, EAxis Up, EAxis Back, float ScaleToMeter, bool FlipNormalmapGreen);
	void UnloadScene();
	void LoadTexture(int32 MaterialIndex, const DString& TextureFilepath, EModelViewerTexture TextureType, bool FlipGreen = false);
	void UnloadTexture(int32 MaterialIndex, EModelViewerTexture TextureType);
	void LoadAmbientHDR(const DString& HDRFilepath, float UOffset);
	void GenerateIBLMaps(float UOffset, bool Log);
	void UnloadAmbientHDR();
	void ResetModelTransform();
	void ResetCameraTranslation();
	void ResetCameraRotation();
	void RotateModelHorizontally(float Value);
	void RotateModelVertically(float Value);
	// Dx, Dy in pixels
	void RotateCameraMouseDelta(float Dx, float Dy);
	// H, V in radians
	void RotateCamera(float H, float V);
	void TranslateModel(const FVec3& Vec);
	//void TranslateCamera(ECameraTranslateAxis Axis, float Value);
	void TranslateCameraForward(float Value);
	void TranslateCameraRight(float Value);
	void TranslateCameraUp(float Value);
	void SetSunRotation(float RotXDeg, float RotYDeg);
	void Select(int32 ScreenX, int32 ScreenY);
	void UpdateSelectionBracket();
	shared<HRenderer> GetRenderer() const;
};