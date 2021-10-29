#include "Engine/pch.h"
#include "DBitmapImage.h"
#include "Engine/Dependencies/STB/stb_image.h"
#include "Engine/Dependencies/STB/stb_image_resize.h"
#include "Engine/Dependencies/STB/stb_image_write.h"


DBitmapImageAllocator DBitmapImageAllocator::INSTANCE;


uint8* DBitmapImageAllocator::Allocate(cint Size)
{
	return (uint8*)malloc(sizeof(uint8) * Size);
}

void DBitmapImageAllocator::Deallocate(uint8* Array, cint Size)
{
	stbi_image_free(Array);
}


DBitmapImage::DBitmapImage() : 
	Width(0),
	Height(0),
	ChannelCount(0)
{
}


DBitmapImage::DBitmapImage(DArray<uint8, DBitmapImageAllocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount) :
	Buffer(std::forward<DArray<uint8, DBitmapImageAllocator>>(InBuffer)),
	Width(InWidth),
	Height(InHeight),
	ChannelCount(InChannelCount)
{
}


const DArray<uint8, DBitmapImageAllocator>& DBitmapImage::GetBuffer() const
{
	return this->Buffer;
}


int32 DBitmapImage::GetWidth() const
{
	return this->Width;
}


int32 DBitmapImage::GetHeight() const
{
	return this->Height;
}


int32 DBitmapImage::GetChannelCount() const
{
	return this->ChannelCount;
}


void DBitmapImage::Reset()
{
	this->Buffer.Clear();
	this->Width = this->Height = this->ChannelCount = 0;
}


bool DBitmapImage::Load(DStringView Filepath, int32 WantedChannelCount, uint8 ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	std::filesystem::path path = SPath::From(Filepath);
	std::string s = path.string();
	
	uint8* buff = stbi_load(s.c_str(), &this->Width, &this->Height, &this->ChannelCount, 0);
	
	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image: ") + Filepath);
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<uint8, DBitmapImageAllocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount > 0)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImage result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImage result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImage::Load(DArrayView<byte> Bytes, int32 WantedChannelCount, uint8 ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	uint8* buff = stbi_load_from_memory(Bytes.Data(), Bytes.Length(), &this->Width, &this->Height, &this->ChannelCount, 0);
	
	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image from byte array"));
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<uint8, DBitmapImageAllocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount > 0)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImage result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImage result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImage::AddChannels(int32 ExtraChannelCount, DBitmapImage& Result, uint8 Value)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount + ExtraChannelCount;
	int32 newElemCount = this->Width * this->Height * newChannelCount;

	Result.Buffer.Reallocate(newElemCount, true);
	uint8* newBuffer = Result.Buffer.GetDataUnsafe();
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


bool DBitmapImage::RemoveChannels(int32 ChannelsToRemove, DBitmapImage& Result)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount - ChannelsToRemove;
	int32 newElemCount = this->Width * this->Height * newChannelCount;
	
	Result.Buffer.Reallocate(newElemCount, true);
	uint8* newBuffer = Result.Buffer.GetDataUnsafe();
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


bool DBitmapImage::Resize(int32 NewWidth, int32 NewHeight, bool sRGB, DBitmapImage& Result)
{
	int32 newLen = NewWidth * NewHeight * this->ChannelCount;
	Result.Buffer.Reallocate(newLen, true);
	uint8* result = Result.Buffer.GetDataUnsafe();
	Result.Width = NewWidth;
	Result.Height = NewHeight;
	Result.ChannelCount = this->ChannelCount;

	if (sRGB)
	{ 
		return stbir_resize_uint8_srgb(
			this->Buffer.GetDataUnsafe(), this->Width, this->Height, 0, 
			result, NewWidth, NewHeight, 0, 
			this->ChannelCount, this->ChannelCount == 4? 3 : 0, 0
		);
	}
	else
	{
		return stbir_resize_uint8(
			this->Buffer.GetDataUnsafe(), this->Width, this->Height, 0, 
			result, NewWidth, NewHeight, 0, this->ChannelCount
		);
	}
}


void DBitmapImage::OverwriteSize(int32 NewWidth, int32 NewHeight)
{
	this->Width = NewWidth;
	this->Height = NewHeight;
}


bool DBitmapImage::WritePNG(DStringView Filepath, bool VFlip)
{
	std::filesystem::path path = SPath::From(Filepath);
	std::string s = path.string();

	stbi_flip_vertically_on_write(VFlip);

	return stbi_write_png(
		s.c_str(), this->Width, this->Height, this->ChannelCount, this->Buffer.Data(), 
		/*bytes in a row*/ (int32)sizeof(uint8) * this->Width * this->ChannelCount
	);
}


bool DBitmapImage::InvertChannel(int32 ChannelIndex)
{
	if (ChannelIndex < 0 || ChannelIndex >= this->ChannelCount)
	{
		SLog::Error("Invalid ChannelIndex while inverting channel of image");
		return false;
	}

	int32 len = this->Buffer.Length();
	uint8* buffer = this->Buffer.GetDataUnsafe();
	for (int32 i = ChannelIndex; i < len; i += this->ChannelCount)
	{
		buffer[i] = 255 - buffer[i];
	}

	return true;
}


REFLECT(DBitmapImage)
FIELD(Buffer)
FIELD(Width)
FIELD(Height)
FIELD(ChannelCount)
REFLECT_END()
