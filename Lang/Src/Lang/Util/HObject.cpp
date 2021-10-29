#include "pch.h"
#include "HObject.h"


DClass HObject::ClassData("HObject", nullptr);


bool HObject::StaticInit = true;


DClass& HObject::GetDeclaredClass()
{
	return ClassData;
}


DClass& HObject::GetClass() const
{
	return ClassData;
}


DString HObject::ToString() const
{
	DString str = DStrings::UTF8ToUTF16(this->GetClass().GetClassName());
	str += TX(" object at ");
	DNumberFormat::DEFAULT.AppendTo((uint64)std::hash<const void*>{}(this), str);
	return str;
}