#include "pch.h"
#include "DString8.h"


const DString8 DString8::EMPTY;


/*___________________________

CONSTRUCT
___________________________*/

DString8::DString8()
{
	this->__Construct__();
}


DString8::DString8(cint InCapacity, bool OverwriteLength)
{
	this->__Construct__(InCapacity, OverwriteLength);
}


DString8::DString8(const char8* InCString)
{
	this->__Construct__(InCString);
}


DString8::DString8(const char8* InBuffer, cint InLength)
{
	this->__Construct__(InBuffer, InLength);
}


DString8 DString8::From(DStringView8 Other)
{
	if (Other.IsValid())
		return DString8(Other.Data(), Other.Length());
	else
		return DString8();
}


/*___________________________

COPY
___________________________*/

DString8::DString8(const DString8& Other)
{
	this->__CopyConstruct__(Other);
}


DString8& DString8::operator=(const DString8& Other)
{
	this->__CopyAssign__(Other);
	return *this;
}

/*___________________________

MOVE
___________________________*/

DString8::DString8(DString8&& Other) noexcept
{
	this->__MoveConstruct__(std::move(Other));
}


DString8& DString8::operator=(DString8&& Other) noexcept
{
	this->__MoveAssign__(std::move(Other));
	return *this;
}

/*___________________________

DESTRUCT
___________________________*/

DString8::~DString8()
{
	this->__Destruct__();
}

/*___________________________

MANIPULATION
___________________________*/

char8& DString8::operator[](cint Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


const char8& DString8::operator[](cint Index) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


char8& DString8::At(cint Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


const char8& DString8::At(cint Index) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= this->Size || Index < 0)
	{
		throw DIndexException(Index, this->Size);
	}
#endif
	return this->_Data()[Index];
}


bool DString8::Equals(DStringView8 Other) const
{
	return SAscii::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length());
}


bool DString8::operator==(DStringView8 Other) const
{
	return SAscii::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length());
}


bool DString8::operator!=(DStringView8 Other) const
{
	return SAscii::EqualStrings(this->_Data(), this->Size, Other.Data(), Other.Length()) == false;
}


DString8& DString8::operator+=(DStringView8 Other)
{
	this->_Append(Other.Data(), Other.Length());
	return *this;
}


void DString8::Append(DStringView8 Buffer)
{
	this->_Append(Buffer.Data(), Buffer.Length());
}


void DString8::Append(const std::string::const_iterator& Start, const std::string::const_iterator& Stop)
{
	this->_Append(&(*Start), (cint)(Stop - Start));
}


void DString8::Append(char8 Char)
{
	this->_Append(&Char, 1);
}


void DString8::Prepend(DStringView8 Buffer)
{
	this->_Prepend(Buffer.Data(), Buffer.Length());
}


void DString8::Prepend(char8 Char)
{
	this->_Prepend(&Char, 1);
}


cint DString8::IndexOf(DStringView8 Search, cint Start, cint Stop) const
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


cint DString8::IndexOf(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
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


cint DString8::LastIndexOf(DStringView8 Search, cint Start, cint Stop) const
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


cint DString8::LastIndexOf(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
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


bool DString8::StartsWith(DStringView8 Prefix) const
{
	return _IndexOf(this->_Data(), this->Size, Prefix.Data(), Prefix.Length(), 0, this->Size) == 0;
}


bool DString8::StartsWith(DStringView8 Source, DStringView8 Prefix)
{
	return _IndexOf(Source.Data(), Source.Length(), Prefix.Data(), Prefix.Length(), 0, Source.Length()) == 0;
}


bool DString8::EndsWith(DStringView8 Suffix) const
{
	return _LastIndexOf(this->_Data(), this->Size, Suffix.Data(), Suffix.Length(), this->Size - 1, 0) == this->Size - Suffix.Length();
}


bool DString8::EndsWith(DStringView8 Source, DStringView8 Suffix)
{
	return _LastIndexOf(Source.Data(), Source.Length(), Suffix.Data(), Suffix.Length(), Source.Length() - 1, 0) == Source.Length() - Suffix.Length();
}


bool DString8::Contains(DStringView8 Search) const
{
	return _IndexOf(this->_Data(), this->Size, Search.Data(), Search.Length(), 0, this->Size) >= 0;
}


bool DString8::Contains(DStringView8 Source, DStringView8 Search)
{
	return _IndexOf(Source.Data(), Source.Length(), Search.Data(), Search.Length(), 0, Source.Length()) >= 0;
}


cint DString8::CountOf(DStringView8 Search, cint Start, cint Stop) const
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


cint DString8::CountOf(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
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


DString8 DString8::SubstrRange(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	_SubstrRange(this->_Data(), this->Size, Start, Stop, result);
	return result;
}


DString8 DString8::SubstrRange(DStringView8 Source, cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 result;
	_SubstrRange(Source.Data(), Source.Length(), Start, Stop, result);
	return result;
}


DString8 DString8::SubstrCount(cint Start, cint Count) const
{
	cint stop = Start + Count;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, stop, this->Size))
	{
		throw DIndexException(Start, stop, this->Size);
	}
#endif
	DString8 result;
	_SubstrCount(this->_Data(), this->Size, Start, Count, result);
	return result;
}


DString8 DString8::SubstrCount(DStringView8 Source, cint Start, cint Count)
{
	cint stop = Start + Count;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, stop, Source.Length()))
	{
		throw DIndexException(Start, stop, Source.Length());
	}
#endif
	DString8 result;
	_SubstrCount(Source.Data(), Source.Length(), Start, Count, result);
	return result;
}


DString8 DString8::Substr(cint Start) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= this->Size || Start < 0)
	{
		throw DIndexException(Start, this->Size);
	}
#endif
	DString8 result;
	_Substr(this->_Data(), this->Size, Start, result);
	return result;
}


DString8 DString8::Substr(DStringView8 Source, cint Start)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Start >= Source.Length() || Start < 0)
	{
		throw DIndexException(Start, Source.Length());
	}
#endif
	DString8 result;
	_Substr(Source.Data(), Source.Length(), Start, result);
	return result;
}


bool DString8::ReplaceAll(DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	bool ret = _ReplaceAllCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	this->__CopyAssign__(result);
	return ret;
}


bool DString8::ReplaceFirst(DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop)
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


void DString8::ReplaceRange(cint Start, cint Stop, DStringView8 Other)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_ReplaceRange(Start, Stop, Other.Data(), Other.Length());
}


DString8 DString8::AllReplaced(DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	_ReplaceAllCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString8 DString8::AllReplaced(DStringView8 Source, DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 result;
	_ReplaceAllCopy(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString8 DString8::FirstReplaced(DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	_ReplaceFirstCopy(this->_Data(), this->Size, Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString8 DString8::FirstReplaced(DStringView8 Source, DStringView8 Search, DStringView8 Replacement, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 result;
	_ReplaceFirstCopy(Source.Data(), Source.Length(), Search.Data(), Search.Length(), Replacement.Data(), Replacement.Length(), result, Start, Stop);
	return result;
}


DString8 DString8::RangeReplaced(cint Start, cint Stop, DStringView8 Other) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	_ReplaceRangeCopy(this->_Data(), this->Size, Start, Stop, Other.Data(), Other.Length(), result);
	return result;
}


DString8 DString8::RangeReplaced(DStringView8 Source, cint Start, cint Stop, DStringView8 Other)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 result;
	_ReplaceRangeCopy(Source.Data(), Source.Length(), Start, Stop, Other.Data(), Other.Length(), result);
	return result;
}


void DString8::ReplaceRangeWithChar(cint Start, cint Stop, char8 Char)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_ReplaceRangeWithChar(Start, Stop, Char);
}


DString8 DString8::RangeReplacedWithChar(cint Start, cint Stop, char8 Char) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 copy;
	_ReplaceRangeWithCharCopy(this->_Data(), this->Size, Start, Stop, Char, copy);
	return copy;
}


DString8 DString8::RangeReplacedWithChar(DStringView8 Source, cint Start, cint Stop, char8 Char)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 copy;
	_ReplaceRangeWithCharCopy(Source.Data(), Source.Length(), Start, Stop, Char, copy);
	return copy;
}


void DString8::RemoveRange(cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	this->_RemoveRange(Start, Stop);
}


DString8 DString8::RangeRemoved(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	DString8 result;
	_RemoveRangeCopy(this->_Data(), this->Size, Start, Stop, result);
	return result;
}


DString8 DString8::RangeRemoved(DStringView8 Source, cint Start, cint Stop)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	DString8 result;
	_RemoveRangeCopy(Source.Data(), Source.Length(), Start, Stop, result);
	return result;
}


DArray<DString8> DString8::Split(DStringView8 Delimiter) const
{
	if (this->Length() == 0 || Delimiter.Length() == 0) return { *this };
	
	// first item
	cint i = this->IndexOf(Delimiter);
	if (i >= 0)
	{
		DArray<DString8> outArr;
		outArr.Add(this->SubstrRange(0, i));

		// other items
		cint start = i + Delimiter.Length();
		while (true)
		{
			i = this->IndexOf(Delimiter, start);
			if (i >= 0)
			{
				outArr.Add(this->SubstrRange(start, i));
				start = i + Delimiter.Length();
			}
			else break;
		}

		// last item
		if (start < this->Length())
		{
			outArr.Add(this->Substr(start));
		}

		return outArr;
	}
	else return { *this };
}


DArray<DString8> DString8::Split(DStringView8 Source, DStringView8 Delimiter)
{
	if (Source.Length() == 0 || Delimiter.Length() == 0) return { DString8::From(Source) };

	// first item
	cint i = IndexOf(Source, Delimiter);
	if (i >= 0)
	{
		DArray<DString8> outArr;
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
	else return { DString8::From(Source) };
}


DString8 DString8::Join(DStringView8 Delimiter, DArrayView<DString8> Elements)
{
	DString8 out;

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


void DString8::Reverse()
{
	this->_Reverse();
}


DString8 DString8::Reversed() const
{
	DString8 result;
	_Reversed(this->_Data(), this->Size, result);
	return result;
}


DString8 DString8::Reversed(DStringView8 Source)
{
	DString8 result;
	_Reversed(Source.Data(), Source.Length(), result);
	return result;
}

/*______________________________________

CHARACTER ENCODING DEPENDANT
______________________________________*/


bool DString8::EqualsIgnoreCase_Ascii(DStringView8 Other) const
{
	return SAscii::EqualStringsIgnoreCase(this->_Data(), this->Size, Other.Data(), Other.Length());
}


bool DString8::EqualsIgnoreCase_Ascii(DStringView8 A, DStringView8 B)
{
	return SAscii::EqualStringsIgnoreCase(A.Data(), A.Length(), B.Data(), B.Length());
}


cint DString8::CompareTo_Ascii(DStringView8 Other) const
{
	return SAscii::CompareStrings(this->Data(), this->Length(), Other.Data(), Other.Length());
}


cint DString8::Compare_Ascii(DStringView8 A, DStringView8 B)
{
	return SAscii::CompareStrings(A.Data(), B.Length(), A.Data(), B.Length());
}


cint DString8::CompareToIgnoreCase_Ascii(DStringView8 Other) const
{
	return SAscii::CompareStringsIgnoreCase(this->_Data(), this->Size, Other.Data(), Other.Length());
}


cint DString8::CompareIgnoreCase_Ascii(DStringView8 A, DStringView8 B)
{
	return SAscii::CompareStringsIgnoreCase(A.Data(), A.Length(), B.Data(), B.Length());
}


void DString8::UpperCase_Ascii()
{
	SAscii::ToUpperString(this->_Data(), this->Size, this->_Data());
}


void DString8::LowerCase_Ascii()
{
	SAscii::ToLowerString(this->_Data(), this->Size, this->_Data());
}


DString8 DString8::ToUpper_Ascii() const
{
	DString8 out;
	out.Reallocate(this->Size, true);
	SAscii::ToUpperString(this->_Data(), this->Size, out._Data());
	return out;
}


DString8 DString8::ToUpper_Ascii(DStringView8 Source)
{
	DString8 out;
	out.Reallocate(Source.Length(), true);
	SAscii::ToUpperString(Source.Data(), Source.Length(), out._Data());
	return out;
}


DString8 DString8::ToLower_Ascii() const
{
	DString8 out;
	out.Reallocate(this->Size, true);
	SAscii::ToLowerString(this->_Data(), this->Size, out._Data());
	return out;
}


DString8 DString8::ToLower_Ascii(DStringView8 Source)
{
	DString8 out;
	out.Reallocate(Source.Length(), true);
	SAscii::ToLowerString(Source.Data(), Source.Length(), out._Data());
	return out;
}


void DString8::Capitalize_Ascii()
{
	if (this->Size > 0)
	{
		(*this)[0] = SAscii::ToUpper((*this)[0]);
	}
}


DString8 DString8::Capitalized_Ascii() const
{
	DString8 out = *this;
	if (this->Size > 0)
	{
		out[0] = SAscii::ToUpper(out[0]);
	}
	return out;
}


DString8 DString8::Capitalized_Ascii(DStringView8 Source)
{
	DString8 out = DString8::From(Source);
	if (Source.Length() > 0)
	{
		out[0] = SAscii::ToUpper(out[0]);
	}
	return out;
}


cint DString8::IndexOfIgnoreCase_Ascii(DStringView8 Search, cint Start, cint Stop) const
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
		SAscii::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString8::IndexOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
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
		SAscii::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString8::LastIndexOfIgnoreCase_Ascii(DStringView8 Search, cint Start, cint Stop) const
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
		SAscii::CompareIgnoreCase, 
		Start, Stop
	);
}


cint DString8::LastIndexOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
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
		SAscii::CompareIgnoreCase, 
		Start, Stop
	);
}


bool DString8::StartsWithIgnoreCase_Ascii(DStringView8 Prefix) const
{
	return _IndexOfCustomCompare(
		this->_Data(), this->Size, 
		Prefix.Data(), Prefix.Length(), 
		SAscii::CompareIgnoreCase,
		0, this->Size
	) == 0;
}


bool DString8::StartsWithIgnoreCase_Ascii(DStringView8 Source, DStringView8 Prefix)
{
	return _IndexOfCustomCompare(
		Source.Data(), Source.Length(), 
		Prefix.Data(), Prefix.Length(), 
		SAscii::CompareIgnoreCase,
		0, Source.Length()
	) == 0;
}


bool DString8::EndsWithIgnoreCase_Ascii(DStringView8 Suffix) const
{
	return _LastIndexOfCustomCompare(
		this->_Data(), this->Size, 
		Suffix.Data(), Suffix.Length(),
		SAscii::CompareIgnoreCase,
		this->Size - 1, 0
	) == this->Size - Suffix.Length();
}


bool DString8::EndsWithIgnoreCase_Ascii(DStringView8 Source, DStringView8 Suffix)
{
	return _LastIndexOfCustomCompare(
		Source.Data(), Source.Length(), 
		Suffix.Data(), Suffix.Length(),
		SAscii::CompareIgnoreCase,
		Source.Length() - 1, 0
	) == Source.Length() - Suffix.Length();
}


bool DString8::ContainsIgnoreCase_Ascii(DStringView8 Search) const
{
	return _IndexOfCustomCompare(
		this->_Data(), this->Size, 
		Search.Data(), Search.Length(),
		SAscii::CompareIgnoreCase,
		0, this->Size
	) >= 0;
}


bool DString8::ContainsIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search)
{
	return _IndexOfCustomCompare(
		Source.Data(), Source.Length(), 
		Search.Data(), Search.Length(),
		SAscii::CompareIgnoreCase,
		0, Source.Length()
	) >= 0;
}


void DString8::RemovePadding_Ascii()
{
	char8* arr = this->_Data();

	cint prefix = 0;
	for (cint i = 0; i < this->Size; i++)
	{
		if (SAscii::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = Size - 1; i >= 0; i--)
	{
		if (SAscii::IsWhitespace(arr[i])) suffix++;
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


DString8 DString8::PaddingRemoved_Ascii() const
{
	const char8* arr = this->_Data();

	cint prefix = 0;
	for (cint i = 0; i < this->Size; i++)
	{
		if (SAscii::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = Size - 1; i >= 0; i--)
	{
		if (SAscii::IsWhitespace(arr[i])) suffix++;
		else break;
	}

	cint charsToKeep = this->Size - prefix - suffix;
	DString8 out;
	out.Append(DStringView8(arr + prefix, charsToKeep));
	return out;
}


DString8 DString8::PaddingRemoved_Ascii(DStringView8 Source)
{
	const char8* arr = Source.Data();

	cint prefix = 0;
	for (cint i = 0; i < Source.Length(); i++)
	{
		if (SAscii::IsWhitespace(arr[i])) prefix++;
		else break;
	}

	cint suffix = 0;
	for (cint i = Source.Length() - 1; i >= 0; i--)
	{
		if (SAscii::IsWhitespace(arr[i])) suffix++;
		else break;
	}

	cint charsToKeep = Source.Length() - prefix - suffix;
	DString8 out;
	out.Append(DStringView8(arr + prefix, charsToKeep));
	return out;
}


cint DString8::CountOfIgnoreCase_Ascii(DStringView8 Search, cint Start, cint Stop) const
{
	if (Stop < 0) Stop = this->Size;
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return _CountOfCustomCompare(this->_Data(), this->Size, Search.Data(), Search.Length(), SAscii::CompareIgnoreCase, Start, Stop);
}


cint DString8::CountOfIgnoreCase_Ascii(DStringView8 Source, DStringView8 Search, cint Start, cint Stop)
{
	if (Stop < 0) Stop = Source.Length();
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, Source.Length()))
	{
		throw DIndexException(Start, Stop, Source.Length());
	}
#endif
	return _CountOfCustomCompare(Source.Data(), Source.Length(), Search.Data(), Search.Length(), SAscii::CompareIgnoreCase, Start, Stop);
}


bool DString8::IsBlank_Ascii() const
{
	if (this->Size == 0) return true;
	const char8* data = this->_Data();
	for (cint i = 0; i < this->Size; ++i)
	{
		if (SAscii::IsWhitespace(data[i]) == false) return false; 
	}
	return true;
}


bool DString8::IsBlank_Ascii(DStringView8 Source)
{
	if (Source.Length() == 0) return true;
	const char8* data = Source.Data();
	for (cint i = 0; i < Source.Length(); ++i)
	{
		if (SAscii::IsWhitespace(data[i]) == false) return false; 
	}
	return true;
}

/*___________________________

UTIL
___________________________*/

void DString8::Clear()
{
	this->_Clear();
}


bool DString8::IsEmpty() const
{
	return this->Size < 1;
}


void DString8::Reserve(cint NewCapacity)
{
	this->_Reserve(NewCapacity);
}


void DString8::ReserveExtra(cint ExtraCapacity)
{
	this->_Reserve(this->Size + ExtraCapacity);
}


void DString8::Resize(cint NewSize, char8 PaddingChar)
{
	this->_Resize(NewSize, PaddingChar);
}


void DString8::Reallocate(cint NewCapacity, bool CapacityAsLength)
{
	this->_Reallocate(NewCapacity, CapacityAsLength);
}


void DString8::OverwriteLengthUnsafe(cint NewSize)
{
	this->Size = NewSize;
	this->_Data()[NewSize] = '\0';
}


const char8* DString8::CString() const
{
	return this->_Data();
}


const char8* DString8::Data() const
{
	return this->_Data();
}


char8* DString8::GetDataUnsafe()
{
	return this->_Data();
}


cint DString8::Capacity() const
{
	return this->_Capacity();
}


// enhanced for loop
char8* DString8::begin() { return this->_Data(); };
const char8* DString8::begin() const { return this->_Data(); }
char8* DString8::end() { return this->_Data() + this->Size; }
const char8* DString8::end() const { return this->_Data() + this->Size; }


DString8::operator DStringView8() const
{
	return DStringView8(this->_Data(), this->Size);
}


DStringView8 DString8::ViewRange(cint Start, cint Stop) const
{
#ifdef LANG_DEBUG_CONTAINERS
	if (SCounterUtil::RangeError(Start, Stop, this->Size))
	{
		throw DIndexException(Start, Stop, this->Size);
	}
#endif
	return DStringView8(this->_Data() + Start, Stop - Start);
}


/*___________________________

PARSING
___________________________*/


int32 DString8::ParseInt32(DStringView8 Buffer)
{
	return std::atoi(Buffer.Data());
}


int64 DString8::ParseInt64(DStringView8 Buffer)
{
	return std::atoll(Buffer.Data());
}


float DString8::ParseFloat(DStringView8 Buffer)
{
	return (float)std::atof(Buffer.Data());
}


double DString8::ParseDouble(DStringView8 Buffer)
{
	return std::atof(Buffer.Data());
}


DString8 operator+(DStringView8 Left, DStringView8 Right)
{
	DString8 out (Left.Length() + Right.Length());
	out.Append(Left);
	out.Append(Right);
	return out;
}


cint std::hash<DString8>::operator()(const DString8& Str) const
{
	return SCounterUtil::PrimitiveCArrayHashCode<char8>(Str.Data(), Str.Length());
}
