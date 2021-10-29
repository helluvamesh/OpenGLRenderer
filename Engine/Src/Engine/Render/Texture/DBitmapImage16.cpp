#include "Engine/pch.h"
#include "DBitmapImage16.h"
#include "Engine/Dependencies/STB/stb_image.h"
#include "Engine/Dependencies/STB/stb_image_resize.h"


DBitmapImage16Allocator DBitmapImage16Allocator::INSTANCE;


uint16* DBitmapImage16Allocator::Allocate(cint Size)
{
	return (uint16*)malloc(sizeof(uint16) * Size);
}

void DBitmapImage16Allocator::Deallocate(uint16* Array, cint Size)
{
	stbi_image_free(Array);
}


DBitmapImage16::DBitmapImage16() : 
	Width(0),
	Height(0),
	ChannelCount(0)
{
}


DBitmapImage16::DBitmapImage16(DArray<uint16, DBitmapImage16Allocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount) :
	Buffer(std::forward<DArray<uint16, DBitmapImage16Allocator>>(InBuffer)),
	Width(InWidth),
	Height(InHeight),
	ChannelCount(InChannelCount)
{
}


const DArray<uint16, DBitmapImage16Allocator>& DBitmapImage16::GetBuffer() const
{
	return this->Buffer;
}


int32 DBitmapImage16::GetWidth() const
{
	return this->Width;
}


int32 DBitmapImage16::GetHeight() const
{
	return this->Height;
}


int32 DBitmapImage16::GetChannelCount() const
{
	return this->ChannelCount;
}


void DBitmapImage16::Reset()
{
	this->Buffer.Clear();
	this->Width = this->Height = this->ChannelCount = 0;
}


bool DBitmapImage16::Load(DStringView Filepath, int32 WantedChannelCount, uint16 ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	std::filesystem::path path = SPath::From(Filepath);
	std::string s = path.string();
	
	uint16* buff = stbi_load_16(s.c_str(), &this->Width, &this->Height, &this->ChannelCount, 0);

	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image: ") + Filepath);
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<uint16, DBitmapImage16Allocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount > 0)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImage16 result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImage16 result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImage16::Load(const DArrayView<byte> Bytes, int32 WantedChannelCount, uint16 ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	uint16* buff = stbi_load_16_from_memory(Bytes.Data(), Bytes.Length(), &this->Width, &this->Height, &this->ChannelCount, 0);

	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image from byte array"));
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<uint16, DBitmapImage16Allocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount > 0)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImage16 result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImage16 result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImage16::AddChannels(int32 ExtraChannelCount, DBitmapImage16& Result, uint16 Value)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount + ExtraChannelCount;
	int32 newElemCount = this->Width * this->Height * newChannelCount;

	Result.Buffer.Reserve(newElemCount);
	uint16* newBuffer = Result.Buffer.GetDataUnsafe();
	Result.Width = this->Width;
	Result.Height = this->Height;
	Result.ChannelCount = newChannelCount;

	int32 newBufferProgress = 0;
	for (int32 i = 0; i < elemCount; i += this->ChannelCount)
	{
		for (int32 ch_i = 0; ch_i < this->ChannelCount; ch_i++)
		{
			newBuffer[newBufferProgress + ch_i] = this->Buffer[i + ch_i];
		}

		int32 offset = newBufferProgress + this->ChannelCount;
		for (int32 ch_i = 0; ch_i < ExtraChannelCount; ch_i++)
		{
			newBuffer[offset + ch_i] = Value;
		}

		newBufferProgress += newChannelCount;
	}

	return true;
}


bool DBitmapImage16::RemoveChannels(int32 ChannelsToRemove, DBitmapImage16& Result)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount - ChannelsToRemove;
	int32 newElemCount = this->Width * this->Height * newChannelCount;

	Result.Buffer.Reserve(newElemCount);
	uint16* newBuffer = Result.Buffer.GetDataUnsafe();
	Result.Width = this->Width;
	Result.Height = this->Height;
	Result.ChannelCount = newChannelCount;

	int32 newBufferProgress = 0;
	for (int32 i = 0; i < elemCount; i += this->ChannelCount)
	{
		for (int32 ch_i = 0; ch_i < newChannelCount; ch_i++)
		{
			newBuffer[newBufferProgress + ch_i] = this->Buffer[i + ch_i];
		}

		newBufferProgress += newChannelCount;
	}

	return true;
}


bool DBitmapImage16::Resize(int32 NewWidth, int32 NewHeight, bool sRGB, DBitmapImage16& Result)
{
	int32 newLen = NewWidth * NewHeight * this->ChannelCount;
	Result.Buffer.Reallocate(newLen, true);
	uint16* result = Result.Buffer.GetDataUnsafe();
	Result.Width = NewWidth;
	Result.Height = NewHeight;
	Result.ChannelCount = this->ChannelCount;
	
	uint16* acxt = nullptr;
	return stbir_resize_uint16_generic(
		this->Buffer.GetDataUnsafe(), this->Width, this->Height, 0, 
		result, NewWidth, NewHeight, 0, 
		this->ChannelCount, this->ChannelCount == 4? 3 : 0, 0, stbir_edge::STBIR_EDGE_CLAMP,
		stbir_filter::STBIR_FILTER_DEFAULT, stbir_colorspace::STBIR_COLORSPACE_LINEAR, acxt
	);
}


void DBitmapImage16::OverwriteSize(int32 NewWidth, int32 NewHeight)
{
	this->Width = NewWidth;
	this->Height = NewHeight;
}


REFLECT(DBitmapImage16)
FIELD(Buffer)
FIELD(Width)
FIELD(Height)
FIELD(ChannelCount)
REFLECT_END()
