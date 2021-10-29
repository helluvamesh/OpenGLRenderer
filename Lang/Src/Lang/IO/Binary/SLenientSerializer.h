/*
* LenientSerializer
* - converts arbitrary data to a byte array and back
* - uses reflection system, read ReflectionSystem.txt
* - byte array contains type information about every field
* - deserializer reads back what it can, skips the rest
* - the layout of reflection data and that of the byte array can have differences
* - this causes speed degradation
* - and larger file size
* - if speed or size is a concern, use StrictSerializer instead
*/

#pragma once


class SLenientSerializer
{
public:

    static cint GetSerializedByteCountOfObject(shared<HObject> Obj);

    // returns number of bytes written
    static cint SerializeObject(shared<HObject> Obj, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian = true);
    
    // returns number of bytes read
    static cint DeserializeObject(shared<HObject> Obj, const byte* Bytes, bool LittleEndian = true);


    static cint GetSerializedByteCountOfStruct(const DStruct& Obj, const DClass& Cls);

    // returns number of bytes written
    static cint SerializeStruct(const DStruct& Obj, const DClass& Cls, byte* OutBytes, bool LengthAsCapacity, bool LittleEndian = true);

    // returns number of bytes read
    static cint DeserializeStruct(DStruct& Obj, const DClass& Cls, const byte* Bytes, bool LittleEndian = true);

};