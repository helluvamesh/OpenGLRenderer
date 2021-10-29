#pragma once


struct HSceneQuad : public HObject
{
private:
	uint32 VertArrayId;
	uint32 VertBufferId;
	uint32 IndexBufferId;
	int32 VertBufferLength;
	int32 IndexBufferLength;

public:
	HSceneQuad();
	~HSceneQuad();

	void Draw() const;
};