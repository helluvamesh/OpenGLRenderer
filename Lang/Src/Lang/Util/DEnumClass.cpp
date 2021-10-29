#include "pch.h"
#include "DEnumClass.h"


DMap<const char*, DEnumClass> DEnumClass::EnumClasses;

const DString8 DEnumClass::UNKNOWN_ENUM_VALUE = "UNKNOWN_ENUM_VALUE";


DEnumClass* DEnumClass::FindEnumClass(const char* ID)
{
	return EnumClasses.Find(ID);
}


DEnumClass::DEnumClass(const char* ID, const char8* InItemString, uint8 InLength) 
{
	this->ItemString = InItemString;
	this->Length = InLength;
	EnumClasses.Emplace(ID, *this);
}


const DString8& DEnumClass::GetUnknownEnumName()
{
	return UNKNOWN_ENUM_VALUE;
}


uint8 DEnumClass::GetLength()
{
	return this->Length;
}


const DArray<DString8>& DEnumClass::GetItemNames()
{
	if (this->ItemNames.Length() == 0)
	{
		this->ItemNames = DString8(this->ItemString).Split(u8", ");
	}
	return this->ItemNames;
}


const DString8& DEnumClass::GetItemName(uint8 ItemValue)
{
	const DArray<DString8>& items = this->GetItemNames();
	if (items.IsValidIndex(ItemValue))
	{
		return items[ItemValue];
	}
	return UNKNOWN_ENUM_VALUE;
}


uint8 DEnumClass::GetItemByName(const DString8& ItemName)
{
	cint i = this->GetItemNames().IndexOf(ItemName);
	return i >= 0? ((uint8)i) : 0;
}
