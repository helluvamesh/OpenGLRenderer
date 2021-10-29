#include "pch.h"
#include "SStrictSerializer.h"
#include "SObjectTraveller.h"
#include "SObjectConverter.h"
#include "SStrictObjectReconverter.h"


cint SStrictSerializer::GetSerializedByteCountOfObject(const shared<HObject>& Obj)
{
    return SObjectTraveller<true, byte, false>::GetByteCount(Obj.Ptr(), Obj->GetClass());
}


cint SStrictSerializer::SerializeObject(shared<HObject> Obj, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SObjectConverter<true, byte, false>::ObjectToBytes(Obj.Ptr(), Obj->GetClass(), OutBytes, LengthAsCapacity);
    }
    else
    {
        return SObjectConverter<false, byte, false>::ObjectToBytes(Obj.Ptr(), Obj->GetClass(), OutBytes, LengthAsCapacity);
    }
}


cint SStrictSerializer::DeserializeObject(shared<HObject> Obj, const byte* Bytes, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SStrictObjectReconverter<true, byte>::BytesToObject(Bytes, Obj.Ptr(), Obj->GetClass());
    }
    else
    {
        return SStrictObjectReconverter<false, byte>::BytesToObject(Bytes, Obj.Ptr(), Obj->GetClass());
    }
}


cint SStrictSerializer::GetSerializedByteCountOfStruct(const DStruct& Obj, const DClass& Cls)
{
    return SObjectTraveller<true, byte, false>::GetByteCount(&Obj, Cls);
}


cint SStrictSerializer::SerializeStruct(const DStruct& Obj, const DClass& Cls, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SObjectConverter<true, byte, false>::ObjectToBytes(&Obj, Cls, OutBytes, LengthAsCapacity);
    }
    else
    {
        return SObjectConverter<false, byte, false>::ObjectToBytes(&Obj, Cls, OutBytes, LengthAsCapacity);
    }
}


cint SStrictSerializer::DeserializeStruct(DStruct& Obj, const DClass& Cls, const byte* Bytes, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SStrictObjectReconverter<true, byte>::BytesToObject(Bytes, &Obj, Cls);
    }
    else
    {
        return SStrictObjectReconverter<false, byte>::BytesToObject(Bytes, &Obj, Cls);
    }
}