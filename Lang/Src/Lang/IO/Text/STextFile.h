#pragma once


class STextFile
{
public:

	enum class EUnicodeBOMReadMode : uint8 { DETECT_BOM, DO_NOT_DETECT_BOM };

	enum class EUnicodeBOMWriteMode : uint8 { NO_BOM, ADD_BOM };
	
	enum class EFileWriteMode : uint8 { OVERWRITE, APPEND };

private:

	static std::ios_base::openmode FileWriteModeToCppOpenMode(EFileWriteMode V);

	/* returns whether BOM was found */
	static bool SkipUTF8BOM(std::ifstream& Stream, DString8& BytesRead);

public:
	
	static bool ReadAsciiFile(DStringView Path, DString8& Result);

	// read utf-8 file to utf-16 string
	static bool ReadUTF8File(
		DStringView Path, DString& Result, 
		EUnicodeBOMReadMode BOM_Mode = EUnicodeBOMReadMode::DETECT_BOM
	);

	// read utf-8 file to utf-8 string
	static bool ReadUTF8File(
		DStringView Path, DString8& Result, 
		EUnicodeBOMReadMode BOM_Mode = EUnicodeBOMReadMode::DETECT_BOM
	);
	
	static bool WriteToAsciiFile(
		DStringView Path, DStringView8 Text, 
		EFileWriteMode WriteMode = EFileWriteMode::OVERWRITE
	);
	
	// write utf-16 string to utf-8 file
	static bool WriteToUTF8File(
		DStringView Path, DStringView Text,
		EFileWriteMode WriteMode = EFileWriteMode::OVERWRITE, 
		EUnicodeBOMWriteMode BOM_Mode = EUnicodeBOMWriteMode::NO_BOM
	);

	// write utf-8 string to utf-8 file
	static bool WriteToUTF8File(
		DStringView Path, DStringView8 Text,
		EFileWriteMode WriteMode = EFileWriteMode::OVERWRITE, 
		EUnicodeBOMWriteMode BOM_Mode = EUnicodeBOMWriteMode::NO_BOM
	);

};