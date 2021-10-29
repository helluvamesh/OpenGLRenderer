#pragma once


class DBitmapImage16Allocator
{
public:
	static DBitmapImage16Allocator INSTANCE;

	uint16* Allocate(cint Size);

	void Deallocate(uint16* Array, cint Size);
};


class DBitmapImage16 : public DStruct
{
	REFLECT_H(DBitmapImage16, DStruct)
private:
	DArray<uint16, DBitmapImage16Allocator> Buffer;
	int32 Width;
	int32 Height;
	int32 ChannelCount;

public:
	DBitmapImage16();
	DBitmapImage16(DArray<uint16, DBitmapImage16Allocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount);

	const DArray<uint16, DBitmapImage16Allocator>& GetBuffer() const;
	int32 GetWidth() const;
	int32 GetHeight() const;
	int32 GetChannelCount() const;

	void Reset();

	bool Load(DStringView Filepath, int32 WantedChannelCount = 0, uint16 ExtraChannelValue = UINT16_MAX, bool VFlip = false);
	bool Load(DArrayView<byte> Bytes, int32 WantedChannelCount = 0, uint16 ExtraChannelValue = UINT16_MAX, bool VFlip = false);

	bool AddChannels(int32 ExtraChannelCount, DBitmapImage16& Result, uint16 Value = UINT16_MAX);
	bool RemoveChannels(int32 ChannelsToRemove, DBitmapImage16& Result);

	bool Resize(int32 NewWidth, int32 NewHeight, bool sRGB, DBitmapImage16& Result);

	void OverwriteSize(int32 NewWidth, int32 NewHeight);

};