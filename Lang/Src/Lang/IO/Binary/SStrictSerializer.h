/*
* StrictSerializer
* - converts arbitrary data to a byte array and back
* - uses reflection system, read ReflectionSystem.txt
* - byte array contains no meta data
* - deserializer expects the layout of reflection data and that of the byte array to be EXACTLY the same when reading back
* - this gives maximum speed
* - and minimal file size
* - but entails severe version control
* - if speed or size is not a concern, use LenientSerializer instead
*/

#pragma once


class SStrictSerializer
{
public:
    
    static cint GetSerializedByteCountOfObject(const shared<HObject>& Obj);

    // returns number of bytes written
    static cint SerializeObject(shared<HObject> Obj, byte* OutBytes, bool LengthAsCapacity = true, bool LittleEndian = true);

    // returns number of bytes read
    static cint DeserializeObject(shared<HObject> Obj, const byte* Bytes, bool LittleEndian = true);


    static cint GetSerializedByteCountOfStruct(const DStruct& Obj, const DClass& Cls);    

    // returns number of bytes written
    static cint SerializeStruct(const DStruct& Obj, const DClass& Cls, byte* OutBytes, bool LengthAsCapacity = true, bool LittleEndian = true);

    // returns number of bytes read
    static cint DeserializeStruct(DStruct& Obj, const DClass& Cls, const byte* Bytes, bool LittleEndian = true);

};