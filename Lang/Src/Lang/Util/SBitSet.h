#pragma once


class SBitSet
{
public:

	static bool Get(uint8 Flags, uint8 Index);
	static void Set(uint8& Flags, uint8 Index, bool Value);
	static bool OverlapTRUE(uint8 FlagsA, uint8 FlagsB);
	static DString ToString(uint8 Flags);
	
	static bool Get(uint16 Flags, uint8 Index);
	static void Set(uint16& Flags, uint8 Index, bool Value);
	static bool OverlapTRUE(uint16 FlagsA, uint16 FlagsB);
	static DString ToString(uint16 Flags);

	static bool Get(uint32 Flags, uint8 Index);
	static void Set(uint32& Flags, uint8 Index, bool Value);
	static bool OverlapTRUE(uint32 FlagsA, uint32 FlagsB);
	static DString ToString(uint32 Flags);
	
	static bool Get(uint64 Flags, uint8 Index);
	static void Set(uint64& Flags, uint8 Index, bool Value);
	static bool OverlapTRUE(uint64 FlagsA, uint64 FlagsB);
	static DString ToString(uint64 Flags);

};
