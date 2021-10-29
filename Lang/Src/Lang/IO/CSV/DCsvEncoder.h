#pragma once

#include "DCsvSyntax.h"
#include "../JSON/DJsonEncoder.h"


class DCsvEncoder
{
private:
    
    DCsvSyntax Syntax;
    DNumberFormat8 NumberFormatter;
    DString8 Escape;
    DString8 EscapeRelacement;
    DJsonEncoder JSONEncoder;


    void AppendStringToCSVString(const DString8& Src, DString8& Out);

    void AppendHeader(const DClass& Cls, bool IsMapHeader, DString8& Out);
    
    // Key: if not nullptr, added as the first field
    void EncodeRecord(const DStruct* Obj, const DClass& Cls, const DString* Key, DString8& Out);

    template<typename OT>
    bool Encode(const OT* Obj, const DClass& Cls, const DString8& FieldName, DString8& Out);

public:

    DCsvEncoder();
    DCsvEncoder(const DCsvSyntax& InSyntax);

    // FieldName: DMap<DString, T> or DArray<T> where T extends Struct
    // returns success
    bool EncodeField(shared<HObject> Obj, const DString8& FieldName, DString8& Out);

    // FieldName: DMap<DString, T> or DArray<T> where T extends Struct
    // returns success
    bool EncodeField(const DStruct& Obj, const DClass& Cls, const DString8& FieldName, DString8& Out);

};