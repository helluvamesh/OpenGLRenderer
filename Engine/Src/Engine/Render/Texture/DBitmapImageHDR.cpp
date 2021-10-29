#include "Engine/pch.h"
#include "DBitmapImageHDR.h"
#include "Engine/Dependencies/STB/stb_image.h"
#include "Engine/Dependencies/STB/stb_image_resize.h"
#include "Engine/Dependencies/STB/stb_image_write.h"


DBitmapImageHDRAllocator DBitmapImageHDRAllocator::INSTANCE;


float* DBitmapImageHDRAllocator::Allocate(cint Size)
{
	return (float*)malloc(sizeof(float) * Size);
}

void DBitmapImageHDRAllocator::Deallocate(float* Array, cint Size)
{
	stbi_image_free(Array);
}


DBitmapImageHDR::DBitmapImageHDR() : 
	Width(0),
	Height(0),
	ChannelCount(0)
{
}


DBitmapImageHDR::DBitmapImageHDR(DArray<float, DBitmapImageHDRAllocator>&& InBuffer, int32 InWidth, int32 InHeight, int32 InChannelCount) :
	Buffer(std::forward<DArray<float, DBitmapImageHDRAllocator>>(InBuffer)),
	Width(InWidth),
	Height(InHeight),
	ChannelCount(InChannelCount)
{
}


const DArray<float, DBitmapImageHDRAllocator>& DBitmapImageHDR::GetBuffer() const
{
	return this->Buffer;
}


int32 DBitmapImageHDR::GetWidth() const
{
	return this->Width;
}


int32 DBitmapImageHDR::GetHeight() const
{
	return this->Height;
}


int32 DBitmapImageHDR::GetChannelCount() const
{
	return this->ChannelCount;
}


void DBitmapImageHDR::Reset()
{
	this->Buffer.Clear();
	this->Width = this->Height = this->ChannelCount = 0;
}


bool DBitmapImageHDR::Load(DStringView Filepath, int32 WantedChannelCount, float ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	std::filesystem::path path = SPath::From(Filepath);
	std::string s = path.string();

	float* buff = stbi_loadf(s.c_str(), &this->Width, &this->Height, &this->ChannelCount, 0);

	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image: ") + Filepath);
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<float, DBitmapImageHDRAllocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImageHDR result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImageHDR result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImageHDR::Load(DArrayView<byte> Bytes, int32 WantedChannelCount, float ExtraChannelValue, bool VFlip)
{
	this->Reset();

	stbi_set_flip_vertically_on_load(VFlip);

	float* buff = stbi_loadf_from_memory(Bytes.Data(), Bytes.Length(), &this->Width, &this->Height, &this->ChannelCount, 0);

	if (buff == nullptr)
	{
		SLog::Error(TX("Failed to load image from byte array"));
		return false;
	}

	int32 len = this->Width * this->Height * this->ChannelCount;
	this->Buffer = DArray<float, DBitmapImageHDRAllocator>::TakeOverBufferUnsafe(buff, len, len);

	if (WantedChannelCount)
	{
		if (WantedChannelCount < this->ChannelCount)
		{
			DBitmapImageHDR result;
			this->RemoveChannels(this->ChannelCount - WantedChannelCount, result);
			*this = result;
		}
		else if (WantedChannelCount > this->ChannelCount)
		{
			DBitmapImageHDR result;
			this->AddChannels(WantedChannelCount - this->ChannelCount, result, ExtraChannelValue);
			*this = result;
		}
	}

	return true;
}


bool DBitmapImageHDR::AddChannels(int32 ExtraChannelCount, DBitmapImageHDR& Result, float Value)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount + ExtraChannelCount;
	int32 newElemCount = this->Width * this->Height * newChannelCount;
	
	Result.Buffer.Reallocate(newElemCount, true);
	float* newBuffer = Result.Buffer.GetDataUnsafe();
	Result.Width = this->Width;
	Result.Height = this->Height;
	this->ChannelCount = newChannelCount;

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


bool DBitmapImageHDR::RemoveChannels(int32 ChannelsToRemove, DBitmapImageHDR& Result)
{
	int32 elemCount = this->Width * this->Height * this->ChannelCount;
	int32 newChannelCount = this->ChannelCount - ChannelsToRemove;
	int32 newElemCount = this->Width * this->Height * newChannelCount;

	Result.Buffer.Reallocate(newElemCount, true);
	float* newBuffer = Result.Buffer.GetDataUnsafe();
	Result.Width = this->Width;
	Result.Height = this->Height;
	this->ChannelCount = newChannelCount;

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


bool DBitmapImageHDR::Resize(int32 NewWidth, int32 NewHeight, DBitmapImageHDR& Result)
{
	int32 newLen = NewWidth * NewHeight * this->ChannelCount;
	Result.Buffer.Reallocate(newLen, true);
	float* result = Result.Buffer.GetDataUnsafe();
	Result.Width = NewWidth;
	Result.Height = NewHeight;
	Result.ChannelCount = this->ChannelCount;

	return stbir_resize_float(
		this->Buffer.GetDataUnsafe(), this->Width, this->Height, 0, 
		result, NewWidth, NewHeight, 0, this->ChannelCount
	);
}


void DBitmapImageHDR::OverwriteSize(int32 NewWidth, int32 NewHeight)
{
	this->Width = NewWidth;
	this->Height = NewHeight;
}


bool DBitmapImageHDR::WriteHDR(DStringView Filepath, bool VFlip)
{
	stbi_flip_vertically_on_write(VFlip);

	std::filesystem::path path = SPath::From(Filepath);
	std::string s = path.string();
	
	return stbi_write_hdr(s.c_str(), this->Width, this->Height, this->ChannelCount, this->Buffer.Data());
}


REFLECT(DBitmapImageHDR)
FIELD(Buffer)
FIELD(Width)
FIELD(Height)
FIELD(ChannelCount)
REFLECT_END()
