#include "pch.h"


void SLog::Print(DStringFormatArg Arg, DStringView Extra)
{
	DStringView str = Arg.GetString();
	std::cout << DStrings::UTF16ToAscii(str).CString();
	std::cout << DStrings::UTF16ToAscii(Extra).CString() << std::endl;
}


void SLog::PrintList(const DArray<DStringFormatArg>& Args, DStringView Separator)
{
	DString res;

	for (cint i = 0; i < Args.Length(); i++)
	{
		res.Append(Args[i].GetString());
		if (i < Args.Length() - 1)
		{
			res.Append(Separator);
		}
	}

	std::cout << DStrings::UTF16ToAscii(res).CString() << std::endl;
}


void SLog::Message(DStringView Str)
{
	std::cout << DStrings::UTF16ToAscii(Str).CString() << std::endl;
}


void SLog::Message(DStringView8 Str)
{
	std::cout << Str.Data() << std::endl;
}


void SLog::Error(DStringView Str)
{
	std::cout << DStrings::UTF16ToAscii(Str).CString() << std::endl;
}


void SLog::Error(DStringView8 Str)
{
	std::cout << Str.Data() << std::endl;
}