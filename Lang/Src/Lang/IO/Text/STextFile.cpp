#include "pch.h"
#include "STextFile.h"
#include "fstream"


/*
	std::ifstream
	- get: reads until a line separator (separator included in result)
	- getline: reads until a line separator (separator not included in result)
	- read/readsome: reads number of bytes
*/


std::ios_base::openmode STextFile::FileWriteModeToCppOpenMode(EFileWriteMode V)
{
	switch (V)
	{
	case EFileWriteMode::OVERWRITE:
		return std::ios_base::out;

	case EFileWriteMode::APPEND:
		return std::ios_base::app;
	default:
		return std::ios_base::out;
	}
}


bool STextFile::SkipUTF8BOM(std::ifstream& Stream, DString8& BytesRead)
{
	BytesRead.Reserve(3);
	std::istream& streampos = Stream.read(BytesRead.GetDataUnsafe(), 3);
	BytesRead.OverwriteLengthUnsafe(SCounterUtil::size_t_to_cint(streampos.gcount()));
	
	return 
		BytesRead.Length() == 3 && 
		BytesRead[0] == (char)0xEF && 
		BytesRead[1] == (char)0xBB && 
		BytesRead[2] == (char)0xBF;
}


bool STextFile::ReadAsciiFile(DStringView Path, DString8& Result)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ifstream stream (p);
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + Path);
		return false;
	}
	else
	{
		cint fileSize = SCounterUtil::size_t_to_cint(std::filesystem::file_size(p));
		
		Result.Reallocate(fileSize);
		std::istream& s = stream.read(Result.GetDataUnsafe(), fileSize);
		stream.close();
		Result.OverwriteLengthUnsafe(SCounterUtil::size_t_to_cint(s.gcount()));
		return true;
	}
}


bool STextFile::ReadUTF8File(DStringView Path, DString8& Result, EUnicodeBOMReadMode BOM_Mode)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ifstream stream (p);
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + Path);
		return false;
	}
	else
	{
		cint fileSize = SCounterUtil::size_t_to_cint(std::filesystem::file_size(p));

		Result.Clear();

		if (BOM_Mode == EUnicodeBOMReadMode::DETECT_BOM)
		{
			DString8 bytes;
			bool foundBOM = SkipUTF8BOM(stream, bytes);
			if (foundBOM == false)
			{
				Result.Append(bytes);
			}
		}

		cint BOM_bytes = Result.Length();

		Result.Reserve(fileSize);
		std::istream& streampos = stream.read(Result.GetDataUnsafe() + BOM_bytes, fileSize);
		stream.close();
		Result.OverwriteLengthUnsafe(SCounterUtil::size_t_to_cint(streampos.gcount() + BOM_bytes));

		return true;
	}
}


bool STextFile::ReadUTF8File(DStringView Path, DString& Result, EUnicodeBOMReadMode BOM_Mode)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ifstream stream (p);
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + Path);
		return false;
	}
	else
	{
		cint fileSize = SCounterUtil::size_t_to_cint(std::filesystem::file_size(p));
		
		DString8 text;
		
		if (BOM_Mode == EUnicodeBOMReadMode::DETECT_BOM)
		{
			DString8 bytes;
			bool foundBOM = SkipUTF8BOM(stream, bytes);
			if (foundBOM == false)
			{
				text.Append(bytes);
			}
		}

		cint BOM_bytes = text.Length();

		text.Reserve(fileSize);
		std::istream& streampos = stream.read(text.GetDataUnsafe() + BOM_bytes, fileSize);
		stream.close();
		text.OverwriteLengthUnsafe(SCounterUtil::size_t_to_cint(streampos.gcount() + BOM_bytes));

		Result = DStrings::UTF8ToUTF16(text);
		return true;
	}
}


bool STextFile::WriteToAsciiFile(DStringView Path, DStringView8 Text, EFileWriteMode WriteMode)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ofstream stream (p, FileWriteModeToCppOpenMode(WriteMode));
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + Path);
		return false;
	}
	else
	{
		stream.write(Text.Data(), Text.Length());
		stream.close();
		return true;
	}
}


bool STextFile::WriteToUTF8File(
	DStringView Path, DStringView Text, 
	EFileWriteMode WriteMode, EUnicodeBOMWriteMode BOM_Mode
)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ofstream stream (p, FileWriteModeToCppOpenMode(WriteMode));
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + Path);
		return false;
	}
	else
	{
		if (BOM_Mode == EUnicodeBOMWriteMode::ADD_BOM)
		{
			char bom[] { (char)0xEF, (char)0xBB, (char)0xBF };
			stream.write(&bom[0], 3);
		}

		DString8 u8Text = DStrings::UTF16ToUTF8(Text);
		stream.write(u8Text.Data(), u8Text.Length());
		stream.close();
		return true;
	}
}



bool STextFile::WriteToUTF8File(
	DStringView Path, DStringView8 Text, 
	EFileWriteMode WriteMode, EUnicodeBOMWriteMode BOM_Mode
)
{
	std::filesystem::path p (Path.Data(), Path.Data() + Path.Length());
	std::ofstream stream (p, FileWriteModeToCppOpenMode(WriteMode));
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + Path);
		return false;
	}
	else
	{
		if (BOM_Mode == EUnicodeBOMWriteMode::ADD_BOM)
		{
			char bom[] { (char)0xEF, (char)0xBB, (char)0xBF };
			stream.write(&bom[0], 3);
		}

		stream.write(Text.Data(), Text.Length());
		stream.close();
		return true;
	}
}