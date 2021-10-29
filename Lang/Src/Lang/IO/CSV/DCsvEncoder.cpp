#include "pch.h"
#include "DCsvEncoder.h"
#include "SCsvUtils.h"


DCsvEncoder::DCsvEncoder()
{
    this->Escape.Append(this->Syntax.EscapeChar);
    this->EscapeRelacement.Append(this->Syntax.EscapeChar);
    this->EscapeRelacement.Append(this->Syntax.EscapeChar);
    this->JSONEncoder.SetIndentation(0);
}


DCsvEncoder::DCsvEncoder(const DCsvSyntax& InSyntax)
{
    this->Syntax = InSyntax;
    this->Escape.Append(InSyntax.EscapeChar);
    this->EscapeRelacement.Append(InSyntax.EscapeChar);
    this->EscapeRelacement.Append(InSyntax.EscapeChar);
    this->JSONEncoder.SetIndentation(0);
}


void DCsvEncoder::AppendStringToCSVString(const DString8& Src, DString8& Out)
{
    bool containsQuote = false;
    bool containsDelimiter = false;
    bool containsLineBreak = false;

    cint len = Src.Length();
    for (cint i = 0; i < len; ++i)
    {
        char8 c = Src[i];
        if (c == this->Syntax.EscapeChar) containsQuote = true;
        else if (c == this->Syntax.Delimiter) containsDelimiter = true;
        else if (c == '\n') containsLineBreak = true;
    }
    
    if (containsQuote)
    {
        Out.ReserveExtra(2 + Src.Length());
        Out.Append(this->Syntax.EscapeChar);
        Out.Append(Src.AllReplaced(this->Escape, this->EscapeRelacement));
        Out.Append(this->Syntax.EscapeChar);
    }
    else if (containsDelimiter || containsLineBreak || this->Syntax.ExportQuoteAllFields)
    {
        Out.ReserveExtra(2 + Src.Length());
        Out.Append(this->Syntax.EscapeChar);
        Out.Append(Src);
        Out.Append(this->Syntax.EscapeChar);
    }
    else
    {
        Out.Append(Src);
    }
};


void DCsvEncoder::AppendHeader(const DClass& Cls, bool IsMapHeader, DString8& Out)
{
    if (IsMapHeader)
    {
        Out.Append(this->Syntax.ExportKeyName);
        Out.Append(this->Syntax.Delimiter);
    }

    DFieldEnumerator etor = Cls.GetFieldEnumerator();
    while (auto* entry = etor.Next())
    {
        DString8 fieldName = entry->GetKey();
        AppendStringToCSVString(fieldName, Out);
        Out.Append(this->Syntax.Delimiter);
    }

    // remove final delimiter
    if (Out.Length() > 0 && Out[Out.Length() - 1] == this->Syntax.Delimiter)
    {
        Out.ReplaceRange(Out.Length() - 1, Out.Length(), "");
    }
}


void DCsvEncoder::EncodeRecord(const DStruct* Obj, const DClass& Cls, const DString* Key, DString8& Out)
{
    cint i = 0;

    if (Key)
    {
        this->AppendStringToCSVString(DStrings::UTF16ToUTF8(*Key), Out);
        i++;
    }

    DFieldEnumerator etor = Cls.GetFieldEnumerator();
    while (auto* entry = etor.Next())
    {
        const DString8& fieldName = entry->GetKey();
        const DField* f = entry->GetValue();
        
        switch (f->ArchType)
        {
        case EArchType::BOOL:
        {
            const DBoolField<DStruct>* field = static_cast<const DBoolField<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            Out.Append(
                (*field->GetPtr(Obj))? this->Syntax.TrueStr : this->Syntax.FalseStr
            );
            break;
        }
        case EArchType::ENUM:
        {
            const DEnumField<DStruct, uint8>* field = static_cast<const DEnumField<DStruct, uint8>*>(f);
            DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            if (enumClass)
            {
                Out.Append(
                    enumClass->GetItemName(*field->GetPtr(Obj))
                );
            }
            else
            {
                Out.Append(
                    DEnumClass::GetUnknownEnumName()
                );
            }
            break;
        }
        case EArchType::INT:
        {
            const DIntField<DStruct>* field = static_cast<const DIntField<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->NumberFormatter.AppendTo(
                *field->GetPtr(Obj),
                Out
            );
            break;
        }
        case EArchType::INT64:
        {
            const DInt64Field<DStruct>* field = static_cast<const DInt64Field<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->NumberFormatter.AppendTo(
                *field->GetPtr(Obj),
                Out
            );
            break;
        }
        case EArchType::FLOAT:
        {
            const DFloatField<DStruct>* field = static_cast<const DFloatField<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->NumberFormatter.AppendTo(
                *field->GetPtr(Obj), 
                Out
            );
            break;
        }
        case EArchType::DOUBLE:
        {
            const DDoubleField<DStruct>* field = static_cast<const DDoubleField<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->NumberFormatter.AppendTo(
                *field->GetPtr(Obj),
                Out
            );
            break;
        }
        case EArchType::STRING:
        {
            const DStringField<DStruct>* field = static_cast<const DStringField<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->AppendStringToCSVString(
                DStrings::UTF16ToUTF8(*field->GetPtr(Obj)),
                Out
            );
            break;
        }
        case EArchType::STRING8:
        {
            const DString8Field<DStruct>* field = static_cast<const DString8Field<DStruct>*>(f);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->AppendStringToCSVString(
                *field->GetPtr(Obj),
                Out
            );
            break;
        }
        case EArchType::FLOAT_STRUCT:
        case EArchType::STRUCT:
        case EArchType::ARRAY:
        case EArchType::SET:
        case EArchType::MAP:
        {
            DString8 jstr;
            this->JSONEncoder.EncodeStructField(*Obj, Cls, fieldName, jstr);
            if (i++ > 0) Out.Append(this->Syntax.Delimiter);
            this->AppendStringToCSVString(jstr, Out);
            break;
        }
        }
    }

    if (i > 0)
    {
        Out.Append('\n');
    }
}


template<typename OT>
bool DCsvEncoder::Encode(const OT* Obj, const DClass& Cls, const DString8& FieldName, DString8& Out)
{
    const DField* f = Cls.FindField(FieldName);
    if (!f) 
    {
        return false;
    }

    if (f->ArchType == EArchType::MAP)
    {
        if (f->KeyType == EKeyType::STRING && f->ItemType == EItemType::STRUCT)
        {
            auto* field = static_cast<const DStructMapField<OT, DString, DStruct>*>(f);
            DMap<DString, DStruct>* map = field->GetPtr(Obj);
            const DClass& structClass = field->GetItemStructClass();

            this->AppendHeader(structClass, true, Out);
            Out.Append('\n');

            for (auto& entry : *map)
            {
                this->EncodeRecord(&entry.GetValue(), structClass, &entry.GetKey(), Out);
            }
        }
        else
        {
            return false;
        }
    }
    else if (f->ArchType == EArchType::ARRAY)
    {
        if (f->ItemType == EItemType::STRUCT)
        {
            auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
            DArray<DStruct>* arr = field->GetPtr(Obj);
            const DClass& structClass = field->GetStructClass();

            this->AppendHeader(structClass, false, Out);
            Out.Append('\n');

            cint len = arr->Length();
            cint structByteCount = field->GetItemByteCount();
            const DStruct* arrStart = arr->Data();
            for (cint i = 0; i < len; ++i)
            {
                this->EncodeRecord(arrStart + (size_t)(structByteCount) * i, structClass, /*key*/nullptr, Out);
            }
        }
        else
        {
            return true;
        }
    }

    return true;
}


bool DCsvEncoder::EncodeField(shared<HObject> Obj, const DString8& FieldName, DString8& Out)
{
    return this->Encode(Obj.Ptr(), Obj->GetClass(), FieldName, Out);
}


bool DCsvEncoder::EncodeField(const DStruct& Obj, const DClass& Cls, const DString8& FieldName, DString8& Out)
{
    return this->Encode(&Obj, Cls, FieldName, Out);
}
