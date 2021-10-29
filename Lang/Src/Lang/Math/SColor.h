#pragma once


class SColor
{
public:

	static FVec4 Saturate(const FVec4& Color, float Value);
	static float Brightness(const FVec4& Color);

};
