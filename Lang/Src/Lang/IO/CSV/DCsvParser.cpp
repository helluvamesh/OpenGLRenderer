#include "pch.h"
#include "DCsvParser.h"
#include "SCsvUtils.h"
#include "DCsvException.h"


DCsvParser::DCsvParser()
{
}


DCsvParser::DCsvParser(const DCsvSyntax& InSyntax)
{
    this->Syntax = InSyntax;
}


cint DCsvParser::ParseNextField(const DString8& Str, cint Start, DString8& Out)
{
    bool quoteIsOpen = false;

    cint progress = Start;

    cint last_i = Str.Length() - 1;
    cint len = Str.Length();
    for (cint i = Start; i <= last_i; i++)
    {
        char8 c = Str[i];

        if (c == this->Syntax.EscapeChar) 
        {
            // escapeChar
            bool escaped = false;

            if (i + 1 < len)
            {
                // next char is escapeChar
                if (Str[i + 1] == this->Syntax.EscapeChar) escaped = true;
            }

            if (escaped)
            {
                Out.Append(this->Syntax.EscapeChar);
                i++;
            }

            // close quoted field
            else if (quoteIsOpen)
            {
                quoteIsOpen = false;
                // end of str
                if (i == last_i)
                {
                    return i + 1;
                }
            }

            // open quoted field
            else 
            {
                quoteIsOpen = true;
            }
        }

        // close field
        else if (quoteIsOpen == false && c == this->Syntax.Delimiter)
        {
            return i + 1;
        }

        else if (quoteIsOpen == false && c == '\n')
        {
            return i + 1;
        }

        else if (quoteIsOpen == false && 
            c == '\r' && len > i + 1 && Str[i + 1] == '\n')
        {
            return i + 2;
        }

        // end of str
        else if (i == last_i)
        {
            Out.Append(c);
            return i + 1;
        }

        // add regular character to field
        else 
        {
            Out.Append(c);
        }
    }

    return -1;
}


cint DCsvParser::ParseBoolean(const DString8& FieldStr, bool& Value)
{
    DString8 str = FieldStr.ToLower_Ascii();

    cint i = SCsvUtil::FindStringThruWhitespace(str, 0, this->Syntax.FalseStr);
    if (i >= 0) { Value = false; return i; }
    
    i = SCsvUtil::FindStringThruWhitespace(str, 0, this->Syntax.TrueStr);
    if (i >= 0) { Value = true; return i; }

    i = SCsvUtil::FindCharThruWhitespace(str, 0, '0');
    if (i >= 0) { Value = false; return i; }

    i = SCsvUtil::FindCharThruWhitespace(str, 0, '1');
    if (i >= 0) { Value = true; return i; }

    Value = false;
    return -1;
}


cint DCsvParser::ParseHeader(const DString8& Str, cint Start, DArray<DString8>& OutHeader)
{
    cint firstLinebreak = Str.IndexOf("\n", Start);
    if (firstLinebreak == -1) return -1;

    DString8 firstLine = Str.SubstrRange(Start, firstLinebreak);
    cint i = 0;
    cint progress = 0;

    while (true)
    {
        DString8 fieldStr;
        i = this->ParseNextField(firstLine, i, fieldStr);
        if (i >= 0)
        {
            OutHeader.Add(std::move(fieldStr));
            progress = i;
        }
        else 
        {
            break;
        }
    }

    return Start + firstLinebreak + 1;
}


cint DCsvParser::ParseRecord(DStruct* Obj, const DClass& Cls, DArray<DString8> Header, const DString8& CSVString, cint Start)
{
    cint i = Start;
    cint progress = Start;
    cint len = CSVString.Length();
    cint fieldCount = Header.Length();

    for (cint field_i = 0; field_i < fieldCount; ++field_i)
    {
        DString8 fieldStr;
        i = this->ParseNextField(CSVString, progress, fieldStr);
        if (i >= 0) progress = i; else return -1;  // FINISH

        DString8 fieldName = Header[field_i];
        const DField* f = Cls.FindField(fieldName);
        if (f)
        {
            switch (f->ArchType)
            {
            case EArchType::BOOL:
            {
                const DBoolField<DStruct>* field = static_cast<const DBoolField<DStruct>*>(f);
                bool value;
                i = this->ParseBoolean(fieldStr, value);
                if (i >= 0) progress = i; else throw DCsvException("Expected boolean", CSVString, progress);
                *field->GetPtr(Obj) = value;
                break;
            }
            case EArchType::ENUM:
            {
                const DEnumField<DStruct, uint8>* field = static_cast<const DEnumField<DStruct, uint8>*>(f);
                DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
                if (enumClass)
                {
                    uint8 value = enumClass->GetItemByName(fieldStr);
                    *field->GetPtr(Obj) = value >= 0? value : 0;
                }
                else
                {
                    *field->GetPtr(Obj) = 0;
                }
                break;
            }
            case EArchType::INT:
            {
                const DIntField<DStruct>* field = static_cast<const DIntField<DStruct>*>(f);
                int32 value = DString8::ParseInt32(fieldStr);
                i = SCsvUtil::FindEndOfNumberString(fieldStr, progress);
                if (i >= 0) progress = i; else throw DCsvException("Expected number", CSVString, progress);
                *field->GetPtr(Obj) = value;
                break;
            }
            case EArchType::INT64:
            {
                const DInt64Field<DStruct>* field = static_cast<const DInt64Field<DStruct>*>(f);
                int64 value = DString8::ParseInt64(fieldStr);
                i = SCsvUtil::FindEndOfNumberString(fieldStr, progress);
                if (i >= 0) progress = i; else throw DCsvException("Expected number", CSVString, progress);
                *field->GetPtr(Obj) = value;
                break;
            }
            case EArchType::FLOAT:
            {
                const DFloatField<DStruct>* field = static_cast<const DFloatField<DStruct>*>(f);
                float value = DString8::ParseFloat(fieldStr);
                i = SCsvUtil::FindEndOfNumberString(fieldStr, progress);
                if (i >= 0) progress = i; else throw DCsvException("Expected number", CSVString, progress);
                *field->GetPtr(Obj) = value;
                break;
            }
            case EArchType::DOUBLE:
            {
                const DDoubleField<DStruct>* field = static_cast<const DDoubleField<DStruct>*>(f);
                double value = DString8::ParseDouble(fieldStr);
                i = SCsvUtil::FindEndOfNumberString(fieldStr, progress);
                if (i >= 0) progress = i; else throw DCsvException("Expected number", CSVString, progress);
                *field->GetPtr(Obj) = value;
                break;
            }
            case EArchType::STRING:
            {
                const DStringField<DStruct>* field = static_cast<const DStringField<DStruct>*>(f);
                *field->GetPtr(Obj) = DStrings::UTF8ToUTF16(fieldStr);
                break;
            }
            case EArchType::STRING8:
            {
                const DString8Field<DStruct>* field = static_cast<const DString8Field<DStruct>*>(f);
                *field->GetPtr(Obj) = fieldStr;
                break;
            }
            case EArchType::FLOAT_STRUCT:
            case EArchType::STRUCT:
            case EArchType::ARRAY:
            case EArchType::SET:
            case EArchType::MAP:
            {
                this->JSONParser.ParseStructField(*Obj, Cls, fieldName, fieldStr);
                break;
            }
            }
        }
    }

    return progress;
}


template<typename OT>
bool DCsvParser::Parse(OT* Obj, const DClass& Cls, const DString8 FieldName, const DString8& CSVString, cint Start)
{
    const DField* f = Cls.FindField(FieldName);
    if (!f) 
    {
        SLog::Error("CSVException: Field not found: " + FieldName);
        return false;
    }

    try
    {
        if (f->ArchType == EArchType::MAP)
        {
            if (f->KeyType == EKeyType::STRING && f->ItemType == EItemType::STRUCT)
            {
                const DStructMapField<OT, DString, DStruct>* field = static_cast<const DStructMapField<OT, DString, DStruct>*>(f);
                const DClass& structClass = field->GetItemStructClass();

                cint i;
                cint progress = Start;

                DArray<DString8> header;
                i = this->ParseHeader(CSVString, Start, header);
                if (i >= 0) progress = i; else throw DCsvException("Failed to parse header", CSVString, progress);
                if (header.Length() <= 1) throw DCsvException("Two few fields");

                // remove key from header
                header.Remove(0);

                while (true)
                { 
                    DString8 key;
                    i = this->ParseNextField(CSVString, progress, key);
                    if (i >= 0) progress = i; else break;

                    DStruct* struct_ = field->AddDefault(Obj, DStrings::UTF8ToUTF16(key));
                    i = this->ParseRecord(struct_, structClass, header, CSVString, progress);
                    if (i >= 0) progress = i; else break;
                }

                return true;
            }
        }
        else if (f->ArchType == EArchType::ARRAY)
        {
            if (f->ItemType == EItemType::STRUCT)
            {
                auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
                const DClass& structClass = field->GetStructClass();

                cint i;
                cint progress = Start;

                DArray<DString8> header;
                i = this->ParseHeader(CSVString, Start, header);
                if (i >= 0) progress = i; else throw DCsvException("Failed to parse header", CSVString, progress);
                if (header.Length() <= 1) throw DCsvException("Two few fields");

                while (true)
                {
                    DStruct* struct_ = field->AddDefault(Obj);
                    i = this->ParseRecord(struct_, structClass, header, CSVString, progress);
                    if (i >= 0)
                    {
                        progress = i; 
                    }
                    else 
                    {
                        field->Resize(Obj, field->Length(Obj) - 1);  // remove last item
                        break;
                    }
                }

                return true;
            }
        }
    }
    catch (DCsvException e)
    {
        SLog::Error(e.what());
        SLog::Error(e.ErrorPartString);
    }

    return false;
}


bool DCsvParser::ParseField(shared<HObject> Obj, const DString8& FieldName, const DString8& CSVString, cint Start)
{
    return this->Parse(Obj.Ptr(), Obj->GetClass(), FieldName, CSVString, Start);
}


bool DCsvParser::ParseField(DStruct& Obj, const DClass& Cls, const DString8& FieldName, const DString8& CSVString, cint Start)
{
    return this->Parse(&Obj, Cls, FieldName, CSVString, Start);
}
