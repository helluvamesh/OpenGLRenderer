/*_________________________________________________________________________________

DString8
- dynamic (resizable) string
- stores short strings on the stack
- no specific encoding, 8-bit char

_________________________________________________________________________________*/

#pragma once


#define DSTRING8_INTERNAL_BUFFER_BYTES 32


class DString8 : public _DStringBase<char8, DSTRING8_INTERNAL_BUFFER_BYTES, DArrayAllocator<char8>>
{
public:

	static const DString8 EMPTY;

	// ALL NON-STATIC FIELDS MUST BE DECLARED IN _DStringBase!

	/*___________________________

	CONSTRUCT
	___________________________*/

	DString8();

	// InCapacity: null terminator is not included
	// OverwriteLength = true results in an uninitialized array - Unsafe
	explicit DString8(cint InCapacity, bool OverwriteLength = false);

	DString8(const char8* InCString);

	DString8(const char8* InBuffer, cint InLength);

	static DString8 From(DStringView8 Other);

public:

	/*___________________________

	COPY
	___________________________*/

	DString8(const DString8& Other);

	DString8& operator=(const DString8& Other);

	/*___________________________

	MOVE
	___________________________*/

	DString8(DString8&& Other) noexcept;

	DString8& operator=(DString8&& Other) noexcept;

	/*___________________________

	DESTRUCT
	___________________________*/

	~DString8();

	/*___________________________

	MANIPULATION
	___________________________*/

	char8& operator[](cint Index);

	const char8& operator[](cint Index) const;

	char8& At(cint Index);

	const char8& At(cint Index) const;

	bool Equals(DStringView8 Other) const;

	bool operator==(DStringView8 Other) const;

	bool operator!=(DStringView8 Other) const;

	DString8& operator+=(DStringView8 Other);

	void Append(DStringView8 Buffer);

	void Append(const std::string::const_iterator& Start, const std::string::const_iterator& Stop);

	void Append(char8 Char);

	void Prepend(DStringView8 Buffer);

	void Prepend(char8 Char);

	cint IndexOf(DStringView8 Search, cint Start = 0, cint Stop = -1) const;
	static cint IndexOf(DStringView8 Source, DStringView8 Search, cint Start = 0, cint Stop = -1);

	cint LastIndexOf(DStringView8 Search, cint Start = -1, cint Stop = 0) const;
	static cint LastIndexOf(DStringView8 Source, DStringView8 Search, cint Start = -1, cint Stop = 0);

	bool StartsWith(DStringView8 Prefix) const;
	static bool StartsWith(DStringView8 Source, DStringView8 Prefix);

	bool EndsWith(DStringView8 Suffix) const;
	static bool EndsWith(DStringView8 Source, DStringView8 Suffix);

	bool Contains(DStringView8 Search) const;
	static bool Contains(DStringView8 Source, DStringView8 Search);

	cint CountOf(DStringView8 Search, cint Start = 0, cint Stop = -1) const;
	static cint CountOf(DStringView8 Source, DStringView8 Search, cint Start = 0, cint Stop = -1);

	DString8 SubstrRange(cint Start, cint Stop) const;
	static DString8 SubstrRange(DStringView8 Source, cint Start, cint Stop);

	DString8 SubstrCount(cint Start, cint Count) const;
	static DString8 SubstrCount(DStringView8 Source, cint Start, cint Count);

	DString8 Substr(cint Start) const;
	static DString8 Substr(DStringView8 Source, cint Start);

	/* returns whether something was replaced */
	bool ReplaceAll(DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1);

	/* returns whether something was replaced */
	bool ReplaceFirst(DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1);

	void ReplaceRange(cint Start, cint Stop, DStringView8 Other);

	DString8 AllReplaced(DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1) const;
	static DString8 AllReplaced(DStringView8 Source, DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1);

	DString8 FirstReplaced(DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1) const;
	static DString8 FirstReplaced(DStringView8 Source, DStringView8 Search, DStringView8 Replacement, cint Start = 0, cint Stop = -1);

	DString8 RangeReplaced(cint Start, cint Stop, DStringView8 Other) const;
	static DString8 RangeReplaced(DStringView8 Source, cint Start, cint Stop, DStringView8 Other);

	void ReplaceRangeWithChar(cint Start, cint Stop, char8 Char);

	DString8 RangeReplacedWithChar(cint Start, cint Stop, char8 Char) const;
	static DString8 RangeReplacedWithChar(DStringView8 Source, cint Start, cint Stop, char8 Char);

	void RemoveRange(cint Start, cint Stop);

	DString8 RangeRemoved(cint Start, cint Stop) const;
	static DString8 RangeRemoved(DStringView8 Source, cint Start, cint Stop);

	DArray<DString8> Split(DStringView8 Delimiter) const;
	static DArray<DString8> Split(DStringView8 Source, DStringView8 Delimiter);

	static DString8 Join(DStringView8 Delimiter, DArrayView<DString8> Elements);

	void Reverse();

	DString8 Reversed() const;
	static DString8 Reversed(DStringView8 Source);

	/*______________________________________

	CHARACTER ENCODING DEPENDANT
	______________________________________*/

	bool EqualsIgnoreCase_Ascii(DStringView8 Other) const;
	static bool EqualsIgnoreCase_Ascii(DStringView8 A, DStringView8 B);

	cint CompareTo_Ascii(DStringView8 Other) const;
	static cint Compare_Ascii(DStringView8 A, DStringView8 B);

	cint CompareToIgnoreCase_Ascii(DStringView8 Other) const;
	static cint CompareIgnoreCase_Ascii(DStringView8 A, DStringView8 B);

	void UpperCase_Ascii();

	void LowerCase_Ascii();

	DString8 ToUpper_Ascii() const;
	static DString8 ToUpper_Ascii(DStringView8 Source);

	DString8 ToLower_Ascii() const;
	static DString8 ToLower_Ascii(DStringView8 Source);

	void Capitalize_Ascii();

	DString8 Capitalized_Ascii() const;
	static DString8 Capitalized_Ascii(DStringView8 Source);

	cint IndexOfIgnoreCase_Ascii(DStringView8 Search, cint Start = 0, cint Stop = -1) const;
	static cint IndexOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start = 0, cint Stop = -1);

	cint LastIndexOfIgnoreCase_Ascii(DStringView8 Search, cint Start = -1, cint Stop = 0) const;
	static cint LastIndexOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start = -1, cint Stop = 0);

	bool StartsWithIgnoreCase_Ascii(DStringView8 Prefix) const;
	static bool StartsWithIgnoreCase_Ascii(DStringView8 Source, DStringView8 Prefix);

	bool EndsWithIgnoreCase_Ascii(DStringView8 Suffix) const;
	static bool EndsWithIgnoreCase_Ascii(DStringView8 Source, DStringView8 Suffix);

	bool ContainsIgnoreCase_Ascii(DStringView8 Search) const;
	static bool ContainsIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search);

	void RemovePadding_Ascii();

	DString8 PaddingRemoved_Ascii() const;
	static DString8 PaddingRemoved_Ascii(DStringView8 Source);

	cint CountOfIgnoreCase_Ascii(DStringView8 Search, cint Start, cint Stop) const;
	static cint CountOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start, cint Stop);

	bool IsBlank_Ascii() const;
	static bool IsBlank_Ascii(DStringView8 Source);

	/*___________________________

	UTIL
	___________________________*/

	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < this->Size;
	}

	void Clear();

	FORCEINLINE cint Length() const
	{
		return this->Size;
	}

	bool IsEmpty() const;

	// reserves plus 1 for null terminator
	void Reserve(cint NewCapacity);

	void ReserveExtra(cint ExtraCapacity);

	void Resize(cint NewSize, char8 PaddingChar = ' ');

	// Reserve without copying data from the old buffer
	// CapacityAsLength = false sets length to 0
	// CapacityAsLength = true results in an uninitailized string - Unsafe
	void Reallocate(cint NewCapacity, bool CapacityAsLength = false);

	// Overwrite the size variable and place a null terminator
	void OverwriteLengthUnsafe(cint NewSize);

	const char8* CString() const;

	const char8* Data() const;
	
	char8* GetDataUnsafe();

	// null terminator is not included
	cint Capacity() const;

	// iterators
	char8* begin();
	const char8* begin() const;
	char8* end();
	const char8* end() const;

	operator DStringView8() const;

	DStringView8 ViewRange(cint Start, cint Stop) const;

	/*___________________________

	PARSING
	___________________________*/

	static int32 ParseInt32(DStringView8 Buffer);

	static int64 ParseInt64(DStringView8 Buffer);

	static float ParseFloat(DStringView8 Buffer);

	static double ParseDouble(DStringView8 Buffer);
	
};


DString8 operator+(DStringView8 Left, DStringView8 Right);


namespace std
{
	template<>
	struct hash<DString8> 
	{
		cint operator()(const DString8& Str) const;
	};
}
