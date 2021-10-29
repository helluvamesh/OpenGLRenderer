#include "pch.h"
#include "DClass.h"


DField::DField(EArchType InArchType) :
	ArchType(InArchType), 
	ItemType(EItemType::BOOL),
	KeyType(EKeyType::INT)
{
}


DField::DField(EArchType InArchType, EItemType InItemType) :
	ArchType(InArchType),
	ItemType(InItemType),
	KeyType(EKeyType::INT)
{
}


DField::DField(EArchType InArchType, EKeyType InKeyType) :
	ArchType(InArchType),
	ItemType(EItemType::INT),
	KeyType(InKeyType)
{
}


DField::DField(EArchType InArchType, EItemType InItemType, EKeyType InKeyType) : 
	ArchType(InArchType),
	ItemType(InItemType),
	KeyType(InKeyType)
{
}


// -------------- FieldEnumerator ----------------


DClass::FieldEnumerator::FieldEnumerator(const DClass* InCls)
	: Cls(InCls), Etor(InCls->FieldArray)
{
}

const DMapEntry<DString8, DField*>* DClass::FieldEnumerator::Next()
{
	if (auto* node = this->Etor.Next())
	{
		return *node;
	}
	else
	{
		const DClass* superClass = this->Cls->SuperClass;
		if (superClass)
		{
			this->Cls = superClass;
			this->Etor = superClass->FieldArray.GetConstEnumerator();
			auto* next = this->Etor.Next();
			return next? *next : nullptr;
		}
	}
	return nullptr;
}


// ------------------- Class ---------------------


DClass::DClass(const DString8& InClassName, DClass* InSuperClass)
{
	this->ClassName = InClassName;
	this->SuperClass = InSuperClass;
}


void DClass::AddField(const DString8& InName, DField* InField)
{
	this->FieldArray.Add(
		this->Fields.Emplace(InName, InField)
	);
}


DClass* DClass::Super()
{
	return this->SuperClass;
}


const DClass* DClass::Super() const
{
	return this->SuperClass;
}


const DString8& DClass::GetClassName() const
{
	return this->ClassName;
}


const DField* DClass::FindField(const DString8& Name) const
{
	const DField* field = this->Fields.FindPtr(Name);
	if (field)
	{
		return field;
	}
	else if (this->SuperClass)
	{
		return this->SuperClass->FindField(Name);
	}
	return nullptr;
}


DFieldEnumerator DClass::GetFieldEnumerator() const
{
	return DFieldEnumerator(this);
}


cint DClass::GetFieldCount() const
{
	cint total = this->Fields.Length();
	const DClass* cls = this;
	while (cls = cls->SuperClass)
	{
		total += cls->Fields.Length();
	}
	return total;
}
