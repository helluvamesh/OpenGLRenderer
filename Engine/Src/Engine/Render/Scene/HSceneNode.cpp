#include "Engine/pch.h"
#include "HSceneNode.h"
#include "HScene.h"


const FVec3 HSceneNode::UP = FVec3(.0f, 1.0f, .0f);
const FVec3 HSceneNode::FORWARD = FVec3(.0f, .0f, -1.0f);
const FVec3 HSceneNode::RIGHT = FVec3(1.0f, .0f, .0f);


HSceneNode::HSceneNode(ESceneNodeType NewType) : 
	Type(NewType)
{
}


HSceneNode::HSceneNode(ESceneNodeType NewType, weak<HScene> InScene) : 
	Type(NewType),
	Scene(InScene)
{
}


HSceneNode::HSceneNode() : Type(ESceneNodeType::NONE)
{
}




HSceneNode::~HSceneNode()
{
}


const DSceneNodeRenderData& HSceneNode::GetRenderData() const
{
	return this->RenderData;
}


shared<HSceneNode> HSceneNode::GetParent() const
{
	return this->Parent;
}


shared<HScene> HSceneNode::GetScene() const
{
	return this->Scene;
}


bool HSceneNode::IsVisible() const
{
	return this->Visible;
}
void HSceneNode::SetVisible(bool Value)
{
	this->Visible = Value;
	if (this->PropagateVisibility)
	{
		for (shared<HSceneNode>& child : this->Children)
		{
			child->SetVisible(Value);
		}
	}
}
bool HSceneNode::GetPropagateVisibility() const
{
	return this->PropagateVisibility;
}
void HSceneNode::SetPropagateVisibility(bool Value)
{
	this->PropagateVisibility = Value;
}
bool HSceneNode::GetInheritScale() const
{
	return this->InheritScale;
}
void HSceneNode::SetInheritScale(bool Value)
{
	this->InheritScale = Value;
}


void HSceneNode::UpdateWorldTransformAndMatrix()
{
	shared<HSceneNode> parent = this->Parent;
	if (parent)
	{
		this->WorldLocation = parent->RenderData.WorldMatrix.TransformPos(this->Location);
		this->WorldRotation = parent->WorldRotation * this->Rotation;  // order matters
		if (this->InheritScale)
		{
			this->WorldScaling = this->Scaling * parent->WorldScaling;
		}
		else
		{
			this->WorldScaling = this->Scaling;
		}
	}
	else
	{
		this->WorldLocation = this->Location;
		this->WorldRotation = this->Rotation;
		this->WorldScaling = this->Scaling;
	}
	this->RenderData.WorldMatrix = FMat4::Translation(this->WorldLocation);
	this->RenderData.WorldMatrix *= FMat4::Rotation(this->WorldRotation);
	this->RenderData.WorldMatrix *= FMat4::Scaling(this->WorldScaling);

	int32 len = this->Children.Length();
	for (int32 i = 0; i < len; i++)
	{
		this->Children[i]->UpdateWorldTransformAndMatrix();
	}

	this->OnTransform();
}


void HSceneNode::OnTransform() {  }


void HSceneNode::SetLocation(const FVec3& Loc)
{
	this->Location = Loc;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetRotation(const FQuat& Rot)
{
	this->Rotation = Rot;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetScale(const FVec3 NewScale)
{
	this->Scaling = NewScale;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetLocationAndRotation(const FVec3& Loc, const FQuat& Rot)
{
	this->Location = Loc;
	this->Rotation = Rot;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetTransform(const FVec3& Loc, const FQuat& Rot, const FVec3& NewScale)
{
	this->Location = Loc;
	this->Rotation = Rot;
	this->Scaling = NewScale;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::Translate(const FVec3& Vec)
{
	this->Location += Vec;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::Rotate(const FQuat& Rot)
{
	this->Rotation *= Rot;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::Scale(const FVec3& Vec)
{
	this->Scaling *= Vec;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::Scale(float Scalar)
{
	this->Scaling *= Scalar;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::TranslateAndRotate(const FVec3& Vec, const FQuat& Rot)
{
	this->Location += Vec;
	this->Rotation *= Rot;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::Transform(const FVec3& TVec, const FQuat& Rot, const FVec3& SVec)
{
	this->Location += TVec;
	this->Rotation *= Rot;
	this->Scaling *= SVec;
	this->UpdateWorldTransformAndMatrix();
}


const FVec3& HSceneNode::GetLocation() const { return this->Location; }
const FQuat& HSceneNode::GetRotation() const { return this->Rotation; }
const FVec3& HSceneNode::GetScale() const { return this->Scaling; }


void HSceneNode::_SetWorldLocation(const FVec3& Loc)
{
	shared<HSceneNode> parent = this->Parent;
	if (parent)
	{
		FMat4 invParentMat;
		parent->RenderData.WorldMatrix.TryInvert(invParentMat);
		this->Location = invParentMat.TransformPos(Loc);
	}
	else
	{
		FMat4 invMat;
		parent->RenderData.WorldMatrix.TryInvert(invMat);
		this->Location = invMat.TransformPos(Loc);
	}
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::_SetWorldRotation(const FQuat& Rot)
{
	// rotate to default position then apply rotation
	shared<HSceneNode> parent = this->Parent;
	if (parent)
	{
		this->Rotation = parent->WorldRotation.Inverse();
		this->Rotation *= Rot;
	}
	else
	{
		this->Rotation = this->WorldRotation.Inverse();
		this->Rotation *= Rot;
	}
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::_SetWorldScale(const FVec3 NewScale)
{
	shared<HSceneNode> parent = this->Parent;
	if (parent && this->InheritScale)
	{
		this->Scaling = FVec3(
			parent->WorldScaling.X != .0f? NewScale.X / parent->WorldScaling.X : .0f,
			parent->WorldScaling.Y != .0f? NewScale.Y / parent->WorldScaling.Y : .0f,
			parent->WorldScaling.Z != .0f? NewScale.Z / parent->WorldScaling.Z : .0f
		);
	}
	else
	{
		this->Scaling = NewScale;
	}
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::_WorldTranslate(const FVec3& Vec)
{
	shared<HSceneNode> parent = this->Parent;
	if (parent)
	{
		FMat4 invParentMat;
		parent->RenderData.WorldMatrix.TryInvert(invParentMat);
		this->Location = this->WorldLocation + Vec;
		this->Location = invParentMat.TransformPos(this->Location);
	}
	else
	{
		FMat4 invMat;
		this->RenderData.WorldMatrix.TryInvert(invMat);
		this->Location = this->WorldLocation + Vec;
		this->Location = invMat.TransformPos(this->Location);
	}
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::_WorldRotate(const FQuat& Rot)
{
	// rotate to default position, apply rotation, re-apply previous rotation 
	FQuat q = this->WorldRotation.Inverse();
	q *= Rot;
	q *= this->WorldRotation;
	this->Rotation *= q;
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetWorldLocation(const FVec3& Loc)
{
	this->_SetWorldLocation(Loc);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetWorldRotation(const FQuat& Rot)
{
	this->_SetWorldRotation(Rot);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetWorldScale(const FVec3 NewScale)
{
	this->_SetWorldScale(NewScale);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetWorldLocationAndRotation(const FVec3& Loc, const FQuat& Rot)
{
	this->_SetWorldLocation(Loc);
	this->_SetWorldRotation(Rot);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::SetWorldTransform(const FVec3& Loc, const FQuat& Rot, const FVec3& NewScale)
{
	this->_SetWorldLocation(Loc);
	this->_SetWorldRotation(Rot);
	this->_SetWorldScale(NewScale);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::WorldTranslate(const FVec3& Vec)
{
	this->_WorldTranslate(Vec);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::WorldRotate(const FQuat& Rot)
{
	this->_WorldRotate(Rot);
	this->UpdateWorldTransformAndMatrix();
}


void HSceneNode::WorldTranslateAndRotate(const FVec3& Vec, const FQuat& Rot)
{
	this->_WorldTranslate(Vec);
	this->_WorldRotate(Rot);
	this->UpdateWorldTransformAndMatrix();
}

const FVec3& HSceneNode::GetWorldLocation() const { return this->WorldLocation; }
const FQuat& HSceneNode::GetWorldRotation() const { return this->WorldRotation; }
const FVec3& HSceneNode::GetWorldScale() const { return this->WorldScaling; }


const FMat4& HSceneNode::GetWorldMatrix() const
{
	return this->RenderData.WorldMatrix;
}


FVec3 HSceneNode::CalcForward() const
{
	return FMat4::Rotation(this->WorldRotation).TransformDir(FORWARD);
}


FVec3 HSceneNode::CalcUp() const
{
	return FMat4::Rotation(this->WorldRotation).TransformDir(UP);
}


FVec3 HSceneNode::CalcRight() const
{
	return FMat4::Rotation(this->WorldRotation).TransformDir(RIGHT);
}


void HSceneNode::AddChild(shared<HSceneNode> Child, bool KeepTransform)
{
	shared<HSceneNode> childParentSptr = Child->GetParent();
	shared<HSceneNode> thisParent = this->GetParent();
	HSceneNode* childParent = childParentSptr.Ptr();
	if (Child.Ptr() != this && childParent != this && thisParent != Child)
	{
		if (childParent)
		{
			int32 i = childParent->Children.IndexOf(Child);
			if (i > -1)
			{
				childParent->Children.Remove(i);
			}
		}
		Child->Parent = this->WeakThisAs<HSceneNode>();
		this->Children.Add(Child);
		
		bool isNewScene = this->Scene && Child->Scene != this->Scene;
		Child->Scene = this->Scene;

		if (KeepTransform)
		{
			FVec3 oldLoc = Child->WorldLocation;
			FQuat oldRot = Child->WorldRotation;
			FVec3 oldScale = Child->WorldScaling;

			Child->UpdateWorldTransformAndMatrix();

			Child->SetWorldTransform(oldLoc, oldRot, oldScale);
		}
		else
		{
			Child->UpdateWorldTransformAndMatrix();
		}
		if (isNewScene)
		{
			Child->OnEnteringScene();
		}
	}
}


void HSceneNode::RemoveChild(shared<HSceneNode> Child, bool UpdateTransform)
{
	int32 i = this->Children.IndexOf(Child);
	if (i > -1)
	{
		Child->OnLeavingScene();
		Child->Parent = nullptr;
		if (UpdateTransform)
		{
			Child->UpdateWorldTransformAndMatrix();
		}
		this->Children.Remove(i);
		Child->Scene = nullptr;
	}
}


void HSceneNode::RemoveChild(int32 Index, bool UpdateTransform)
{
	if (this->Children.IsValidIndex(Index))
	{
		shared<HSceneNode>& child = this->Children[Index];
		child->OnLeavingScene();
		child->Parent = nullptr;
		if (UpdateTransform)
		{
			child->UpdateWorldTransformAndMatrix();
		}
		this->Children.Remove(Index);
		child->Scene = nullptr;
	}
}


void HSceneNode::ClearChildren(bool UpdateTransform)
{
	if (UpdateTransform)
	{
		for (int32 i = 0; i < this->Children.Length(); i++)
		{
			shared<HSceneNode>& child = this->Children[i];
			child->OnLeavingScene();
			child->Parent = nullptr;
			child->UpdateWorldTransformAndMatrix();
			child->Scene = nullptr;
		}
	}
	else
	{
		for (int32 i = 0; i < this->Children.Length(); i++)
		{
			shared<HSceneNode>& child = this->Children[i];
			child->OnLeavingScene();
			child->Parent = nullptr;
			child->Scene = nullptr;
		}
	}
	this->Children.Clear();
}


void HSceneNode::MoveChild(int32 Index, int32 NewIndex)
{
	Index = FMath::Clamp(Index, 0, this->Children.Length());
	NewIndex = FMath::Clamp(Index, 0, this->Children.Length());
	if (Index != NewIndex)
	{
		std::swap(this->Children[Index], this->Children[NewIndex]);
	}
}


const DArray<shared<HSceneNode>>& HSceneNode::GetChildren() const
{
	return this->Children;
}


const DString& HSceneNode::GetName() const
{
	return this->Name;
}
void HSceneNode::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


void HSceneNode::OnEnteringScene() { }
void HSceneNode::OnLeavingScene() { }
