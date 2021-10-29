#include "pch.h"
#include "SPath.h"


const DString SPath::INVALID_FILENAME_CHARS = TX("/\\:*?\"<>|.,=&%#@^\n\t\r\b\f");


bool SPath::SplitToBaseAndExtension(DStringView Path, DString& OutBase, DString& OutExtension)
{
	cint len = Path.Length();
	for (cint i = len - 1; i >= 0 ; i--)
	{
		tchar c = Path[i];
		if (c == TX('.') && i < len - 1)
		{
			OutBase = DString::SubstrRange(Path, 0, i);
			OutExtension = DString::Substr(Path, i + 1);
			return true;
		}
	}
	return false;
}


DString SPath::GetExtension(DStringView Path)
{
	cint len = Path.Length();
	for (cint i = len - 1; i >= 0 ; i--)
	{
		tchar c = Path[i];
		if (c == TX('.') && i < len - 1)
		{
			return DString::Substr(Path, i + 1);
		}
	}
	return TX("");
}


void SPath::RemoveExtension(DString& Path)
{
	cint len = Path.Length();
	for (cint i = len - 1; i >= 0 ; i--)
	{
		tchar c = Path[i];
		if (c == TX('.') && i < len - 1)
		{
			Path.Resize(i);
			return;
		}
	}
}


DString SPath::GetDirectory(DStringView Path)
{
	cint len = Path.Length();
	for (cint i = len - 1; i >=0 ; i--)
	{
		tchar c = Path[i];
		if (c == TX('/') || c == TX('\\'))
		{
			return DString::SubstrRange(Path, 0, i + 1);
		}
	}
	return TX("");
}


std::filesystem::path SPath::From(DStringView Str)
{
	return std::filesystem::path(Str.Data(), Str.Data() + Str.Length());
}


DString SPath::ToString(const std::filesystem::path& Path)
{
	std::u16string str = Path.u16string();
	if (str.length() > 0)
		return DString(&str[0], (cint)str.length());
	else
		return DString();
}


DString SPath::MakeSafeFilename(DStringView Filename)
{
	if (Filename.Length() == 0)
	{
		return TX("_");
	}
	DString result;
	cint len = Filename.Length();
	result.Reserve(len);
	for (cint i = 0; i < len; ++i)
	{
		tchar c = Filename[i];
		if (DString::Contains(INVALID_FILENAME_CHARS, DStringView(&c, 1)))
			result.Append(TX('_'));
		else
			result.Append(c);
	}
	return result;
}
