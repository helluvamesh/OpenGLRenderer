#pragma once

#include "Engine\Asset\HAsset.h"
#include "DLineWorkData.h"
#include "Engine\Render\Shader\HSurfaceMaterial.h"
#include "ISurface.h"


class HLineStroke : public HObject, public ISurface
{
private:

	uint32 VertArrayId = 0;
	uint32 VertBufferId = 0;
	uint32 IndexBufferId = 0;
	int32 IndexBufferLength = 0;
	
	float LineWidth = 1.0f;

public:

	HLineStroke(const DLineStrokeData& Data);
	~HLineStroke();

	void UpdateData(const DLineStrokeData& Data);
	
	// ISurface
	// a shader must be 'used'
	void Draw(HSurfaceMaterial* DrawMaterial) override;
	// a shader must be 'used'
	void DrawStandalone() override;
	// .
	
};


class HLineWork : public HAsset
{
private:
	shared<HLineStroke> Stroke;
	shared<HSurfaceMaterial> Material;
	FAxisBox BoundingBox;

public:
	void Load() override;
	bool IsLoaded() override;
	void Unload() override;

	void Load(const DLineWorkData& Data);
	void UpdateData(const DLineWorkData& Data);

	const FAxisBox& GetBoundingBox() const;

	shared<HSurfaceMaterial> GetMaterial() const;
	HSurfaceMaterial* GetMaterialPtr() const;
	void SetMaterial(shared<HSurfaceMaterial> NewMaterial);
	
	shared<HLineStroke> GetStroke() const;
	HLineStroke* GetStrokePtr() const;

};