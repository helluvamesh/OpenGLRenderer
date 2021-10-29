#pragma once


class DBitmapImageAllocator
{
public:
	static DBitmapImageAllocator INSTANCE;

	uint8* Allocate(cint Size);

	void Deallocate(uint8* Array, cint Size);
};


class DBitmapImage : public DStruct
{
	REFLECT_H(DBitmapImage, DStruct)
private:
	DArray<uint8, DBitmapImageAllocator> Buffer;
	int32 Width;
	int32 Height;
	int32 ChannelCount;

public:
	DBitmapImage();
	DBitmapImage(DArray<uint8, DBitmapImageAllocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount);

	const DArray<uint8, DBitmapImageAllocator>& GetBuffer() const;
	int32 GetWidth() const;
	int32 GetHeight() const;
	int32 GetChannelCount() const;

	void Reset();
	
	bool Load(DStringView Filepath, int32 WantedChannelCount = 0, uint8 ExtraChannelValue = 255, bool VFlip = false);
	bool Load(DArrayView<byte> Bytes, int32 WantedChannelCount = 0, uint8 ExtraChannelValue = 255, bool VFlip = false);

	bool AddChannels(int32 ExtraChannelCount, DBitmapImage& Result, uint8 Value = 255);
	bool RemoveChannels(int32 ChannelsToRemove, DBitmapImage& Result);

	bool Resize(int32 NewWidth, int32 NewHeight, bool sRGB, DBitmapImage& Result);

	void OverwriteSize(int32 NewWidth, int32 NewHeight);

	bool WritePNG(DStringView Filepath, bool VFlip = true);

	bool InvertChannel(int32 ChannelIndex);

};