#include "pch.h"
#include "DString.h"


// --------------------- FormatArgStorage ---------------------


DString::FormatArgStorage::FormatArgStorage()
{
	this->__Construct__();
}


DString::FormatArgStorage::FormatArgStorage(const DString::FormatArgStorage& Other)
{
	this->__CopyConstruct__(Other);
}
DString::FormatArgStorage::FormatArgStorage(const DString& Str)
{
	this->__CopyConstruct__(Str);
}
DString::FormatArgStorage& DString::FormatArgStorage::operator=(const DString::FormatArgStorage& Other)
{
	this->__CopyAssign__(Other);
	return *this;
}
DString::FormatArgStorage& DString::FormatArgStorage::operator=(const DString& Other)
{
	this->__CopyAssign__(Other);
	return *this;
}


DString::FormatArgStorage::FormatArgStorage(DString::FormatArgStorage&& Other) noexcept
{
	this->__MoveConstruct__(std::move(Other));
}
DString::FormatArgStorage::FormatArgStorage(DString&& Str)
{
	this->__MoveConstruct__(std::move(Str));
}
DString::FormatArgStorage& DString::FormatArgStorage::operator=(DString::FormatArgStorage&& Other) noexcept
{
	this->__MoveAssign__(std::move(Other));
	return *this;
}
DString::FormatArgStorage& DString::FormatArgStorage::operator=(DString&& Str)
{
	this->__MoveAssign__(std::move(Str));
	return *this;
}


DString::FormatArgStorage::~FormatArgStorage()
{
	this->__Destruct__();
}


DString::FormatArgStorage::operator DStringView() const
{
	return DStringView(this->_Data(), this->Size);
}


// ---------------------- DStringFormatArg ---------------------


DString::FormatArg::FormatArg()
{
}
DString::FormatArg::FormatArg(int32 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(uint32 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(int64 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(uint64 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(int8 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(uint8 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(int16 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(uint16 V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(float V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(double V) : 
	Storage(DNumberFormat::DEFAULT.ToString(V)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(bool V) : 
	Storage(V? TX("true") : TX("false")), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(tchar V) :
	Storage(DString(&V, 1)),
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(char8 V) :
	Storage(DStrings::UTF8ToUTF16(&V)),
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(const tchar* Str) : 
	StringView(Str)
{
}
DString::FormatArg::FormatArg(const char8* Str) : 
	Storage(DStrings::AsciiToUTF16(Str)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(const DString& Str) :
	StringView(Str)
{
}
DString::FormatArg::FormatArg(const DString8& Str) : 
	Storage(DStrings::AsciiToUTF16(Str.CString())), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(DStringView Str) :
	StringView(Str)
{
}
DString::FormatArg::FormatArg(DStringView8 Str) : 
	Storage(DStrings::AsciiToUTF16(Str)), 
	StringView(Storage)
{
}
DString::FormatArg::FormatArg(const void* Ptr) : 
	Storage(DNumberFormat::DEFAULT.ToString(std::hash<const void*>()(Ptr))),
	StringView(Storage)
{
}


DStringView DString::FormatArg::GetString() const
{
	return this->StringView;
}


// --------------- DString -----------------


const DString DString::EMPTY;


/*___________________________

	CONSTRUCT
___________________________*/

DString::DString()
{
	this->__Construct__();
}


DString::DString(cint InCapacity, bool OverwriteLength)
{
	this->__Construct__(InCapacity, OverwriteLength);
}


DString::DString(const tchar* InCString)
{
	this->__Construct__(InCString);
}


DString::DString(const tchar* InBuffer, cint InLength)
{
	this->__Construct__(InBuffer, InLength);
}


DString DString::From(DStringView Other)
{
	if (Other.IsValid())
		return DString(Other.Data(), Other.Length());
	else
		return DString();
}


/*___________________________

	COPY
___________________________*/


DString::DString(const DString& Other)
{
	this->__CopyConstruct__(Other);
}


DString& DString::operator=(const DString& Other)
{
	this->__CopyAssign__(Other);
	return *this;
}

/*___________________________

	MOVE
___________________________*/

DString::DString(DString&& Other) noexcept
{
	this->__MoveConstruct__(std::move(Other));
}


DString& DString::operator=(DString&& Other) noexcept
{
	this->__MoveAssign__(std::move(Other));
	return *this;
}

/*___________________________

	DESTRUCT
___________________________*/

DString::~DString()
{
	this->__Destruct__();
}

/*___________________________

	MANIPULATION
___________________________*/

tchar& DString::operator[](cint Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


const tchar& DString::operator[](cint Index) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


tchar& DString::At(cint Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


const tchar& DString::At(cint Index) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


bool DString::operator==(DStringView Other) const
{
	return TChar::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length());
}


bool DString::operator!=(DStringView Other) const
{
	return TChar::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length()) == false;
}


bool DString::Equals(DStringView Other) const
{
	return TChar::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length());
}


DString& DString::operator+=(DStringView Other)
{
	this->_Append(Other.Data(), Other.Length());
	return *this;
}


void DString::Append(DStringView Buffer)
{
	this->_Append(Buffer.Data(), Buffer.Length());
}


void DString::Append(tchar Char)
{
	this->_Append(&Char, 1);
}
	

void DString::Prepend(DStringView Buffer)
{
	this->_Prepend(Buffer.Data(), Buffer.Length());
}

	 
void DString::Prepend(tchar Char)
{
	this->_Prepend(&Char, 1);
}


cint DString::IndexOf(DStringView Search, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return _IndexOf(this->_Data(), this->Size, Search.Data(), Search.Length(), Start, Stop);
}


cint DString::IndexOf(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	return _IndexOf(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Start, Stop);
}


cint DString::LastIndexOf(DStringView Search, cint Start, cint Stop) const
{
	if (Start < 0) Start = this->Size - 1;
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= this->Size || Stop < 0 || Stop > Start)
	{
		DString8 str = "Invalid range with reverse searching: start: ";
		str.Append(DNumberFormat8::DEFAULT.ToString(Start));
		str.Append(" stop: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Stop));
		str.Append(" length: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(this->Size));

		throw DIndexException(str.CString());
	}
#endif
	return _LastIndexOf(this->_Data(), this->Size, Search.Data(), Search.Length(), Start, Stop);
}


cint DString::LastIndexOf(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Start < 0) Start = Source.Length() - 1;
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= Source.Length() || Stop < 0 || Stop > Start)
	{
		DString8 str = "Invalid range with reverse searching: start: ";
		str.Append(DNumberFormat8::DEFAULT.ToString(Start));
		str.Append(" stop: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Stop));
		str.Append(" length: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Source.Length()));

		throw DIndexException(str.CString());
	}
#endif
	return _LastIndexOf(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Start, Stop);
}


bool DString::StartsWith(DStringView Prefix) const
{
	return _IndexOf(this->_Data(), this->Size, Prefix.Data(), Prefix.Length(), 0, this->Size) == 0;
}


bool DString::StartsWith(DStringView Source, DStringView Prefix)
{
	return _IndexOf(Source.Data(), Source.Length(), Prefix.Data(), Prefix.Length(), 0, Source.Length()) == 0;
}


bool DString::EndsWith(DStringView Suffix) const
{
	return _LastIndexOf(this->_Data(), this->Size, Suffix.Data(), Suffix.Length(), this->Size - 1, 0) == this->Size - Suffix.Length();
}


bool DString::EndsWith(DStringView Source, DStringView Suffix)
{
	return _LastIndexOf(Source.Data(), Source.Length(), Suffix.Data(), Suffix.Length(), Source.Length() - 1, 0) == Source.Length() - Suffix.Length();
}


bool DString::Contains(DStringView Search) const
{
	return _IndexOf(this->_Data(), this->Size, Search.Data(), Search.Length(), 0, this->Size) >= 0;
}


bool DString::Contains(DStringView Source, DStringView Search)
{
	return _IndexOf(Source.Data(), Source.Length(), Search.Data(), Search.Length(), 0, Source.Length()) >= 0;
}


cint DString::CountOf(DStringView Search, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return _CountOf(this->_Data(), this->Size, Search.Data(), Search.Length(), Start, Stop);
}


cint DString::CountOf(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	return _CountOf(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Start, Stop);
}


DString DString::SubstrRange(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	_SubstrRange(this->_Data(), this->Size, Start, Stop, result);
	return result;
}


DString DString::SubstrRange(DStringView Source, cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString result;
	_SubstrRange(Source.Data(), Source.Length(), Start, Stop, result);
	return result;
}


DString DString::SubstrCount(cint Start, cint Count) const
{
	cint stop = Start + Count;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, stop, this->Size))
	{
		throw DIndexException(Start, stop, this->Size);
	}
#endif
	DString result;
	_SubstrCount(this->_Data(), this->Size, Start, Count, result);
	return result;
}


DString DString::SubstrCount(DStringView Source, cint Start, cint Count)
{
	cint stop = Start + Count;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, stop, Source.Length()))
	{
		throw DIndexException(Start, stop, Source.Length());
	}
#endif
	DString result;
	_SubstrCount(Source.Data(), Source.Length(), Start, Count, result);
	return result;
}


DString DString::Substr(cint Start) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= this->Size || Start < 0)
	{
		throw DIndexException(Start, this->Size);
	}
#endif
	DString result;
	_Substr(this->_Data(), this->Size, Start, result);
	return result;
}


DString DString::Substr(DStringView Source, cint Start)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= Source.Length() || Start < 0)
	{
		throw DIndexException(Start, Source.Length());
	}
#endif
	DString result;
	_Substr(Source.Data(), Source.Length(), Start, result);
	return result;
}


bool DString::ReplaceAll(DStringView Search, DStringView Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	bool ret = _ReplaceAllCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	this->__CopyAssign__(result);
	return ret;
}


bool DString::ReplaceFirst(DStringView Search, DStringView Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return this->_ReplaceFirst(Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), Start, Stop);
}


void DString::ReplaceRange(cint Start, cint Stop, DStringView Other)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_ReplaceRange(Start, Stop, Other.Data(), Other.Length());
}


DString DString::AllReplaced(DStringView Search, DStringView Replacement, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	_ReplaceAllCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString DString::AllReplaced(DStringView Source, DStringView Search, DStringView Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString result;
	_ReplaceAllCopy(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString DString::FirstReplaced(DStringView Search, DStringView Replacement, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	_ReplaceFirstCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString DString::FirstReplaced(DStringView Source, DStringView Search, DStringView Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString result;
	_ReplaceFirstCopy(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString DString::RangeReplaced(cint Start, cint Stop, DStringView Other) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	_ReplaceRangeCopy(this->_Data(), this->Size, Start, Stop, Other.Data(), Other.Length(), result);
	return result;
}


DString DString::RangeReplaced(DStringView Source, cint Start, cint Stop, DStringView Other)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString result;
	_ReplaceRangeCopy(Source.Data(), Source.Length(), Start, Stop, Other.Data(), Other.Length(), result);
	return result;
}


void DString::ReplaceRangeWithChar(cint Start, cint Stop, tchar Char)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_ReplaceRangeWithChar(Start, Stop, Char);
}


DString DString::RangeReplacedWithChar(cint Start, cint Stop, tchar Char) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString copy;
	_ReplaceRangeWithCharCopy(this->_Data(), this->Size, Start, Stop, Char, copy);
	return copy;
}


DString DString::RangeReplacedWithChar(DStringView Source, cint Start, cint Stop, tchar Char)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString copy;
	_ReplaceRangeWithCharCopy(Source.Data(), Source.Length(), Start, Stop, Char, copy);
	return copy;
}


void DString::RemoveRange(cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_RemoveRange(Start, Stop);
}


DString DString::RangeRemoved(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString result;
	_RemoveRangeCopy(this->_Data(), this->Size, Start, Stop, result);
	return result;
}


DString DString::RangeRemoved(DStringView Source, cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString result;
	_RemoveRangeCopy(Source.Data(), Source.Length(), Start, Stop, result);
	return result;
}


DArray<DString> DString::Split(DStringView Delimiter) const
{
	if (this->Size == 0 || Delimiter.Length() == 0) return { *this };

	// first item
	cint i = this->IndexOf(Delimiter);
	if (i >= 0)
	{
		DArray<DString> outArr;
		outArr.Add(this->SubstrRange(0, i));

		// other items
		cint start = i + Delimiter.Length();
		while (true)
		{
			i = IndexOf(*this, Delimiter, start);
			if (i >= 0)
			{
				outArr.Add(this->SubstrRange(start, i));
				start = i + Delimiter.Length();
			}
			else break;
		}

		// last item
		if (start < this->Size)
		{
			outArr.Add(this->Substr(start));
		}

		return outArr;
	}
	else return { *this };
}


DArray<DString> DString::Split(DStringView Source, DStringView Delimiter)
{
	if (Source.Length() == 0 || Delimiter.Length() == 0) return { DString::From(Source) };

	// first item
	cint i = IndexOf(Source, Delimiter);
	if (i >= 0)
	{
		DArray<DString> outArr;
		outArr.Add(SubstrRange(Source, 0, i));

		// other items
		cint start = i + Delimiter.Length();
		while (true)
		{
			i = IndexOf(Source, Delimiter, start);
			if (i >= 0)
			{
				outArr.Add(SubstrRange(Source, start, i));
				start = i + Delimiter.Length();
			}
			else break;
		}
		
		// last item
		if (start < Source.Length())
		{
			outArr.Add(Substr(Source, start));
		}

		return outArr;
	}
	else return { DString::From(Source) };
}


DString DString::Join(DStringView Delimiter, DArrayView<DString> Elements)
{
	DString out;
	
	cint len = 0;
	for (cint i = 0; i < Elements.Length(); i++)
	{
		len += Elements[i].Length();
	}
	len += Delimiter.Length() * (SCounterUtil::Max(0, Elements.Length() - 1));
	out.Reserve(len);

	for (cint i = 0; i < Elements.Length(); i++)
	{
		out.Append(Elements[i]);
		if (i < Elements.Length() - 1)
		{
			out.Append(Delimiter);
		}
	}

	return out;
}


void DString::Reverse()
{
	this->_Reverse();
}


DString DString::Reversed() const
{
	DString result;
	_Reversed(this->_Data(), this->Size, result);
	return result;
}


DString DString::Reversed(DStringView Source)
{
	DString result;
	_Reversed(Source.Data(), Source.Length(), result);
	return result;
}


DString DString::Format(DStringView FormatStr, const DArray<DString::FormatArg>& Args)
{
	DString res;

	int32 open = -1;
	DString numStr;
	for (int32 i = 0; i < FormatStr.Length(); i++)
	{
		tchar c = FormatStr[i];
		if (c == TX('{'))
		{
			open = i;
		}
		else if (c == TX('}'))
		{
			if (open >= 0)
			{
				int32 num = DString::ParseInt32(numStr);
				if (num >= 0 && num < Args.Length())
				{
					res.Append(Args[num].GetString());
				}
				else
				{
					// invalid arg index
				}
				numStr.Clear();
				open = -1;
			}
			else
			{
				break;  // invalid format string
			}
		}
		else if (open >= 0)
		{
			if (TChar::IsDigit(c))
			{
				numStr.Append(c);
			}
			else
			{
				break;  // invalid format string
			}
		}
		else
		{
			res.Append(c);
		}
	}

	return res;
}

/*______________________________________

	CHARACTER ENCODING DEPENDANT
______________________________________*/

bool DString::Equals(DStringView A, DStringView B)
{
	return TChar::EqualStrings(A.Data(), A.Length(), B.Data(), B.Length());
}


bool DString::EqualsIgnoreCase(DStringView Other) const
{
	return TChar::EqualStringsIgnoreCase(this->_Data(), this->Size, Other.Data(), Other.Length());
}


bool DString::EqualsIgnoreCase(DStringView A, DStringView B)
{
	return TChar::EqualStringsIgnoreCase(A.Data(), A.Length(), B.Data(), B.Length());
}


cint DString::CompareTo(DStringView Other) const
{
	return TChar::CompareStrings(this->_Data(), this->Size, Other.Data(), Other.Length());
}


cint DString::Compare(DStringView A, DStringView B)
{
	return TChar::CompareStrings(A.Data(), A.Length(), B.Data(), B.Length());
}


cint DString::CompareToIgnoreCase(DStringView Other) const
{
	return TChar::CompareStringsIgnoreCase(this->Data(), this->Size, Other.Data(), Other.Length());
}


cint DString::CompareIgnoreCase(DStringView A, DStringView B)
{
	return TChar::CompareStringsIgnoreCase(A.Data(), A.Length(), B.Data(), B.Length());
}


void DString::UpperCase()
{
	TChar::ToUpperString(this->_Data(), this->Size, this->_Data());
}


void DString::LowerCase()
{
	TChar::ToLowerString(this->_Data(), this->Size, this->_Data());
}


DString DString::ToUpper() const
{
	DString out;
	out.Reallocate(this->Size, true);
	TChar::ToUpperString(this->_Data(), this->Size, out._Data());
	return out;
}


DString DString::ToUpper(DStringView Source)
{
	DString out;
	out.Reallocate(Source.Length(), true);
	TChar::ToUpperString(Source.Data(), Source.Length(), out._Data());
	return out;
}


DString DString::ToLower() const
{
	DString out;
	out.Reallocate(this->Size, true);
	TChar::ToLowerString(this->_Data(), this->Size, out._Data());
	return out;
}


DString DString::ToLower(DStringView Source)
{
	DString out;
	out.Reallocate(Source.Length(), true);
	TChar::ToLowerString(Source.Data(), Source.Length(), out._Data());
	return out;
}


void DString::Capitalize()
{
	if (this->Size > 0)
	{
		(*this)[0] = TChar::ToUpper((*this)[0]);
	}
}


DString DString::Capitalized() const
{
	DString out = *this;
	if (this->Size > 0)
	{
		out[0] = TChar::ToUpper(out[0]);
	}
	return out;
}


DString DString::Capitalized(DStringView Source)
{
	DString out = DString::From(Source);
	if (Source.Length() > 0)
	{
		out[0] = TChar::ToUpper(out[0]);
	}
	return out;
}


cint DString::IndexOfIgnoreCase(DStringView Search, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return _IndexOfCustomCompare(
		this->_Data(), this->Size,
		Search.Data(), Search.Length(), 
		TChar::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString::IndexOfIgnoreCase(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	return _IndexOfCustomCompare(
		Source.Data(), Source.Length(),
		Search.Data(), Search.Length(), 
		TChar::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString::LastIndexOfIgnoreCase(DStringView Search, cint Start, cint Stop) const
{
	if (Start < 0) Start = this->Size - 1;
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= this->Size || Stop < 0 || Stop > Start)
	{
		DString8 str = "Invalid range with reverse searching: start: ";
		str.Append(DNumberFormat8::DEFAULT.ToString(Start));
		str.Append(" stop: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Stop));
		str.Append(" length: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(this->Size));

		throw DIndexException(str.CString());
	}
#endif
	return _LastIndexOfCustomCompare(
		this->_Data(), this->Size,
		Search.Data(), Search.Length(), 
		TChar::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString::LastIndexOfIgnoreCase(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Start < 0) Start = Source.Length() - 1;
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= Source.Length() || Stop < 0 || Stop > Start)
	{
		DString8 str = "Invalid range with reverse searching: start: ";
		str.Append(DNumberFormat8::DEFAULT.ToString(Start));
		str.Append(" stop: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Stop));
		str.Append(" length: ");
		str.Append(DNumberFormat8::DEFAULT.ToString(Source.Length()));

		throw DIndexException(str.CString());
	}
#endif
	return _LastIndexOfCustomCompare(
		Source.Data(), Source.Length(),
		Search.Data(), Search.Length(), 
		TChar::CompareIgnoreCase, 
		Start, Stop
	);
}


bool DString::StartsWithIgnoreCase(DStringView Prefix) const
{
	return _IndexOfCustomCompare(
		this->_Data(), this->Size,
		Prefix.Data(), Prefix.Length(), 
		TChar::CompareIgnoreCase,
		0, this->Size
	) == 0;
}


bool DString::StartsWithIgnoreCase(DStringView Source, DStringView Prefix)
{
	return _IndexOfCustomCompare(
		Source.Data(), Source.Length(),
		Prefix.Data(), Prefix.Length(), 
		TChar::CompareIgnoreCase,
		0, Source.Length()
	) == 0;
}


bool DString::EndsWithIgnoreCase(DStringView Suffix) const
{
	return _LastIndexOfCustomCompare(
		this->_Data(), this->Size,
		Suffix.Data(), Suffix.Length(),
		TChar::CompareIgnoreCase,
		this->Size - 1, 0
	) == this->Size - Suffix.Length();
}


bool DString::EndsWithIgnoreCase(DStringView Source, DStringView Suffix)
{
	return _LastIndexOfCustomCompare(
		Source.Data(), Source.Length(),
		Suffix.Data(), Suffix.Length(),
		TChar::CompareIgnoreCase,
		Source.Length() - 1, 0
	) == Source.Length() - Suffix.Length();
}


bool DString::ContainsIgnoreCase(DStringView Search) const
{
	return _IndexOfCustomCompare(
		this->_Data(), this->Size,
		Search.Data(), Search.Length(),
		TChar::CompareIgnoreCase,
		0, this->Size
	) >= 0;
}


bool DString::ContainsIgnoreCase(DStringView Source, DStringView Search)
{
	return _IndexOfCustomCompare(
		Source.Data(), Source.Length(),
		Search.Data(), Search.Length(),
		TChar::CompareIgnoreCase,
		0, Source.Length()
	) >= 0;
}


cint DString::CountOfIgnoreCase(DStringView Search, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return _CountOfCustomCompare(this->_Data(), this->Size, Search.Data(), Search.Length(), TChar::CompareIgnoreCase, Start, Stop);
}


cint DString::CountOfIgnoreCase(DStringView Source, DStringView Search, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	return _CountOfCustomCompare(Source.Data(), Source.Length(), Search.Data(), Search.Length(), TChar::CompareIgnoreCase, Start, Stop);
}


void DString::RemovePadding()
{
	tchar* arr = this->_Data();

	cint prefix = 0;
	for (cint i = 0; i < this->Size; i++)
	{
		if (TChar::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = Size - 1; i >= 0; i--)
	{
		if (TChar::IsWhitespace(arr[i])) suffix++;
		else break;
	}

	cint charsToKeep = this->Size - prefix - suffix;
	cint stop = prefix + charsToKeep;
	for (cint i = prefix; i < stop; i++)
	{
		arr[i - prefix] = arr[i];
	}
	this->Size = charsToKeep;
	arr[this->Size] = '\0';
}


DString DString::PaddingRemoved() const
{
	const tchar* arr = this->_Data();

	cint prefix = 0;
	for (cint i = 0; i < this->Size; i++)
	{
		if (TChar::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = this->Size - 1; i >= 0; i--)
	{
		if (TChar::IsWhitespace(arr[i])) suffix++;
		else break;
	}

	cint charsToKeep = this->Size - prefix - suffix;
	DString out;
	out.Append(DStringView(arr + prefix, charsToKeep));
	return out;
}


DString DString::PaddingRemoved(DStringView Source)
{
	const tchar* arr = Source.Data();

	cint prefix = 0;
	for (cint i = 0; i < Source.Length(); i++)
	{
		if (TChar::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = Source.Length() - 1; i >= 0; i--)
	{
		if (TChar::IsWhitespace(arr[i])) suffix++;
		else break;
	}

	cint charsToKeep = Source.Length() - prefix - suffix;
	DString out;
	out.Append(DStringView(arr + prefix, charsToKeep));
	return out;
}


bool DString::IsBlank() const
{
	if (this->Size == 0) return true;
	const tchar* data = this->_Data();
	for (cint i = 0; i < this->Size; ++i)
	{
		if (TChar::IsWhitespace(data[i]) == false) return false; 
	}
	return true;
}


bool DString::IsBlank(DStringView Source)
{
	if (Source.Length() == 0) return true;
	const tchar* data = Source.Data();
	for (cint i = 0; i < Source.Length(); ++i)
	{
		if (TChar::IsWhitespace(data[i]) == false) return false; 
	}
	return true;
}


/*___________________________

	UTIL
___________________________*/

void DString::Clear()
{
	this->_Clear();
}


bool DString::IsEmpty() const
{
	return this->Size < 1;
}


void DString::Reserve(cint NewCapacity)
{
	this->_Reserve(NewCapacity);
}


void DString::ReserveExtra(cint ExtraCapacity)
{
	this->_Reserve(this->Size + ExtraCapacity);
}


void DString::Resize(cint NewSize, tchar PaddingChar)
{
	this->_Resize(NewSize, PaddingChar);
}


void DString::Reallocate(cint NewCapacity, bool CapacityAsLength)
{
	this->_Reallocate(NewCapacity, CapacityAsLength);
}


void DString::OverwriteLengthUnsafe(cint NewSize)
{
	this->Size = NewSize;
	this->_Data()[NewSize] = '\0';
}


const tchar* DString::CString() const
{
	return this->_Data();
}


const tchar* DString::Data() const
{
	return this->_Data();
}


tchar* DString::GetDataUnsafe()
{
	return this->_Data();
}


cint DString::Capacity() const
{
	return this->_Capacity();
}


// enhanced for loop
tchar* DString::begin() { return this->_Data(); };
const tchar* DString::begin() const { return this->_Data(); }
tchar* DString::end() { return this->_Data() + this->Size; }
const tchar* DString::end() const { return this->_Data() + this->Size; }


DString::operator DStringView() const
{
	return DStringView(this->_Data(), this->Size);
}


DStringView DString::ViewRange(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return DStringView(this->_Data() + Start, Stop - Start);
}


/*___________________________

	PARSING
___________________________*/


int32 DString::ParseInt32(DStringView Buffer)
{
	const tchar* arr = Buffer.Data();
	cint len = Buffer.Length();
	char* ascii = (char*)_malloca((static_cast<size_t>(len) + 1) * sizeof(char));
	int32 result;
	if (ascii)
	{
		for (cint i = 0; i < len; i++)
		{
			ascii[i] = (char)arr[i];
		}
		ascii[len] = '\0';
		result = std::atoi(ascii);
	}
	else
	{
		result = 0;
	}
	_freea(ascii);
	return result;
}


int64 DString::ParseInt64(DStringView Buffer)
{
	const tchar* arr = Buffer.Data();
	cint len = Buffer.Length();
	char* ascii = (char*)_malloca((static_cast<size_t>(len) + 1) * sizeof(char));
	int64 result;
	if (ascii)
	{
		for (cint i = 0; i < len; i++)
		{
			ascii[i] = (char)arr[i];
		}
		ascii[len] = '\0';
		result = std::atoll(ascii);
	}
	else
	{
		result = 0LL;
	}
	_freea(ascii);
	return result;
}


float DString::ParseFloat(DStringView Buffer)
{
	const tchar* arr = Buffer.Data();
	cint len = Buffer.Length();
	char* ascii = (char*)_malloca((static_cast<size_t>(len) + 1) * sizeof(char));
	float result;
	if (ascii)
	{
		for (cint i = 0; i < len; i++)
		{
			ascii[i] = (char)arr[i];
		}
		ascii[len] = '\0';
		result = (float)std::atof(ascii);
	}
	else
	{
		result = .0f;
	}
	_freea(ascii);
	return result;
}


double DString::ParseDouble(DStringView Buffer)
{
	const tchar* arr = Buffer.Data();
	cint len = Buffer.Length();
	char* ascii = (char*)_malloca((static_cast<size_t>(len) + 1) * sizeof(char));
	double result;
	if (ascii)
	{
		for (cint i = 0; i < len; i++)
		{
			ascii[i] = (char)arr[i];
		}
		ascii[len] = '\0';
		result = std::atof(ascii);
	}
	else
	{
		result = .0;
	}
	_freea(ascii);
	return result;
}


DString operator+(DStringView Left, DStringView Right)
{
	DString out (Left.Length() + Right.Length());
	out.Append(Left);
	out.Append(Right);
	return out;
}


cint std::hash<DString>::operator()(const DString& Str) const
{
	return SCounterUtil::PrimitiveCArrayHashCode<tchar>(Str.Data(), Str.Length());
}
