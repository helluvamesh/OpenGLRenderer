#pragma once

#include "HSceneNode.h"
#include "HCameraNode.h"
#include "Engine/Render/Shader/HSurfaceShader.h"
#include "Engine/Render/Shader/HSurfaceMaterial.h"
#include "Engine/Asset/HAsset.h"
#include "HStaticMeshNode.h"
#include "HLightNode.h"
#include "HDirectionalLightNode.h"
#include "HPointLightNode.h"
#include "HSpotLightNode.h"
#include "HAmbientLightNode.h"
#include "HSurfaceNode.h"
#include "Engine\Render\Mesh\ISurface.h"
#include "Engine\Render\Shader\HShadowDepthShader.h"


struct DLightGroup
{
private:
	DString Name;
	DSet<shared<HLightNode>> Lights;
	DSet<shared<HSurfaceNode>> Surfaces;
public:
	const DSet<shared<HLightNode>> GetLights() const;
	const DSet<shared<HSurfaceNode>> GetSurfaces() const;
	const DString GetName() const;
	void SetName(DStringView NewName);

	friend class HScene;
};


class HScene : public HAsset
{
public:

	static const int32 LIGHT_GROUP_COUNT = 64;

	static const int32 DIR_LIGHT_LIMIT = 4;
	static const int32 POINT_LIGHT_LIMIT = 10;
	static const int32 SPOT_LIGHT_LIMIT = 10;
	static const int32 AMBIENT_LIGHT_LIMIT = 1;

	static const int32 SHADOWED_DIR_LIGHT_LIMIT = 1;
	static const int32 SHADOWED_POINT_LIGHT_LIMIT = 4;
	static const int32 SHADOWED_SPOT_LIGHT_LIMIT = 4;


	struct DSurfaceRenderInfo
	{
		HSurfaceNode* Node = nullptr;
		ISurface* Surface = nullptr;
	};


	struct DMaterialRenderGroup
	{
		DArray<DSurfaceRenderInfo> Surfaces;
	};


	struct DShaderRenderGroup
	{
		DMap<HSurfaceMaterial*, DMaterialRenderGroup> Materials;
	};


	struct DTransparentSubmeshInfo
	{
		HSurfaceMaterial* Material;
		HStaticSubMesh* Submesh;
		HStaticMeshNode* Node;
	};


	struct DSceneRenderData
	{
		DMap<HSurfaceShader*, DShaderRenderGroup> Shaders;
		DArray<DTransparentSubmeshInfo> TransparentSubmeshes;

		void Clear();
	};


	struct DSceneShadowRenderData
	{
		DMap<HSurfaceShader*, DShaderRenderGroup> Shaders;

		void Clear();
	};


private:
	shared<HSceneNode> Root;
	DFixedArray<DLightGroup, 64> LightGroups;
	HTimerManager* Timer = new HTimerManager();
	shared<HCameraNode> ActiveCamera;
	shared<HTextureCubeBase> SkyboxTexture;
	shared<HSurfaceMaterial> SkyboxMaterial;
	shared<HStaticMeshNode> SkyboxNode;

public:
	HScene();
	~HScene();

	void Init();

	shared<HSceneNode> GetRoot() const;
	HSceneNode* GetRootPtr() const;
	
private:
	/* Not null-checked! */
	void ApplyLightsOnShader(HSurfaceNode* SurfaceNode, HSurfaceShader* Shader, bool DrawShadows);
	
private:
	void _GatherRenderData(HSceneNode* Parent, DSceneRenderData& DataRoot) const;
public:
	void PopulateRenderData(DSceneRenderData& Data) const;

private:
	void GatherShadowRenderData(DSceneShadowRenderData& OutData, const DSet<shared<HSurfaceNode>>& SurfaceNodes, const DFrustumPlanes& LightFrustum) const;

public:
	void RenderShadowDepths();
	void Draw(bool UseStencil, DSceneRenderData& SceneRenderData, bool DrawShadows);

	HTimerManager* GetTimer() const;

	void SetLightGroupName(int32 Index, DStringView GroupName);
	void AddMeshToLightGroup(int32 Index, shared<HSurfaceNode> Node);
	void AddMeshesToLightGroup(int32 Index, const DArray<shared<HSurfaceNode>>& Nodes);
	void AddLightToLightGroup(int32 Index, shared<HLightNode> Node);
	void AddLightsToLightGroup(int32 Index, const DArray<shared<HLightNode>>& Nodes);
	void RemoveMeshFromLightGroup(int32 Index, shared<HSurfaceNode> Node);
	void RemoveLightFromLightGroup(int32 Index, shared<HLightNode> Node);
	void RemoveAllMeshesFromLightingGroup(int32 Index);
	void RemoveAllLightsFromLightingGroup(int32 Index);
	void RemoveMeshFromAllLightGroups(shared<HSurfaceNode> Node);
	void RemoveLightFromAllLightGroups(shared<HLightNode> Node);
	const DFixedArray<DLightGroup, 64>& GetLightGroups() const;

	shared<HCameraNode> GetActiveCamera() const;
	HCameraNode* GetActiveCameraPtr() const;
	void SetActiveCamera(shared<HCameraNode> Camera);

	/* Callback: (const shared<HSceneNode>& Node) -> void */
	template<typename CallbackT>
	void ForEachNode(shared<HSceneNode> Parent, const CallbackT& Callback)
	{
		const DArray<shared<HSceneNode>>& children = Parent->GetChildren();
		for (int32 i = 0; i < children.Length(); i++)
		{
			Callback(children[i]);
			this->ForEachNode(children[i], Callback);
		}
	}

	/* Callback: (HSceneNode* Node) -> void */
	template<typename CallbackT>
	void ForEachNode(HSceneNode* Parent, const CallbackT& Callback)
	{
		const DArray<shared<HSceneNode>>& children = Parent->GetChildren();
		for (int32 i = 0; i < children.Length(); i++)
		{
			HSceneNode* child = children[i].Ptr();
			Callback(child);
			this->ForEachNode(child, Callback);
		}
	}

	/* 
	Callback: (const shared<HSceneNode>& Node) -> bool
	if Callback returns true, the loop breaks
	*/
	template<typename CallbackT>
	bool ForEachNodeWithBreak(shared<HSceneNode> Parent, const CallbackT& Callback)
	{
		const DArray<shared<HSceneNode>>& children = Parent->GetChildren();
		for (int32 i = 0; i < children.Length(); i++)
		{
			bool ret = Callback(children[i]);
			if (ret) return true;
			if (this->ForEachNodeWithBreak(children[i], Callback)) return true;
		}
		return false;
	}

	/* 
	Callback: (HSceneNode* Node) -> bool
	if Callback returns true, the loop breaks
	*/
	template<typename CallbackT>
	bool ForEachNodeWithBreak(HSceneNode* Parent, const CallbackT& Callback)
	{
		const DArray<shared<HSceneNode>>& children = Parent->GetChildren();
		for (int32 i = 0; i < children.Length(); i++)
		{
			HSceneNode* child = children[i].Ptr();
			bool ret = Callback(child);
			if (ret) return true;
			if (this->ForEachNodeWithBreak(child, Callback)) return true;
		}
		return false;
	}
	
	virtual void Load() override;
	virtual bool IsLoaded() override;
	virtual void Unload() override;

	shared<HTextureCubeBase> GetSkyboxTexture() const;
	HTextureCubeBase* GetSkyboxTexturePtr() const;
	void SetSkyboxTexture(shared<HTextureCubeBase> Cubemap);

};
