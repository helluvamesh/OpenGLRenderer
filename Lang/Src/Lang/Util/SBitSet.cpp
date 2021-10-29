#include "pch.h"
#include "SBitSet.h"


// ----------- uint8 ------------


bool SBitSet::Get(uint8 Flags, uint8 Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 8 || Index < 0)
	{
		throw DIndexException(Index, 8);
	}
#endif

	return (Flags >> Index) & 1U;
}


void SBitSet::Set(uint8& Flags, uint8 Index, bool Value)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 8 || Index < 0)
	{
		throw DIndexException(Index, 8);
	}
#endif

	if (Value)
	{
		Flags |= 1U << Index;
	}
	else
	{
		Flags &= ~(1U << Index);
	}
}


bool SBitSet::OverlapTRUE(uint8 A, uint8 B) 
{
	for (uint8 i = 0; i < 32U; ++i)
	{
		bool a = (bool)((A >> i) & 1U);
		bool b = (bool)((B >> i) & 1U);
		if (a && b) return true;
	}
	return false;
}


DString SBitSet::ToString(uint8 Flags)
{
	cint bitCount = 8;
	DString s;
	s.Reserve(8 + bitCount * 2 - 1);
	s.Append(TX("SBitSet("));
	cint last_i = bitCount - 1;
	for (cint i = 0; i < bitCount; ++i)
	{
		s.Append(
			SBitSet::Get(Flags, i)? TX("1") : TX("0")
		);
		if (i < last_i)
		{
			s.Append(TX(" "));
		}
	}
	s.Append(TX(")"));
	return s;
}


// ----------- uint16 ------------


bool SBitSet::Get(uint16 Flags, uint8 Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 16 || Index < 0)
	{
		throw DIndexException(Index, 8);
	}
#endif

	return (Flags >> Index) & 1U;
}


void SBitSet::Set(uint16& Flags, uint8 Index, bool Value)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 16 || Index < 0)
	{
		throw DIndexException(Index, 16);
	}
#endif

	if (Value)
	{
		Flags |= 1U << Index;
	}
	else
	{
		Flags &= ~(1U << Index);
	}
}


bool SBitSet::OverlapTRUE(uint16 A, uint16 B) 
{
	for (uint8 i = 0; i < 32U; ++i)
	{
		bool a = (bool)((A >> i) & 1U);
		bool b = (bool)((B >> i) & 1U);
		if (a && b) return true;
	}
	return false;
}


DString SBitSet::ToString(uint16 Flags)
{
	cint bitCount = 16;
	DString s;
	s.Reserve(8 + bitCount * 2 - 1);
	s.Append(TX("SBitSet("));
	cint last_i = bitCount - 1;
	for (cint i = 0; i < bitCount; ++i)
	{
		s.Append(
			SBitSet::Get(Flags, i)? TX("1") : TX("0")
		);
		if (i < last_i)
		{
			s.Append(TX(" "));
		}
	}
	s.Append(TX(")"));
	return s;
}


// ----------- uint32 ------------


bool SBitSet::Get(uint32 Flags, uint8 Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 32 || Index < 0)
	{
		throw DIndexException(Index, 8);
	}
#endif

	return (Flags >> Index) & 1U;
}


void SBitSet::Set(uint32& Flags, uint8 Index, bool Value)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 32 || Index < 0)
	{
		throw DIndexException(Index, 32);
	}
#endif

	if (Value)
	{
		Flags |= 1U << Index;
	}
	else
	{
		Flags &= ~(1U << Index);
	}
}


bool SBitSet::OverlapTRUE(uint32 A, uint32 B) 
{
	for (uint8 i = 0; i < 32U; ++i)
	{
		bool a = (bool)((A >> i) & 1U);
		bool b = (bool)((B >> i) & 1U);
		if (a && b) return true;
	}
	return false;
}


DString SBitSet::ToString(uint32 Flags)
{
	cint bitCount = 32;
	DString s;
	s.Reserve(8 + bitCount * 2 - 1);
	s.Append(TX("SBitSet("));
	cint last_i = bitCount - 1;
	for (cint i = 0; i < bitCount; ++i)
	{
		s.Append(
			SBitSet::Get(Flags, i)? TX("1") : TX("0")
		);
		if (i < last_i)
		{
			s.Append(TX(" "));
		}
	}
	s.Append(TX(")"));
	return s;
}


// ----------- uint64 ------------


bool SBitSet::Get(uint64 Flags, uint8 Index)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 64 || Index < 0)
	{
		throw DIndexException(Index, 8);
	}
#endif

	return (Flags >> Index) & 1ULL;
}


void SBitSet::Set(uint64& Flags, uint8 Index, bool Value)
{
#ifdef LANG_DEBUG_CONTAINERS
	if (Index >= 64 || Index < 0)
	{
		throw DIndexException(Index, 64);
	}
#endif

	if (Value)
	{
		Flags |= 1ULL << Index;
	}
	else
	{
		Flags &= ~(1ULL << Index);
	}
}


bool SBitSet::OverlapTRUE(uint64 A, uint64 B) 
{
	for (uint8 i = 0; i < 32U; ++i)
	{
		bool a = (bool)((A >> i) & 1ULL);
		bool b = (bool)((B >> i) & 1ULL);
		if (a && b) return true;
	}
	return false;
}


DString SBitSet::ToString(uint64 Flags)
{
	cint bitCount = 64;
	DString s;
	s.Reserve(8 + bitCount * 2 - 1);
	s.Append(TX("SBitSet("));
	cint last_i = bitCount - 1;
	for (cint i = 0; i < bitCount; ++i)
	{
		s.Append(
			SBitSet::Get(Flags, i)? TX("1") : TX("0")
		);
		if (i < last_i)
		{
			s.Append(TX(" "));
		}
	}
	s.Append(TX(")"));
	return s;
}
