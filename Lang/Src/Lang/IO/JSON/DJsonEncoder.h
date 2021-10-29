/*
* JsonEncoder
* - converts arbitrary data into a utf-8 encoded JSON string 
* - JSON specification: https://www.json.org/json-en.html
* - uses reflection system, read ReflectionSystem.txt
* - output string is
* - human readable / editable
* - indented or not
* - enums are represented as string
*/

#pragma once


class DJsonEncoder
{
private:
    
    DString8* JStr = nullptr;
    
    bool Indent = true;
    cint IndentLevel = 0;
    cint IndentStep = 4;


    FORCEINLINE void DoIndent();

    FORCEINLINE void AppendFieldName(bool NotFirst, const DString8& Key);

    FORCEINLINE void StartObject();

    FORCEINLINE void CloseObject(cint Length);

    FORCEINLINE void StartArray();

    FORCEINLINE void CloseArray(cint Length);

    FORCEINLINE void NewItem(bool NotFirst);

    FORCEINLINE void NewMapKey(bool NotFirst);

    FORCEINLINE void NewMapValue();
    
    template<typename OT>
    void FieldToJsonString(const OT* Obj, const DClass& Cls, const DString8& fieldName, const DField* f, bool AppendKey, bool NotFirst);

    template<typename OT>
    void ObjectToJsonString(const OT* Obj, const DClass& Cls);
    
    void ResetState();

public:

    void EncodeObject(shared<HObject> Obj, DString8& OutStr);

    void EncodeStruct(const DStruct& Obj, const DClass& Cls, DString8& OutStr);

    void EncodeObjectField(shared<HObject> Obj, const DString8& FieldName, DString8& OutStr);

    void EncodeStructField(const DStruct& Obj, const DClass& Cls, const DString8& FieldName, DString8& OutStr);

    void SetIndentation(cint Step);

};