#pragma once


class SPath
{
public:
	static const DString INVALID_FILENAME_CHARS;

	static bool SplitToBaseAndExtension(DStringView Path, DString& OutBase, DString& OutExtension);

	static DString GetExtension(DStringView Path);

	static void RemoveExtension(DString& Path);

	static DString GetDirectory(DStringView Path);

	static std::filesystem::path From(DStringView Str);

	static DString ToString(const std::filesystem::path& Path);

	// Replaces chars in Paths::INVALID_FILENAME_CHARS with '_'
	static DString MakeSafeFilename(DStringView Filename);

};