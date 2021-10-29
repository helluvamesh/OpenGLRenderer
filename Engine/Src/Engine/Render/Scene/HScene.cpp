#include "Engine/pch.h"
#include "HScene.h"
#include "GL\glew.h"
#include "Engine\Render\RenderUtil\GLUtil.h"
#include "HLineWorkNode.h"
#include "Engine\Render\Texture\HFrameBufferDepth.h"
#include "Engine\Util\SEngine.h"


// ----------------------- DLightGroup ---------------------


const DSet<shared<HLightNode>> DLightGroup::GetLights() const
{
	return this->Lights;
}


const DSet<shared<HSurfaceNode>> DLightGroup::GetSurfaces() const
{
	return this->Surfaces;
}


const DString DLightGroup:: GetName() const
{
	return this->Name;
}
void DLightGroup::SetName(DStringView NewName)
{
	this->Name = DString::From(NewName);
}


// ------------------ HScene::DSceneRenderData -------------------


void HScene::DSceneRenderData::Clear()
{
	this->Shaders.Clear();
	this->TransparentSubmeshes.Clear();
}


// ------------------ HScene::DSceneShadowRenderData ------------------


void HScene::DSceneShadowRenderData::Clear()
{
	this->Shaders.Clear();
}


// -------------------------- HScene ------------------------


HScene::HScene()
{
	DSurfaceShaderSettings environmentShaderSettings;
	environmentShaderSettings.SourceFilepaths = {
		SEngine::ContentPath + TX("Shaders/Environment.vert"),
		SEngine::ContentPath + TX("Shaders/EnvironmentSkybox.frag")
	};
	shared<HSurfaceShader> environmentShader = MakeShared<HSurfaceShader>();
	environmentShader->Load(environmentShaderSettings);

	DSurfaceMaterialSettings environmentMaterialSettings;
	environmentMaterialSettings.Shader = environmentShader;
	environmentMaterialSettings.Skybox = true;
	this->SkyboxMaterial = MakeShared<HSurfaceMaterial>();
	this->SkyboxMaterial->Load(environmentMaterialSettings);
	this->SkyboxMaterial->SetDoubleSided(true);

	shared<HStaticMesh> skyboxMesh = MakeShared<HStaticMesh>();
	skyboxMesh->SetFilepath(SEngine::ContentPath + TX("Meshes/UnitCube.asset"));
	skyboxMesh->Load();
	skyboxMesh->SetMaterialInSlot(0, this->SkyboxMaterial);

	this->SkyboxNode = MakeShared<HStaticMeshNode>();
	this->SkyboxNode->SetMesh(skyboxMesh);
}


HScene::~HScene()
{
	delete this->Timer;
}


void HScene::Init()
{
	this->Root = MakeShared<HSceneNode>(ESceneNodeType::NONE, WeakThisAs<HScene>());
}


shared<HSceneNode> HScene::GetRoot() const
{
	return this->Root;
}
HSceneNode* HScene::GetRootPtr() const
{
	return this->Root.Ptr();
}


void HScene::ApplyLightsOnShader(HSurfaceNode* SurfaceNode, HSurfaceShader* Shader, bool DrawShadows)
{
	int32 dirLightCount = 0;
	int32 pointLightCount = 0;
	int32 spotLightCount = 0;
	int32 ambientLightCount = 0;

	int32 shadowedDirLightCount = 0;
	int32 shadowedPointLightCount = 0;
	int32 shadowedSpotLightCount = 0;

	for (int32 i = 0; i < LIGHT_GROUP_COUNT; ++i)
	{ 
		if (SBitSet::Get(SurfaceNode->LightGroups, i))
		{
			DLightGroup& lightGroup = this->LightGroups[i];

			for (const shared<HLightNode>& _light : lightGroup.Lights)
			{
				HLightNode* light = _light.Ptr();
				if (light)
				{
					switch (light->LightType)
					{
					case ELightType::DIRECTIONAL:
					{
						if (dirLightCount < DIR_LIGHT_LIMIT)
						{
							HDirectionalLightNode* dirLight = static_cast<HDirectionalLightNode*>(light);
							bool shadowed = dirLight->GetEnableShadows() && shadowedDirLightCount < SHADOWED_DIR_LIGHT_LIMIT;
							dirLight->ApplyOnShader(Shader, dirLightCount, shadowed);
							if (shadowed) shadowedDirLightCount++;
							dirLightCount++;
						}
						break;
					}
					case ELightType::POINT:
					{
						if (pointLightCount < POINT_LIGHT_LIMIT)
						{
							HPointLightNode* pointLight = static_cast<HPointLightNode*>(light);
							pointLight->ApplyOnShader(Shader, pointLightCount);
							pointLightCount++;
						}
						break;
					}
					case ELightType::SPOT:
					{
						if (spotLightCount < SPOT_LIGHT_LIMIT)
						{
							HSpotLightNode* spotLight = static_cast<HSpotLightNode*>(light);
							bool shadowed = spotLight->GetEnableShadows() && shadowedSpotLightCount < SHADOWED_SPOT_LIGHT_LIMIT;
							spotLight->ApplyOnShader(Shader, spotLightCount, shadowed? shadowedSpotLightCount : -1);
							if (shadowed) shadowedSpotLightCount++;
							spotLightCount++;
						}
						break;
					}
					case ELightType::AMBIENT_CUBEMAP:
					{
						if (ambientLightCount < AMBIENT_LIGHT_LIMIT)
						{
							HAmbientLightNode* ambientLight = static_cast<HAmbientLightNode*>(light);
							ambientLight->ApplyOnShader(Shader, ambientLightCount);
						}
						break;
					}
					}
				}
			}
		}
	}

	const DBaseSurfaceShaderLocs& locs = Shader->GetBaseUniformLocs();
	Shader->SetIntParam(locs.DirLightCount, dirLightCount);
	Shader->SetIntParam(locs.PointLightCount, pointLightCount);
	Shader->SetIntParam(locs.SpotLightCount, spotLightCount);

	if (shadowedDirLightCount == 0)
	{
		Shader->SetIntParam(locs.ShadowedDirLightIdx, -1);
	}
}


void HScene::_GatherRenderData(HSceneNode* Parent, DSceneRenderData& DataRoot) const
{
	uint8 stencil_i = 1;

	const DArray<shared<HSceneNode>>& children = Parent->GetChildren();
	for (int32 i = 0; i < children.Length(); i++)
	{
		HSceneNode* node = children[i].Ptr();
		if (node->Visible)
		{
			if (node->Type == ESceneNodeType::STATIC_MESH)
			{
				HStaticMeshNode* meshNode = static_cast<HStaticMeshNode*>(node);
				HStaticMesh* mesh = meshNode->Mesh.Ptr();
				if (mesh)
				{
					// frustum culling
					const DCameraRenderData& cam = this->ActiveCamera->GetCameraData();
					if (cam.FrustumPlanes.Test(meshNode->WorldBoundingSphere) &&
						cam.FrustumPlanes.Test(meshNode->WorldBoundingBox))
					{
						meshNode->LastStencilValue = stencil_i < 255? stencil_i++ : 0;
						
						for (int32 i = 0; i < meshNode->MaterialSlots.Length(); ++i)
						{ 
							HSurfaceMaterial* material = meshNode->MaterialSlots[i].Material.Ptr();
							if (material)
							{
								HStaticSubMesh* submesh = meshNode->MaterialSlots[i].Submesh.Ptr();
								if (submesh)
								{
									HSurfaceShader* shader = material->GetShaderPtr();
									if (shader)
									{
										DShaderRenderGroup* shaderGroup;
										if (material->BlendMode != ESurfaceMaterialBlendMode::ALPHA_BLEND)
										{
											shaderGroup = DataRoot.Shaders.Find(shader);
											if (shaderGroup == nullptr)
											{
												shaderGroup = &DataRoot.Shaders.AddDefault(shader);
											}
											DMaterialRenderGroup* materialGroup = shaderGroup->Materials.Find(material);
											if (materialGroup == nullptr)
											{
												materialGroup = &shaderGroup->Materials.AddDefault(material);
											}
											DSurfaceRenderInfo& surfaceInfo = materialGroup->Surfaces.AddDefault();
											surfaceInfo.Node = meshNode;
											surfaceInfo.Surface = submesh;
										}
										else
										{
											DTransparentSubmeshInfo& info = DataRoot.TransparentSubmeshes.AddDefault();
											info.Material = material;
											info.Node = meshNode;
											info.Submesh = submesh;
										}
									}
									else SLog::Error(TX("Missing shader in material \"") + material->Name + TX("\""));
								}
							}
							else SLog::Error(TX("StaticMesh missing material in slot ") + DNumberFormat::DEFAULT.ToString(i) + TX(", in mesh \"") + mesh->GetName() + TX("\""));
						}
					}
				}
				else SLog::Error(TX("StaticMeshNode has no mesh: ") + meshNode->Name);
			}
			else if (node->Type == ESceneNodeType::LINE_WORK)
			{
				HLineWorkNode* lineNode = static_cast<HLineWorkNode*>(node);
				HLineWork* lineWork = lineNode->LineWork.Ptr();

				// frustum culling
				const DCameraRenderData& cam = this->ActiveCamera->GetCameraData();

				if (cam.FrustumPlanes.Test(lineNode->GetBoundingBox()))
				{
					HLineStroke* lineStroke = lineWork->GetStrokePtr();
					if (lineStroke)
					{
						HSurfaceMaterial* material = lineWork->GetMaterialPtr();
						if (material)
						{
							HSurfaceShader* shader = material->GetShaderPtr();
							if (shader)
							{
								DShaderRenderGroup* shaderGroup = DataRoot.Shaders.Find(shader);
								if (shaderGroup == nullptr)
								{
									shaderGroup = &DataRoot.Shaders.AddDefault(shader);
								}
								DMaterialRenderGroup* materialGroup = shaderGroup->Materials.Find(material);
								if (materialGroup == nullptr)
								{
									materialGroup = &shaderGroup->Materials.AddDefault(material);
								}
								DSurfaceRenderInfo& surfaceInfo = materialGroup->Surfaces.AddDefault();
								surfaceInfo.Node = lineNode;
								surfaceInfo.Surface = lineStroke;
							}
						}
					}
				}
			}
		}

		this->_GatherRenderData(node, DataRoot);
	}
}


void HScene::PopulateRenderData(DSceneRenderData& Data) const
{
	Data.Clear();
	this->_GatherRenderData(this->Root.Ptr(), Data);
}


void HScene::GatherShadowRenderData(DSceneShadowRenderData& OutData, const DSet<shared<HSurfaceNode>>& SurfaceNodes, const DFrustumPlanes& LightFrustum) const
{
	for (const shared<HSurfaceNode>& surfaceNode_ : SurfaceNodes)
	{
		HSurfaceNode* surfaceNode = surfaceNode_.Ptr();
		if (surfaceNode && surfaceNode->Visible)
		{
			if (surfaceNode->Type == ESceneNodeType::STATIC_MESH)
			{
				HStaticMeshNode* meshNode = static_cast<HStaticMeshNode*>(surfaceNode);
				if (meshNode->CastShadow &&
					LightFrustum.Test(meshNode->WorldBoundingSphere) &&
					LightFrustum.Test(meshNode->WorldBoundingBox))
				{
					HStaticMesh* mesh = meshNode->Mesh.Ptr();
					if (mesh)
					{
						for (int32 i = 0; i < meshNode->MaterialSlots.Length(); ++i)
						{ 
							HSurfaceMaterial* material = meshNode->MaterialSlots[i].Material.Ptr();
							if (material)
							{
								HStaticSubMesh* submesh = meshNode->MaterialSlots[i].Submesh.Ptr();
								if (submesh)
								{
									HSurfaceShader* shader = material->GetShaderPtr();
									if (shader)
									{
										DShaderRenderGroup* shaderGroup;
										if (material->GetBlendMode() != ESurfaceMaterialBlendMode::ALPHA_BLEND)
										{
											shaderGroup = OutData.Shaders.Find(shader);
											if (shaderGroup == nullptr)
											{
												shaderGroup = &OutData.Shaders.AddDefault(shader);
											}
											DMaterialRenderGroup* materialGroup = shaderGroup->Materials.Find(material);
											if (materialGroup == nullptr)
											{
												materialGroup = &shaderGroup->Materials.AddDefault(material);
											}
											DSurfaceRenderInfo& surfaceInfo = materialGroup->Surfaces.AddDefault();
											surfaceInfo.Node = meshNode;
											surfaceInfo.Surface = submesh;
										}
									}
									else SLog::Error(TX("Missing shader in material \"") + material->Name + TX("\""));
								}
							}
							else SLog::Error(TX("StaticMesh missing material in slot ") + DNumberFormat::DEFAULT.ToString(i) + TX(", in mesh \"") + mesh->GetName() + TX("\""));
						}
					}
					else SLog::Error(TX("StaticMeshNode has no mesh: ") + meshNode->Name);
				}
			}
		}
	}
}


void HScene::RenderShadowDepths()
{
	if (this->ActiveCamera)
	{
		const DCameraRenderData& cameraData = this->ActiveCamera->GetCameraData();
	
		for (DLightGroup& lightGroup : this->LightGroups)
		{
			for (const shared<HLightNode>& lightNode_ : lightGroup.Lights)
			{
				HLightNode* lightNode = lightNode_.Ptr();
				if (lightNode == nullptr || lightNode->Visible == false) continue;

				switch (lightNode->LightType)
				{
				case ELightType::DIRECTIONAL:
				{
					HDirectionalLightNode* sun = static_cast<HDirectionalLightNode*>(lightNode);
					if (sun->GetEnableShadows())
					{
						DSceneShadowRenderData sceneRenderData;
						this->GatherShadowRenderData(sceneRenderData, lightGroup.Surfaces, sun->GetFrustum());

						HFrameBufferDepth* fbo = sun->ShadowFBO.Ptr();
						fbo->Bind(true);

						// draw objects
						for (auto& entry : sceneRenderData.Shaders)
						{
							HShadowDepthShader* shadowDepthShader = entry.GetKey()->GetShadowDepthShaderPtr();
							if (shadowDepthShader)
							{
								DShaderRenderGroup& shaderGroup = entry.GetValue();

								const DBaseShadowDepthShaderLocs& locs = shadowDepthShader->GetBaseUniformLocs();
								shadowDepthShader->Use();
								int32 cascadeCount = sun->GetShadowCascades();
								shadowDepthShader->SetMat4ArrayParam(locs.LightSpaceMats, sun->GetLightSpaceMats().Data(), cascadeCount);
								shadowDepthShader->SetIntParam(locs.Layers, cascadeCount);
								int32 modelMatLoc = locs.ModelMat;
								for (auto& subEntry : shaderGroup.Materials)
								{
									HSurfaceMaterial* material = subEntry.GetKey();
									DMaterialRenderGroup& materialGroup = subEntry.GetValue();
								
									material->ApplyStateOnShadowDepthShader(shadowDepthShader);
									
									for (DSurfaceRenderInfo& surfaceInfo : materialGroup.Surfaces)
									{
										HSurfaceNode* surfaceNode = surfaceInfo.Node;
										ISurface* surface = surfaceInfo.Surface;
										const DSceneNodeRenderData& nodeData = surfaceNode->GetRenderData();
								
										material->ApplyNodeStateOnShadowDepthShader(cameraData, nodeData, shadowDepthShader);

										shadowDepthShader->SetMat4Param(modelMatLoc, nodeData.WorldMatrix);
										surface->DrawStandalone();
									}
								}
							}
						}
						fbo->BindNone();
					}
					break;
				}
				case ELightType::SPOT:
				{
					HSpotLightNode* spotLight = static_cast<HSpotLightNode*>(lightNode);
					if (spotLight->GetEnableShadows())
					{
						DSceneShadowRenderData sceneRenderData;
						this->GatherShadowRenderData(sceneRenderData, lightGroup.Surfaces, spotLight->GetFrustum());

						HFrameBufferDepth* fbo = spotLight->ShadowFBO.Ptr();
						fbo->Bind(true);

						// draw objects
						for (auto& entry : sceneRenderData.Shaders)
						{
							HShadowDepthShader* shadowDepthShader = entry.GetKey()->GetShadowDepthShaderPtr();
							if (shadowDepthShader)
							{
								DShaderRenderGroup& shaderGroup = entry.GetValue();

								const DBaseShadowDepthShaderLocs& locs = shadowDepthShader->GetBaseUniformLocs();
								shadowDepthShader->Use();
								shadowDepthShader->SetMat4Param(locs.LightSpaceMats_0, spotLight->GetLightSpaceMat());
								shadowDepthShader->SetIntParam(locs.Layers, 1);
								int32 modelMatLoc = locs.ModelMat;
								for (auto& subEntry : shaderGroup.Materials)
								{
									HSurfaceMaterial* material = subEntry.GetKey();
									DMaterialRenderGroup& materialGroup = subEntry.GetValue();

									material->ApplyStateOnShadowDepthShader(shadowDepthShader);
									
									for (DSurfaceRenderInfo& surfaceInfo : materialGroup.Surfaces)
									{
										HSurfaceNode* surfaceNode = surfaceInfo.Node;
										ISurface* surface = surfaceInfo.Surface;
										const DSceneNodeRenderData& nodeData = surfaceNode->GetRenderData();
										
										material->ApplyNodeStateOnShadowDepthShader(cameraData, nodeData, shadowDepthShader);

										shadowDepthShader->SetMat4Param(modelMatLoc, nodeData.WorldMatrix);
										surface->DrawStandalone();
									}
								}
							}
						}
						fbo->BindNone();
					}
					break;
				}
				default: continue;
				}
			}
		}
	}
}


void HScene::Draw(bool UseStencil, DSceneRenderData& SceneRenderData, bool DrawShadows)
{
	if (this->ActiveCamera)
	{
		const DCameraRenderData& cameraData = this->ActiveCamera->GetCameraData();
		
		// SKYBOX

		if (this->SkyboxTexture && this->SkyboxNode && this->SkyboxMaterial)
		{
			HSurfaceShader* shader = this->SkyboxMaterial->GetShaderPtr();
			HStaticMesh* mesh = this->SkyboxNode->GetMeshPtr();
			if (shader && mesh)
			{
				glCheck(glDepthMask(GL_FALSE));
				shader->Use();
				this->SkyboxMaterial->ApplyStateOnShader(shader);
				this->SkyboxMaterial->ApplyNodeStateOnShader(cameraData, this->SkyboxNode->GetRenderData(), shader);
				if (UseStencil)
				{
					glCheck(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
					glCheck(glStencilFunc(GL_ALWAYS, 0, 0xFF));
				}
				mesh->DrawStandalone();
				glCheck(glDepthMask(GL_TRUE));
			}
		}

		// NON-TRANSPARENT STATIC MESHES

		for (auto& entry : SceneRenderData.Shaders)
		{
			HSurfaceShader* shader = entry.GetKey();
			DShaderRenderGroup& shaderGroup = entry.GetValue();

			// switch shader
			shader->Use();

			for (auto& subEntry : shaderGroup.Materials)
			{
				HSurfaceMaterial* material = subEntry.GetKey();
				DMaterialRenderGroup& materialGroup = subEntry.GetValue();
				
				// send material properties to shader
				material->ApplyStateOnShader(shader);

				for (DSurfaceRenderInfo& surfaceInfo : materialGroup.Surfaces)
				{
					HSurfaceNode* surfaceNode = surfaceInfo.Node;
					ISurface* surface = surfaceInfo.Surface;
					const DSceneNodeRenderData& nodeData = surfaceNode->GetRenderData();
					// send node data to shader
					material->ApplyNodeStateOnShader(cameraData, nodeData, shader);
					
					this->ApplyLightsOnShader(surfaceNode, shader, DrawShadows);

					// draw into stencil buffer
					if (UseStencil && surfaceNode->LastStencilValue > 0)
					{
						glCheck(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
						glCheck(glStencilFunc(GL_ALWAYS, surfaceNode->LastStencilValue, 0xFF));
					}
					
					// draw mesh
					surface->Draw(material);
				}
			}
		}

		// TRANSPAREST STATIC MESHES

		// sort transparent submeshes by distance to camera (descending)
		DArray<DTransparentSubmeshInfo>& transparentSubmeshes = SceneRenderData.TransparentSubmeshes;
		transparentSubmeshes.Sort(
			[&cameraData](DTransparentSubmeshInfo& A, DTransparentSubmeshInfo& B) {
				
				return   A.Node->GetWorldLocation().Distance(cameraData.WorldLocation)
					   > B.Node->GetWorldLocation().Distance(cameraData.WorldLocation);
			}
		);
		
		HSurfaceShader* prevShader = nullptr;
		HSurfaceMaterial* prevMaterial = nullptr;
		
		for (int32 i = 0; i < transparentSubmeshes.Length(); i++)
		{
			DTransparentSubmeshInfo& info = transparentSubmeshes[i];
			HSurfaceMaterial* material = info.Material;
			if (material)
			{
				HSurfaceShader* shader = material->GetShaderPtr();
				if (shader)
				{
					if (shader != prevShader)
					{
						shader->Use();
						prevShader = shader;
					}
					if (material != prevMaterial)
					{
						material->ApplyStateOnShader(shader);
						prevMaterial = material;
					}	
					material->ApplyNodeStateOnShader(cameraData, info.Node->GetRenderData(), shader);
					this->ApplyLightsOnShader(info.Node, shader, DrawShadows);
					info.Submesh->Draw(material);
				}
			}
		}
	}
	else
	{
		SLog::Error(TX("Trying to draw Scene with Scene::ActiveCamera being nullptr in ") + this->Name);
	}
}


HTimerManager* HScene::GetTimer() const
{
	return this->Timer;
}


void HScene::SetLightGroupName(int32 Index, DStringView GroupName)
{
	this->LightGroups[Index].SetName(GroupName);
}


void HScene::AddMeshToLightGroup(int32 Index, shared<HSurfaceNode> Node)
{
	if (this->LightGroups.IsValidIndex(Index) == false || Node == nullptr) return;
	
	if (SBitSet::Get(Node->LightGroups, Index) == false)
	{
		this->LightGroups[Index].Surfaces.Add(Node);
		SBitSet::Set(Node->LightGroups, Index, true);
	}
}


void HScene::AddMeshesToLightGroup(int32 Index, const DArray<shared<HSurfaceNode>>& Nodes)
{
	if (this->LightGroups.IsValidIndex(Index) == false) return;

	DLightGroup& group = this->LightGroups[Index];
	for (shared<HSurfaceNode> node : Nodes)
	{
		if (SBitSet::Get(node->GetLightGroups(), Index) == false)
		{
			group.Surfaces.Add(node);
			SBitSet::Set(node->LightGroups, Index, true);
		}
	}
}


void HScene::AddLightToLightGroup(int32 Index, shared<HLightNode> Node)
{
	if (this->LightGroups.IsValidIndex(Index) == false || Node == nullptr) return;

	if (SBitSet::Get(Node->LightGroups, Index) == false)
	{
		this->LightGroups[Index].Lights.Add(Node);
		SBitSet::Set(Node->LightGroups, Index, true);
	}
}


void HScene::AddLightsToLightGroup(int32 Index, const DArray<shared<HLightNode>>& Nodes)
{
	if (this->LightGroups.IsValidIndex(Index) == false) return;

	DLightGroup& group = this->LightGroups[Index];
	for (shared<HLightNode> node : Nodes)
	{
		if (node)
		{
			if (SBitSet::Get(node->LightGroups, Index) == false)
			{
				group.Lights.Add(node);
				SBitSet::Set(node->LightGroups, Index, true);
			}
		}
	}
}


void HScene::RemoveMeshFromLightGroup(int32 Index, shared<HSurfaceNode> Node)
{
	if (this->LightGroups.IsValidIndex(Index) == false || Node == nullptr) return;
	
	if (SBitSet::Get(Node->LightGroups, Index))
	{
		SBitSet::Set(Node->LightGroups, Index, false);
		this->LightGroups[Index].Surfaces.Remove(Node);
	}
}


void HScene::RemoveLightFromLightGroup(int32 Index, shared<HLightNode> Node)
{
	if (this->LightGroups.IsValidIndex(Index) == false || Node == nullptr) return;

	if (SBitSet::Get(Node->LightGroups, Index))
	{
		SBitSet::Set(Node->LightGroups, Index, false);
		this->LightGroups[Index].Lights.Remove(Node);
	}
}


void HScene::RemoveAllMeshesFromLightingGroup(int32 Index)
{
	if (this->LightGroups.IsValidIndex(Index) == false) return;
	
	DSet<shared<HSurfaceNode>>& surfaces = this->LightGroups[Index].Surfaces;
	for (const shared<HSurfaceNode>& surface : surfaces)
	{
		SBitSet::Set(surface->LightGroups, Index, false);
	}
	surfaces.Clear();
}


void HScene::RemoveAllLightsFromLightingGroup(int32 Index)
{
	if (this->LightGroups.IsValidIndex(Index) == false) return;

	DSet<shared<HLightNode>>& lights = this->LightGroups[Index].Lights;
	for (const shared<HLightNode>& light : lights)
	{
		SBitSet::Set(light->LightGroups, Index, false);
	}
	lights.Clear();
}


void HScene::RemoveMeshFromAllLightGroups(shared<HSurfaceNode> Node)
{
	if (Node == nullptr) return;
	
	for (int32 i = 0; LIGHT_GROUP_COUNT; ++i)
	{
		if (SBitSet::Get(Node->LightGroups, i))
		{
			this->LightGroups[i].Surfaces.Remove(Node);
		}
	}
}


void HScene::RemoveLightFromAllLightGroups(shared<HLightNode> Node)
{
	if (Node == nullptr) return;

	for (int32 i = 0; LIGHT_GROUP_COUNT; ++i)
	{
		this->LightGroups[i].Lights.Remove(Node);
	}
}


const DFixedArray<DLightGroup, 64>& HScene::GetLightGroups() const
{
	return this->LightGroups;
}


shared<HCameraNode> HScene::GetActiveCamera() const
{
	return this->ActiveCamera;
}
HCameraNode* HScene::GetActiveCameraPtr() const
{
	return this->ActiveCamera.Ptr();
}
void HScene::SetActiveCamera(shared<HCameraNode> Camera)
{
	this->ActiveCamera = Camera;
}


void HScene::Load()
{
}


bool HScene::IsLoaded()
{
	return true;
}


void HScene::Unload()
{
}


shared<HTextureCubeBase> HScene::GetSkyboxTexture() const
{
	return this->SkyboxTexture;
}
HTextureCubeBase* HScene::GetSkyboxTexturePtr() const
{
	return this->SkyboxTexture.Ptr();
}
void HScene::SetSkyboxTexture(shared<HTextureCubeBase> Cubemap)
{
	this->SkyboxTexture = Cubemap;
	if (this->SkyboxMaterial)
	{
		this->SkyboxMaterial->SetTextureParam(TX("EnvMap"), Cubemap);
	}
}
