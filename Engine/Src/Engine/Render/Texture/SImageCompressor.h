#pragma once

#include "Engine\Render\Texture\DBitmapImage.h"


class SImageCompressor
{
private:
	static bool CompressToBC1_BC3(const DBitmapImage& Image, bool KeepAlpha, DBitmapImage& Result);

public:
	static bool CompressToBC1(const DBitmapImage& Image, DBitmapImage& Result);
	static bool CompressToBC3(const DBitmapImage& Image, DBitmapImage& Result);
	static bool CompressToBC4(const DBitmapImage& Image, DBitmapImage& Result);
	static bool CompressToBC5(const DBitmapImage& Image, DBitmapImage& Result);

};