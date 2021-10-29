#pragma once

#include "DCSVSyntax.h"
#include "..\JSON\DJsonParser.h"


class DCsvParser
{
private:

    DCsvSyntax Syntax;
    DJsonParser JSONParser;


    cint ParseNextField(const DString8& Str, cint Start, DString8& Out);

    cint ParseBoolean(const DString8& FieldStr, bool& Value);

    cint ParseHeader(const DString8& Str, cint Start, DArray<DString8>& OutHeader);
    
    cint ParseRecord(DStruct* Obj, const DClass& Cls, DArray<DString8> Header, const DString8& CSVString, cint Start);

    template<typename OT>
    bool Parse(OT* Obj, const DClass& Cls, const DString8 FieldName, const DString8& CSVString, cint Start = 0);

public:

    DCsvParser();
    DCsvParser(const DCsvSyntax& InSyntax);
    
    // FieldName: DMap<DString, T> or DArray<T> where T extends DStruct
    // does not clear container before loading!
    // returns success
    bool ParseField(shared<HObject> Obj, const DString8& FieldName, const DString8& CSVString, cint Start = 0);

    // FieldName: DMap<DString, T> or DArray<T> where T extends DStruct
    // does not clear container before loading!
    // returns success
    bool ParseField(DStruct& Obj, const DClass& Cls, const DString8& FieldName, const DString8& CSVString, cint Start = 0);


};