/*_________________________________________________________________________________

	DString
	- dynamic (resizable) string
	- stores short strings on the stack
	- utf-16 encoding

_________________________________________________________________________________*/

#pragma once


#define DSTRING_INTERNAL_BUFFER_BYTES 32


class DString : public _DStringBase<tchar, DSTRING_INTERNAL_BUFFER_BYTES, DArrayAllocator<tchar>>
{
private:
	
	// FormatArg can't store a DString because at this point DString is an incomplete class

	class FormatArgStorage : public _DStringBase<tchar, DSTRING_INTERNAL_BUFFER_BYTES, DArrayAllocator<tchar>>
	{
	public:
		FormatArgStorage();

		FormatArgStorage(const FormatArgStorage& Other);
		FormatArgStorage(const DString& Str);
		FormatArgStorage& operator=(const FormatArgStorage& Other);
		FormatArgStorage& operator=(const DString& Str);
		
		FormatArgStorage(FormatArgStorage&& Other) noexcept;
		FormatArgStorage(DString&& Str);
		FormatArgStorage& operator=(FormatArgStorage&& Other) noexcept;
		FormatArgStorage& operator=(DString&& Str);
		
		~FormatArgStorage();

		operator DStringView() const;
	};

public:

	class FormatArg
	{
	private:
		FormatArgStorage Storage;
		DStringView StringView;

	public:
		FormatArg();
		FormatArg(int32 V);
		FormatArg(uint32 V);
		FormatArg(int64 V);
		FormatArg(uint64 V);
		FormatArg(int8 V);
		FormatArg(uint8 V);
		FormatArg(int16 V);
		FormatArg(uint16 V);
		FormatArg(float V);
		FormatArg(double V);
		FormatArg(bool V);
		FormatArg(tchar V);
		FormatArg(char8 V);
		FormatArg(const tchar* Str);
		FormatArg(const char8* Str);
		FormatArg(const DString& Str);
		FormatArg(const DString8& Str);
		FormatArg(DStringView Str);
		FormatArg(DStringView8 Str);
		FormatArg(const void* Ptr);

		DStringView GetString() const;
	};


	static const DString EMPTY;

	// ALL NON-STATIC FIELDS MUST BE DECLARED IN _DStringBase!

	/*___________________________

		CONSTRUCT
	___________________________*/

	DString();

	// InCapacity: null terminator is not included
	// OverwriteLength = true results in an uninitialized array with an extra null terminator - Unsafe
	explicit DString(cint InCapacity, bool OverwriteLength = false);

	DString(const tchar* InCString);

	DString(const tchar* InBuffer, cint InLength);

	static DString From(DStringView Other);

public:

	/*___________________________

		COPY
	___________________________*/

	DString(const DString& Other);

	DString& operator=(const DString& Other);

	/*___________________________

		MOVE
	___________________________*/
	
	DString(DString&& Other) noexcept;
	
	DString& operator=(DString&& Other) noexcept;

	/*___________________________

		DESTRUCT
	___________________________*/

	~DString();

	/*___________________________

		MANIPULATION
	___________________________*/

	tchar& operator[](cint Index);

	const tchar& operator[](cint Index) const;

	tchar& At(cint Index);

	const tchar& At(cint Index) const;

	bool operator==(DStringView Other) const;

	bool operator!=(DStringView Other) const;

	bool Equals(DStringView Other) const;
	static bool Equals(DStringView A, DStringView B);

	DString& operator+=(DStringView Other);

	void Append(DStringView Buffer);
	
	void Append(tchar Char);

	void Prepend(DStringView Buffer);
	
	void Prepend(tchar Char);

	cint IndexOf(DStringView Search, cint Start = 0, cint Stop = -1) const;
	static cint IndexOf(DStringView Source, DStringView Search, cint Start = 0, cint Stop = -1);

	cint LastIndexOf(DStringView Search, cint Start = -1, cint Stop = 0) const;
	static cint LastIndexOf(DStringView Source, DStringView Search, cint Start = -1, cint Stop = 0);

	bool StartsWith(DStringView Prefix) const;
	static bool StartsWith(DStringView Source, DStringView Prefix);

	bool EndsWith(DStringView Suffix) const;
	static bool EndsWith(DStringView Source, DStringView Suffix);

	bool Contains(DStringView Search) const;
	static bool Contains(DStringView Source, DStringView Search);

	cint CountOf(DStringView Search, cint Start = 0, cint Stop = -1) const;
	static cint CountOf(DStringView Source, DStringView Search, cint Start = 0, cint Stop = -1);

	DString SubstrRange(cint Start, cint Stop) const;
	static DString SubstrRange(DStringView Source, cint Start, cint Stop);

	DString SubstrCount(cint Start, cint Count) const;
	static DString SubstrCount(DStringView Source, cint Start, cint Count);
	
	DString Substr(cint Start) const;
	static DString Substr(DStringView Source, cint Start);

	/* returns whether something was replaced */
	bool ReplaceAll(DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1);

	/* returns whether something was replaced */
	bool ReplaceFirst(DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1);
	
	void ReplaceRange(cint Start, cint Stop, DStringView Replacement);

	DString AllReplaced(DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1) const;
	static DString AllReplaced(DStringView Source, DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1);

	DString FirstReplaced(DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1) const;
	static DString FirstReplaced(DStringView Source, DStringView Search, DStringView Replacement, cint Start = 0, cint Stop = -1);

	DString RangeReplaced(cint Start, cint Stop, DStringView Replacement) const;
	static DString RangeReplaced(DStringView Source, cint Start, cint Stop, DStringView Replacement);

	void ReplaceRangeWithChar(cint Start, cint Stop, tchar Char);

	DString RangeReplacedWithChar(cint Start, cint Stop, tchar Char) const;
	static DString RangeReplacedWithChar(DStringView Source, cint Start, cint Stop, tchar Char);

	void RemoveRange(cint Start, cint Stop);

	DString RangeRemoved(cint Start, cint Stop) const;
	static DString RangeRemoved(DStringView Source, cint Start, cint Stop);

	DArray<DString> Split(DStringView Delimiter) const;
	static DArray<DString> Split(DStringView Source, DStringView Delimiter);

	static DString Join(DStringView Delimiter, DArrayView<DString> Elements);

	void Reverse();

	DString Reversed() const;
	static DString Reversed(DStringView Source);

	static DString Format(DStringView FormatStr, const DArray<DString::FormatArg>& Args);

	/*______________________________________

		CHARACTER ENCODING DEPENDANT
	______________________________________*/

	bool EqualsIgnoreCase(DStringView Other) const;
	static bool EqualsIgnoreCase(DStringView A, DStringView B);

	cint CompareTo(DStringView Other) const;
	static cint Compare(DStringView A, DStringView B);

	cint CompareToIgnoreCase(DStringView Other) const;
	static cint CompareIgnoreCase(DStringView A, DStringView B);

	void UpperCase();

	void LowerCase();

	DString ToUpper() const;
	static DString ToUpper(DStringView Source);

	DString ToLower() const;
	static DString ToLower(DStringView Source);

	void Capitalize();

	DString Capitalized() const;
	static DString Capitalized(DStringView Source);

	cint IndexOfIgnoreCase(DStringView Search, cint Start = 0, cint Stop = -1) const;
	static cint IndexOfIgnoreCase(DStringView Source, DStringView Search, cint Start = 0, cint Stop = -1);

	cint LastIndexOfIgnoreCase(DStringView Search, cint Start = -1, cint = 0) const;
	static cint LastIndexOfIgnoreCase(DStringView Source, DStringView Search, cint Start = -1, cint = 0);

	bool StartsWithIgnoreCase(DStringView Prefix) const;
	static bool StartsWithIgnoreCase(DStringView Source, DStringView Prefix);

	bool EndsWithIgnoreCase(DStringView Suffix) const;
	static bool EndsWithIgnoreCase(DStringView Source, DStringView Suffix);

	bool ContainsIgnoreCase(DStringView Search) const;
	static bool ContainsIgnoreCase(DStringView Source, DStringView Search);

	cint CountOfIgnoreCase(DStringView Search, cint Start = 0, cint Stop = -1) const;
	static cint CountOfIgnoreCase(DStringView Source, DStringView Search, cint Start = 0, cint Stop = -1);
	
	void RemovePadding();

	DString PaddingRemoved() const;
	static DString PaddingRemoved(DStringView Source);

	bool IsBlank() const;
	static bool IsBlank(DStringView Source);

	/*___________________________

		UTIL
	___________________________*/

	FORCEINLINE bool IsValidIndex(cint Index) const
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

	void Resize(cint NewSize, tchar PaddingChar = TX(' '));

	// Reserve without copying data from the old buffer
	// CapacityAsLength = false sets length to 0
	// CapacityAsLength = true results in an uninitailized string - Unsafe
	void Reallocate(cint NewCapacity, bool CapacityAsLength = false);

	// Overwrite the size variable and place a null terminator
	void OverwriteLengthUnsafe(cint NewSize);

	const tchar* CString() const;

	const tchar* Data() const;
	
	tchar* GetDataUnsafe();

	// null terminator is not included
	cint Capacity() const;

	// iterators
	tchar* begin();
	const tchar* begin() const;
	tchar* end();
	const tchar* end() const;

	operator DStringView() const;

	DStringView ViewRange(cint Start, cint Stop) const;


	/*___________________________

		PARSING
	___________________________*/

	static int32 ParseInt32(DStringView Buffer);

	static int64 ParseInt64(DStringView Buffer);

	static float ParseFloat(DStringView Buffer);

	static double ParseDouble(DStringView Buffer);

};


DString operator+(DStringView Left, DStringView Right);


namespace std
{
	template<>
	struct hash<DString> 
	{
		cint operator()(const DString& Str) const;
	};
}


using DStringFormatArg = DString::FormatArg;
