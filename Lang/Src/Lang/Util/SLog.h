#pragma once


class SLog
{

public:

	static void Print(DStringFormatArg Arg, DStringView Extra = TX("\n"));
	static void PrintList(const DArray<DStringFormatArg>& Args, DStringView Separator = TX(" "));
	
	static void Message(DStringView Str);
	static void Message(DStringView8 Str);
	
	static void Error(DStringView Str);
	static void Error(DStringView8 Str);
};
