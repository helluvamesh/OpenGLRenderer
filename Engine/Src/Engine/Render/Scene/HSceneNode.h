#pragma once

#include "DSceneNodeRenderData.h"


class HScene;


enum class ESceneNodeType : uint8 { NONE, STATIC_MESH, CAMERA, LIGHT, LINE_WORK };


class HSceneNode : public HObject
{
	friend class HScene;

public:
	static const FVec3 FORWARD;
	static const FVec3 UP;
	static const FVec3 RIGHT;

public:
	const ESceneNodeType Type;

private:
	FVec3 Location;
	FQuat Rotation;
	FVec3 Scaling = FVec3(1.0f);

	FVec3 WorldLocation;
	FQuat WorldRotation;
	FVec3 WorldScaling = FVec3(1.0f);
	
	DSceneNodeRenderData RenderData;
	DArray<shared<HSceneNode>> Children;
	weak<HSceneNode> Parent;
	weak<HScene> Scene;

protected:
	DString Name;

private:
	bool Visible = true;
	bool PropagateVisibility = true;
	bool InheritScale = true;

public:
	HSceneNode(ESceneNodeType NewType);
	HSceneNode(ESceneNodeType NewType, weak<HScene> InScene);
	HSceneNode();
	~HSceneNode();

	const DSceneNodeRenderData& GetRenderData() const;
	shared<HSceneNode> GetParent() const;
	shared<HScene> GetScene() const;

public:
	bool IsVisible() const;
	void SetVisible(bool Value);
	bool GetPropagateVisibility() const;
	void SetPropagateVisibility(bool Value);
	bool GetInheritScale() const;
	void SetInheritScale(bool Value);

private:
	void UpdateWorldTransformAndMatrix();

protected:
	virtual void OnTransform();
	
public:
	
	// TRANSFORM IN NODE SPACE:

	void SetLocation(const FVec3& Loc);
	void SetRotation(const FQuat& Rot);
	void SetScale(const FVec3 NewScale);
	void SetLocationAndRotation(const FVec3& Loc, const FQuat& Rot);
	void SetTransform(const FVec3& Loc, const FQuat& Rot, const FVec3& NewScale);
	void Translate(const FVec3& Vec);
	void Rotate(const FQuat& Rot);
	void Scale(const FVec3& Vec);
	void Scale(float Scalar);
	void TranslateAndRotate(const FVec3& Vec, const FQuat& Rot);
	void Transform(const FVec3& TVec, const FQuat& Rot, const FVec3& SVec);
	const FVec3& GetLocation() const;
	const FQuat& GetRotation() const;
	const FVec3& GetScale() const;
	
	// TRANSFORM IN WORLD SPACE:
	// (apply node-space transform that looks like world-space transform)

private:
	void _SetWorldLocation(const FVec3& Loc);
	void _SetWorldRotation(const FQuat& Rot);
	void _SetWorldScale(const FVec3 NewScale);
	void _WorldTranslate(const FVec3& Vec);
	void _WorldRotate(const FQuat& Rot);

public:
	void SetWorldLocation(const FVec3& Loc);
	void SetWorldRotation(const FQuat& Rot);
	void SetWorldScale(const FVec3 NewScale);
	void SetWorldLocationAndRotation(const FVec3& Loc, const FQuat& Rot);
	void SetWorldTransform(const FVec3& Loc, const FQuat& Rot, const FVec3& NewScale);
	void WorldTranslate(const FVec3& Vec);
	void WorldRotate(const FQuat& Rot);
	void WorldTranslateAndRotate(const FVec3& Vec, const FQuat& Rot);
	const FVec3& GetWorldLocation() const;
	const FQuat& GetWorldRotation() const;
	const FVec3& GetWorldScale() const;
	const FMat4& GetWorldMatrix() const;

	FVec3 CalcForward() const;
	FVec3 CalcUp() const;
	FVec3 CalcRight() const;

	void AddChild(shared<HSceneNode> Child, bool KeepTransform = false);
	/* UpdateTransform should be set true, if you want to use the node in the future */
	void RemoveChild(shared<HSceneNode> Child, bool UpdateTransform = false);
	/* UpdateTransform should be set true, if you want to use the node in the future */
	void RemoveChild(int32 Index, bool UpdateTransform = false);
	/* UpdateTransform should be set true, if you want to use the child nodes in the future */
	void ClearChildren(bool UpdateTransform = false);
	void MoveChild(int32 Index, int32 NewIndex);
	const DArray<shared<HSceneNode>>& GetChildren() const;
	
	const DString& GetName() const;
	void SetName(DStringView NewName);

protected:
	virtual void OnEnteringScene();
	virtual void OnLeavingScene();

};