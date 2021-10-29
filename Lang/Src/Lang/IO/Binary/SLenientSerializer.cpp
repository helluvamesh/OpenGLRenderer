#include "pch.h"
#include "SLenientSerializer.h"
#include "SObjectTraveller.h"
#include "SObjectConverter.h"
#include "SLenientObjectReconverter.h"


cint SLenientSerializer::GetSerializedByteCountOfObject(shared<HObject> Obj)
{
    return SObjectTraveller<true, byte, true>::GetByteCount(Obj.Ptr(), Obj->GetClass());
}


cint SLenientSerializer::SerializeObject(shared<HObject> Obj, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SObjectConverter<true, byte, true>::ObjectToBytes(Obj.Ptr(), Obj->GetClass(), OutBytes, LengthAsCapacity);
    }
    else
    {
        return SObjectConverter<false, byte, true>::ObjectToBytes(Obj.Ptr(), Obj->GetClass(), OutBytes, LengthAsCapacity);
    }
}


cint SLenientSerializer::DeserializeObject(shared<HObject> Obj, const byte* Bytes, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SLenientObjectReconverter<true, byte>::BytesToObject(Bytes, Obj.Ptr(), Obj->GetClass());
    }
    else
    {
        return SLenientObjectReconverter<false, byte>::BytesToObject(Bytes, Obj.Ptr(), Obj->GetClass());
    }
}


cint SLenientSerializer::GetSerializedByteCountOfStruct(const DStruct& Obj, const DClass& Cls)
{
    return SObjectTraveller<true, byte, true>::GetByteCount(&Obj, Cls);
}


cint SLenientSerializer::SerializeStruct(const DStruct& Obj, const DClass& Cls, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SObjectConverter<true, byte, true>::ObjectToBytes(&Obj, Cls, OutBytes, LengthAsCapacity);
    }
    else
    {
        return SObjectConverter<false, byte, true>::ObjectToBytes(&Obj, Cls, OutBytes, LengthAsCapacity);
    }
}


cint SLenientSerializer::DeserializeStruct(DStruct& Obj, const DClass& Cls, const byte* Bytes, bool LittleEndian)
{
    if (SBinaryInfo::ShouldKeepEndiannes(LittleEndian))
    {
        return SLenientObjectReconverter<true, byte>::BytesToObject(Bytes, &Obj, Cls);
    }
    else
    {
        return SLenientObjectReconverter<false, byte>::BytesToObject(Bytes, &Obj, Cls);
    }
}
