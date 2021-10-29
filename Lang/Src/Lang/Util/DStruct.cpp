#include "pch.h"
#include "DStruct.h"


DClass DStruct::ClassData("DStruct", nullptr);


bool DStruct::StaticInit = true;


DClass& DStruct::GetDeclaredClass()
{
	return ClassData;
}


DClass& DStruct::GetClass() const
{
	return ClassData;
}