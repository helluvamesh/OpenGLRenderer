#pragma once


class DBitmapImageHDRAllocator
{
public:
	static DBitmapImageHDRAllocator INSTANCE;

	float* Allocate(cint Size);

	void Deallocate(float* Array, cint Size);
};


class DBitmapImageHDR : public DStruct
{
	REFLECT_H(DBitmapImageHDR, DStruct)
private:
	DArray<float, DBitmapImageHDRAllocator> Buffer;
	int32 Width;
	int32 Height;
	int32 ChannelCount;

public:
	DBitmapImageHDR();
	DBitmapImageHDR(DArray<float, DBitmapImageHDRAllocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount);

	const DArray<float, DBitmapImageHDRAllocator>& GetBuffer() const;
	int32 GetWidth() const;
	int32 GetHeight() const;
	int32 GetChannelCount() const;

	void Reset();

	bool Load(DStringView Filepath, int32 WantedChannelCount = 0, float ExtraChannelValue = 1.0f, bool VFlip = false);
	bool Load(DArrayView<byte> Bytes, int32 WantedChannelCount = 0, float ExtraChannelValue = 1.0f, bool VFlip = false);

	bool AddChannels(int32 ExtraChannelCount, DBitmapImageHDR& Result, float Value = UINT16_MAX);
	bool RemoveChannels(int32 ChannelsToRemove, DBitmapImageHDR& Result);

	bool Resize(int32 NewWidth, int32 NewHeight, DBitmapImageHDR& Result);

	void OverwriteSize(int32 NewWidth, int32 NewHeight);

	bool WriteHDR(DStringView Filepath, bool VFilp = true);

};
