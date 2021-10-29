#include "pch.h"
#include "SColor.h"


FVec4 SColor::Saturate(const FVec4& Color, float Value)
{
	FVec3 grayXfer (.3f, .59f, .11f);
	FVec3 color (Color.X, Color.Y, Color.Z);
	FVec3 gray = FVec3(grayXfer.Dot(color));
	return FVec4(
		FMath::Lerp(gray.X, color.X, Value),
		FMath::Lerp(gray.Y, color.Y, Value),
		FMath::Lerp(gray.Z, color.Z, Value),
		Color.W
	);
}


float SColor::Brightness(const FVec4& Color)
{
	return FVec3(Color.X, Color.Y, Color.Z).Dot(FVec3(0.2126f, 0.7152f, 0.0722f));
}
