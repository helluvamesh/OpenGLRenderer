#pragma once

#include <fstream>


class SBinaryFile
{
public:

	static bool ReadFile(DStringView Path, DArray<byte>& OutBytes);

	static bool WriteToFile(DStringView Path, DArrayView<byte> Bytes);

};


/*
* Only meant to be used to read chunks of bytes, then use a serializer/converter on them
*/
class DBinaryReader
{
private:
	std::basic_ifstream<byte> Stream;

public:
	DBinaryReader(DStringView Path);
	DBinaryReader(const std::filesystem::path& Path);
	~DBinaryReader();

	bool ReadBytes(cint ByteCount, DArray<byte>& OutBytes);
	bool ReadBytes(cint ByteCount, byte* OutBytes);
	
	void Close();
	bool IsOpen();

};


class DBinaryWriter
{
private:
	std::basic_ofstream<byte> Stream;

public:
	DBinaryWriter(DStringView Path);
	DBinaryWriter(const std::filesystem::path& Path);
	~DBinaryWriter();

	// Don't forget to close!
	void WriteBytes(DArrayView<byte> Bytes);

	void Close();
	bool IsOpen();

};
