#include "Engine/pch.h"
#include "SImageCompressor.h"
#include "Engine/Dependencies/STB/stb_dxt.h"


bool SImageCompressor::CompressToBC1_BC3(const DBitmapImage& Image, bool KeepAlpha, DBitmapImage& Result)
{
	if (Image.GetChannelCount() != 4)
	{
		SLog::Error(TX("Can only compress BitmapImage with RGBA8 format"));
		return false;
	}

	if ((Image.GetWidth() % 4 != 0) || (Image.GetHeight() % 4 != 0))
	{
		SLog::Error(TX("Texture compression requires dimensions that are multiple of 4"));
		return false;
	}

	int32 imageWidth = Image.GetWidth();
	int32 imageHeight = Image.GetHeight();

	int32 width = imageWidth * 4;
	int32 height = imageHeight;
	const uint8* buffer = Image.GetBuffer().Data();

	int32 resultLen;
	int32 resultChannelCount;
	if (KeepAlpha)
	{
		resultLen = imageWidth * imageHeight;
		resultChannelCount = 4;
	}
	else
	{
		resultLen = imageWidth * imageHeight / 2;
		resultChannelCount = 3;
	}

	DArray<uint8, DBitmapImageAllocator> resultBuffer (resultLen, true);
	uint8* result = resultBuffer.GetDataUnsafe();

	int32 resultProgress = 0;
	for (int32 r = 0; r < height; r += 4)
	{
		for (int32 rb = 0; rb < width; rb += 16)
		{
			uint8 block[64];
			int32 blockOff = 0;

			for (int32 blockRow = 0; blockRow < 4; blockRow++)
			{
				int32 off = width * (r + blockRow) + rb;

				for (int32 blockCol = 0; blockCol < 16; blockCol++)
				{
					block[blockOff + blockCol] = buffer[off + blockCol];
				}

				blockOff += 16;
			}

			stb_compress_dxt_block(/*out*/&result[resultProgress], &block[0], KeepAlpha, STB_DXT_HIGHQUAL);
			resultProgress += KeepAlpha? 16 : 8;
		}
	}

	Result = DBitmapImage(std::move(resultBuffer), Image.GetWidth(), Image.GetHeight(), resultChannelCount);

	return true;
}


bool SImageCompressor::CompressToBC1(const DBitmapImage& Image, DBitmapImage& Result)
{
	return CompressToBC1_BC3(Image, false, Result);
}


bool SImageCompressor::CompressToBC3(const DBitmapImage& Image, DBitmapImage& Result)
{
	return CompressToBC1_BC3(Image, true, Result);
}


bool SImageCompressor::CompressToBC4(const DBitmapImage& Image, DBitmapImage& Result)
{
	if (Image.GetChannelCount() != 4)
	{
		SLog::Error(TX("Can only compress BitmapImage with RGBA8 format"));
		return false;
	}

	if ((Image.GetWidth() % 4 != 0) || (Image.GetHeight() % 4 != 0))
	{
		SLog::Error(TX("Texture compression requires dimensions that are multiple of 4"));
		return false;
	}

	int32 imageWidth = Image.GetWidth();
	int32 imageHeight = Image.GetHeight();

	int32 width = imageWidth * 4;
	int32 height = imageHeight;
	const uint8* buffer = Image.GetBuffer().Data();
	
	int32 resultLen = resultLen = imageWidth * imageHeight / 2;
	int32 resultChannelCount = 1;

	DArray<uint8, DBitmapImageAllocator> resultBuffer (resultLen, true);
	uint8* result = resultBuffer.GetDataUnsafe();

	int32 resultProgress = 0;
	for (int32 r = 0; r < height; r += 4)
	{
		for (int32 rb = 0; rb < width; rb += 16)
		{
			uint8 block[16];
			int32 blockOff = 0;

			for (int32 blockRow = 0; blockRow < 4; blockRow++)
			{
				int32 off = width * (r + blockRow) + rb;

				for (int32 blockCol = 0; blockCol < 16; blockCol += 4)
				{
					block[blockOff++] = buffer[off + blockCol];
				}
			}

			stb_compress_bc4_block(/*out*/&result[resultProgress], &block[0]);
			resultProgress += 8;
		}
	}

	Result = DBitmapImage(std::move(resultBuffer), Image.GetWidth(), Image.GetHeight(), resultChannelCount);

	return true;
}


bool SImageCompressor::CompressToBC5(const DBitmapImage& Image, DBitmapImage& Result)
{
	if (Image.GetChannelCount() != 4)
	{
		SLog::Error(TX("Can only compress BitmapImage with RGBA8 format"));
		return false;
	}

	if ((Image.GetWidth() % 4 != 0) || (Image.GetHeight() % 4 != 0))
	{
		SLog::Error(TX("Texture compression requires dimensions that are multiple of 4"));
		return false;
	}

	int32 imageWidth = Image.GetWidth();
	int32 imageHeight = Image.GetHeight();

	int32 width = imageWidth * 4;
	int32 height = imageHeight;
	const uint8* buffer = Image.GetBuffer().Data();
	
	int32 resultLen = resultLen = imageWidth * imageHeight;
	int32 resultChannelCount = 2;
	
	DArray<uint8, DBitmapImageAllocator> resultBuffer (resultLen, true);
	uint8* result = resultBuffer.GetDataUnsafe();

	int32 resultProgress = 0;
	for (int32 r = 0; r < height; r += 4)
	{
		for (int32 rb = 0; rb < width; rb += 16)
		{
			uint8 block[32];
			int32 blockOff = 0;

			for (int32 blockRow = 0; blockRow < 4; blockRow++)
			{
				int32 off = width * (r + blockRow) + rb;

				for (int32 blockCol = 0; blockCol < 16; blockCol += 4)
				{
					block[blockOff++] = buffer[off + blockCol];
					block[blockOff++] = buffer[off + blockCol + 1];
				}
			}

			stb_compress_bc5_block(/*out*/&result[resultProgress], &block[0]);
			resultProgress += 16;
		}
	}

	Result = DBitmapImage(std::move(resultBuffer), Image.GetWidth(), Image.GetHeight(), resultChannelCount);

	return true;
}
