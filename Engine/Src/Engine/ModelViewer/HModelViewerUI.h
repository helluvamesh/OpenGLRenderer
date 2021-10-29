#pragma once

#include "Engine\Dependencies\IMGUI\ImGuiAll.h"
#include "Engine\UI\HWindow3D.h"
#include "Engine\ModelViewer\HModelViewer.h"
#include "Engine\Render\Texture\HTexture2D.h"


class HModelViewerUI : public HObject
{
public:

	enum class EDropTarget : uint8 { NONE, BASE_COLOR, REFLECTANCE, NORMAL, EMISSION, AMBIENT_HDR };

private:

	enum class EModelViewerUIContext : uint8 { SCENE, LIGHTING, CAMERA, RENDERER };

	enum class EModelViewerCameraMode : uint8 { WALK, TURN_TABLE };

	const ImVec2 UIPos;
	const ImVec2 UISize;
	const float LabelOffsetMul;
	const float VSpacingMul;
	const ImVec2 PaddingMul;
	const ImVec2 DropTargetSizeMul;
	const ImVec2 WideDropTargetSizeMul;
	const ImVec4 WindowBg;
	const ImVec4 LabelFg;
	const ImVec4 FrameBg;
	const ImVec4 FrameBgHover;
	const ImVec4 FrameBgActive;
	const ImVec4 HeaderBg;
	const ImVec4 HeaderBgHover;
	const ImVec4 HeaderBgActive;
	const ImVec4 DropTargetBorder;
	const ImVec4 DropTargetFg;
	const ImVec4 DropTargetBaseColorBg;
	const ImVec4 DropTargetReflectanceBg;
	const ImVec4 DropTargetNormalBg;
	const ImVec4 DropTargetEmissionBg;
	const ImVec4 DropTargetAmbientHDRBg;
	const ImVec4 ButtonBg;
	const ImVec4 ButtonBgHover;
	const ImVec4 ButtonBgActive;
	const ImVec2 ButtonPaddingMul;
	const ImVec4 InteractiveTextFg;
	const ImVec4 MenuItemBgHover;
	const ImVec4 MenuItemBgActive;
	
	// Calculated
	float LabelOffset = .0f;
	float VSpacing = .0f;
	ImVec2 Padding;
	ImVec2 ButtonPadding;
	ImVec2 DropTargetSize;
	ImVec2 WideDropTargetSize;
	ImVec2 DropTargetLabelPadding;
	
	shared<HWindow3D> Window;
	shared<HRenderer> Renderer;
	shared<HModelViewer> Viewer;

	bool LeftMouseDown;
	bool RightMouseDown;
	bool UpKeyDown;
	bool DownKeyDown;
	bool LeftKeyDown;
	bool RightKeyDown;
	bool IsCtrlDown;
	bool LeftDragDetected;

	DArray<DString8> MaterialIndexTexts;
	const DArray<DString8> MaterialBlendModeTexts;
	const DArray<DString8> AxisTexts;
	const DArray<DString8> ShadowResolutionTexts;
	const DArray<DString8> MSAATexts;

	float CameraStridePerSec;
	EModelViewerCameraMode CameraMode;
	EModelViewerUIContext UIContext;
	int32 UIWidth;
	EDropTarget HoveredDropTarget;
	int32 MaterialIndex;
	float SunXRotDeg;
	float SunYRotDeg;
	float AmbientAngle;
	EAxis SwizzleRight;
	EAxis SwizzleUp;
	EAxis SwizzleBack;
	float ScaleToMeter;
	bool DirectXNormalmap;

public:

	HModelViewerUI(
		shared<HWindow3D> InWindow, shared<HRenderer> InRenderer, shared<HModelViewer> InViewer, 
		const DModelViewerConfig& Config
	);
	void Init();

private:

	void Draw();
	void DrawTextureDropTarget(
		shared<HSurfaceMaterial> Material,
		const DString& TextureParamName, EDropTarget DropTargetType, HModelViewer::EModelViewerTexture TextureType,
		const char* Id, const char* Label, const ImVec4& BackgrondColor
	);
	void DrawFloatMaterialParam(
		shared<HSurfaceMaterial> Material, const DString& MaterialParamName, const char* Id, const char* Label,
		float Min = .0f, float Max = 1.0f, float Step = .01f
	);
	void DrawAxisSelector(const char* Id, EAxis& Var);
	void AddVSpacing(float Multiplier = 1.0f);

	void OnKeyEvent(DKeyEvent Event);
	void OnMouseButton(DMouseButtonEvent Event);
	void OnMouseMotion(DMouseMotionEvent Event);
	void OnEveryFrame();
	void OnFileDrop(const DArray<DString>& Filepaths);
	void OnScroll(DScrollEvent Event);
	
	float GetCameraStride() const;
	void SetCameraStride(float Value);

};
