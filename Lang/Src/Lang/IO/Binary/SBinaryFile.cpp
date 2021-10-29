#include "pch.h"
#include "SBinaryFile.h"


bool SBinaryFile::ReadFile(DStringView Path, DArray<byte>& OutBytes)
{
	std::filesystem::path p = SPath::From(Path);
	std::basic_ifstream<byte> stream (p, std::ios_base::binary);
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + Path);
		return false;
	}
	else
	{
		cint fileSize = SCounterUtil::size_t_to_cint(std::filesystem::file_size(p));

		OutBytes.Reallocate(fileSize);
		auto& ret = stream.read(OutBytes.GetDataUnsafe(), fileSize);
		OutBytes.OverwriteLengthUnsafe(SCounterUtil::size_t_to_cint(ret.gcount()));
		stream.close();
		return true;
	}
}


bool SBinaryFile::WriteToFile(DStringView Path, DArrayView<byte> InBytes)
{
	std::filesystem::path p = SPath::From(Path);
	std::basic_ofstream<byte> stream (p, std::ios_base::binary);
	if (!stream.is_open() || stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + Path);
		return false;
	}
	else
	{
		stream.write(InBytes.Data(), InBytes.Length());
		stream.close();
		return true;
	}
}


// -------------------- BinaryReader --------------------


DBinaryReader::DBinaryReader(DStringView Path) :
	Stream(SPath::From(Path), std::ios_base::binary)
{
	if (!this->Stream.is_open() || this->Stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + Path);
		return;
	}
}


DBinaryReader::DBinaryReader(const std::filesystem::path& Path) :
	Stream(Path, std::ios_base::binary)
{
	if (!this->Stream.is_open() || this->Stream.fail())
	{
		SLog::Error(TX("Failed to open file for reading: ") + SPath::ToString(Path));
		return;
	}
}



DBinaryReader::~DBinaryReader()
{
	this->Stream.close();
}


bool DBinaryReader::ReadBytes(cint ByteCount, DArray<byte>& OutBytes)
{
	OutBytes.Reallocate(ByteCount);
	auto& ret = this->Stream.read(OutBytes.GetDataUnsafe(), ByteCount);
	cint bytesRead = SCounterUtil::size_t_to_cint(ret.gcount());
	OutBytes.OverwriteLengthUnsafe(bytesRead);
	return bytesRead == ByteCount;
}


bool DBinaryReader::ReadBytes(cint ByteCount, byte* OutBytes)
{
	auto& ret = this->Stream.read(OutBytes, ByteCount);
	cint bytesRead = SCounterUtil::size_t_to_cint(ret.gcount());
	return bytesRead == ByteCount;
}


void DBinaryReader::Close()
{
	this->Stream.close();
}


bool DBinaryReader::IsOpen()
{
	return this->Stream.is_open() && !this->Stream.fail();
}


// ------------------------ BinaryWriter ---------------------------


DBinaryWriter::DBinaryWriter(DStringView Path) :
	Stream(SPath::From(Path), std::ios_base::binary)
{
	if (!this->Stream.is_open() || this->Stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + Path);
		return;
	}
}


DBinaryWriter::DBinaryWriter(const std::filesystem::path& Path) :
	Stream(Path, std::ios_base::binary)
{
	if (!this->Stream.is_open() || this->Stream.fail())
	{
		SLog::Error(TX("Failed to open file for writing: ") + SPath::ToString(Path));
		return;
	}
}


DBinaryWriter::~DBinaryWriter()
{
	this->Stream.close();
}


void DBinaryWriter::WriteBytes(DArrayView<byte> Bytes)
{
	this->Stream.write(Bytes.Data(), Bytes.Length());
}


void DBinaryWriter::Close()
{
	this->Stream.close();
}


bool DBinaryWriter::IsOpen()
{
	return this->Stream.is_open() && !this->Stream.fail();
}
