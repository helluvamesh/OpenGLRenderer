/*
* HShaderBase is the CPU-side representation of an OpenGL 'Program'
* it is composed of multiple shader source code files - these are called 'Shader' in OpenGL
* the type of OpenGL shaders are detected by the extension of their source file
* - .vert (vertex)
* - .frag (fragment)
* - .geom (geometry)
* - .comp (compute)
* - .tesc (tessellation control)
* - .tese (tessellation evaluation)
* uniforms are parsed on load and meta data is stored on the CPU (type, default value, GPU location, texture unit)
* texture samplers are assigned to texture units
* handled uniform types (EShaderParamType, GLSL Type, C++ Type):
* - INT: (bool, int), (bool, int32)
* - FLOAT: (float), (float)
* - VEC4: (vec4), (FVec4)
* - TEXTURE: (sampler2D, sampler2DMS, samplerCube), (HTextureBase)
* 
* HSurfaceMaterial, ...:
* a material is a collection of shader uniforms that can be applied to primitives
* 
* GetBaseUniformNames()
* should be overriden to disable parameter generation for core uniforms 
* like matrices or the ones the lighting system uses
*/

#pragma once

#include "Engine/Asset/HAsset.h"
#include "Engine/Render/Texture/HTextureBase.h"


enum class EShaderParamType : uint8 { INT, FLOAT, VEC4, TEXTURE };


struct DShaderUniformInfo
{
public:
	EShaderParamType Type = EShaderParamType::FLOAT;
	int32 Location = 0;
	int32 TextureUnit = 0;
	FVec4 DefaultValue;
};


// abstract
class HShaderBase : public HAsset
{
private:
	static const DSet<DString8> EMPTY_UNIFORM_NAME_SET;

	uint32 ProgramId = 0;
	DMap<DString, DShaderUniformInfo> Uniforms;

protected:
	int32 TextureUnitCount = 0;
	
public:
	HShaderBase() = default;
	~HShaderBase();

private:
	void FreeGPU();
	static void DeleteShaders(const DArray<uint32>& ShaderIds);

protected:
	void _Load(const DArray<DString>& SourceFilepaths);
	virtual void OnLoad();

public:
	bool IsLoaded() override;
	void Unload() override;

	void Use();
	static void UseNone();
	uint32 glGetProgramId();
	
	const DMap<DString, DShaderUniformInfo>& GetUniforms();
	virtual const DSet<DString8>& GetBaseUniformNames();
	
	int32 GetUniformLoc(const DString& InName);
	int32 GetTextureUnit(const DString& InName);
	
	// shader must be 'used'
	int32 GetUniformLocFromGPU(DStringView8 InName);

protected:
	// shader must be 'used'
	void AssignSamplerToTextureUnit(int32 Location, int32 TextureUnitIndex);
	
public:
	// shader must be 'used'
	void SetBoolParam(int32 Location, bool Value);
	// shader must be 'used'
	void SetFloatParam(int32 Location, float Value);
	// shader must be 'used'
	void SetIntParam(int32 Location, int32 Value);
	// shader must be 'used'
	void SetVec2Param(int32 Location, FVec2 Vec);
	// shader must be 'used'
	void SetVec2Param(int32 Location, float X, float Y);
	// shader must be 'used'
	void SetVec3Param(int32 Location, const FVec3& Vec);
	// shader must be 'used'
	void SetVec4Param(int32 Location, const FVec4& Vec);
	// shader must be 'used'
	void SetVec4Param(int32 Location, const FVec3& Vec, float W);
	// shader must be 'used'
	void SetMat4Param(int32 Location, const FMat4& Mat);
	// shader must be 'used'
	void SetTextureParam(int32 TextureUnit, HTextureBase* InTexture);
	// shader must be 'used'
	void SetFloatArrayParam(int32 Location, const float* Array, int32 Length);
	// shader must be 'used'
	void SetIntArrayParam(int32 Location, const int32* Array, int32 Length);
	// shader must be 'used'
	void SetVec2ArrayParam(int32 Location, const FVec2* Array, int32 Length);
	// shader must be 'used'
	void SetVec3ArrayParam(int32 Location, const FVec3* Array, int32 Length);
	// shader must be 'used'
	void SetVec4ArrayParam(int32 Location, const FVec4* Array, int32 Length);
	// shader must be 'used'
	void SetMat4ArrayParam(int32 Location, const FMat4* Array, int32 Length);
};