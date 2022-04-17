#include "pch.h"
#include "DJsonEncoder.h"
#include "SJsonUtils.h"
#include "SJsonConvert.h"


FORCEINLINE void DJsonEncoder::DoIndent()
{
    cint len = this->IndentLevel * this->IndentStep;
    for (cint i = 0; i < len; ++i)
    {
        this->JStr->Append(' ');
    }
}

FORCEINLINE void DJsonEncoder::AppendFieldName(bool NotFirst, const DString8& Key)
{
    if (NotFirst) 
    {
        this->JStr->Append(',');
    }

    if (this->Indent)
    {
        this->JStr->Append('\n');
        this->DoIndent();
    }

    SJsonConvert<DString8>::AppendItemToJsonString(Key, *this->JStr);
    this->JStr->Append(':');

    if (Indent)
    {
        this->JStr->Append(' ');
    }
}

FORCEINLINE void DJsonEncoder::StartObject()
{
    this->JStr->Append('{');
    if (this->Indent)
    {
        this->IndentLevel++;
    }
}

FORCEINLINE void DJsonEncoder::CloseObject(cint Length)
{
    if (this->Indent)
    {
        this->IndentLevel--;
        if (Length > 0)
        {
            this->JStr->Append('\n');
            this->DoIndent();
        }
    }
    this->JStr->Append('}');
}

FORCEINLINE void DJsonEncoder::StartArray()
{
    this->JStr->Append('[');
    if (this->Indent)
    {
        this->IndentLevel++;
    }
}

FORCEINLINE void DJsonEncoder::CloseArray(cint Length)
{
    if (this->Indent)
    {
        this->IndentLevel--;
        if (Length > 0)
        {
            this->JStr->Append('\n');
            this->DoIndent();
        }
    }
    this->JStr->Append(']');
}

FORCEINLINE void DJsonEncoder::NewItem(bool NotFirst)
{
    if (NotFirst)
    {
        this->JStr->Append(',');
        if (this->Indent)
        {
            this->JStr->Append('\n');
            this->DoIndent();
        }
    }
    else if (this->Indent)
    {
        this->JStr->Append('\n');
        this->DoIndent();
    }
}

FORCEINLINE void DJsonEncoder::NewMapKey(bool NotFirst)
{
    if (NotFirst)
    {
        this->JStr->Append(',');
    }
    if (this->Indent)
    {
        this->JStr->Append('\n');
        this->DoIndent();
    }
}

FORCEINLINE void DJsonEncoder::NewMapValue()
{
    if (this->Indent)
    {
        this->JStr->Append(": ");
    }
    else
    {
        this->JStr->Append(':');
    }
}


template<typename OT>
void DJsonEncoder::ObjectToJsonString(const OT* Obj, const DClass& Cls)
{
    StartObject();

    bool firstDone = false;

    DFieldEnumerator etor = Cls.GetFieldEnumerator();
    while (const DMapEntry<DString8, DField*>* node = etor.Next())
    {
        DString8 fieldName = node->GetKey();
        const DField* f = node->GetValue();

        FieldToJsonString(Obj, Cls, fieldName, f, true, firstDone);
        firstDone = true;
    }
    CloseObject(1);
};


template<typename OT>
void DJsonEncoder::FieldToJsonString(const OT* Obj, const DClass& Cls, const DString8& fieldName, const DField* f, bool AppendKey, bool NotFirst)
{
    DString8& jstr = *this->JStr;

    switch (f->ArchType)
    {
    case EArchType::BOOL:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DBoolField<OT>* field = static_cast<const DBoolField<OT>*>(f);
        SJsonConvert<bool>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::ENUM:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DEnumField<OT, uint8>* field = static_cast<const DEnumField<OT, uint8>*>(f);
        DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
        SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(enumClass, *field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::INT:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DIntField<OT>* field = static_cast<const DIntField<OT>*>(f);
        SJsonConvert<int32>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::INT64:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DInt64Field<OT>* field = static_cast<const DInt64Field<OT>*>(f);
        SJsonConvert<int64>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::FLOAT:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DFloatField<OT>* field = static_cast<const DFloatField<OT>*>(f);
        SJsonConvert<float>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::DOUBLE:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DDoubleField<OT>* field = static_cast<const DDoubleField<OT>*>(f);
        SJsonConvert<double>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::STRING:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
        DString8 u8str = DStrings::UTF16ToUTF8(*field->GetPtr(Obj));
        SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
        break;
    }
    case EArchType::STRING8:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
        SJsonConvert<DString8>::AppendItemToJsonString(*field->GetPtr(Obj), jstr);
        break;
    }
    case EArchType::FLOAT_STRUCT:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
        cint floatCount = field->GetByteCount() / CINT_SIZEOF(float);
        StartArray();
        const float* floats = field->GetPtr(Obj);
        for (cint float_i = 0; float_i < floatCount; ++float_i)
        {
            NewItem(float_i > 0);
            SJsonConvert<float>::AppendItemToJsonString(floats[float_i], jstr);
        }
        CloseArray(floatCount);
        break;
    }
    case EArchType::STRUCT:
    {
        if (AppendKey) AppendFieldName(NotFirst, fieldName);
        const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
        ObjectToJsonString<DStruct>(field->GetPtr(Obj), field->GetStructClass());
        break;
    }
    case EArchType::ARRAY:
    {
        switch (f->ItemType)
        {
        case EItemType::BOOL:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, bool>*>(f);
            DArray<bool>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                NewItem(i > 0);
                SJsonConvert<bool>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::ENUM:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
            DArray<uint8>* arr = field->GetPtr(Obj);
            DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                NewItem(i > 0);
                SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(enumClass, arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::INT:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, int32>*>(f);
            DArray<int32>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                SJsonConvert<int32>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::INT64:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, int64>*>(f);
            DArray<int64>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                SJsonConvert<int64>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::FLOAT:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, float>*>(f);
            DArray<float>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                SJsonConvert<float>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::DOUBLE:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, double>*>(f);
            DArray<double>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                SJsonConvert<double>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::STRING:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, DString>*>(f);
            DArray<DString>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                NewItem(i > 0);
                DString8 u8str = DStrings::UTF16ToUTF8(arr->At(i));
                SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::STRING8:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, DString8>*>(f);
            DArray<DString8>* arr = field->GetPtr(Obj);
            StartArray();
            cint len = arr->Length();
            for (cint i = 0; i < len; ++i)
            {
                NewItem(i > 0);
                SJsonConvert<DString8>::AppendItemToJsonString(arr->At(i), jstr);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::FLOAT_STRUCT:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DArrayField<OT, float>*>(f);
            DArray<float>* arr = field->GetPtr(Obj);
            cint len = arr->Length();
            const float* arrayStart = arr->Data();
            cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
            cint totalArrayFloatCount = floatCount * len;
            StartArray();
            for (cint off = 0; off < totalArrayFloatCount; off += floatCount)
            {
                NewItem(off > 0);
                StartArray();
                const float* itemFloats = arrayStart + off;
                for (cint float_i = 0; float_i < floatCount; ++float_i)
                {
                    NewItem(float_i > 0);
                    SJsonConvert<float>::AppendItemToJsonString(itemFloats[float_i], jstr);
                }
                CloseArray(floatCount);
            }
            CloseArray(arr->Length());
            break;
        }
        case EItemType::STRUCT:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
            DArray<DStruct>* arr = field->GetPtr(Obj);
            cint len = arr->Length();
            const byte* arrayStart = reinterpret_cast<const byte*>(arr->Data());
            DClass& structClass = field->GetStructClass();
            cint structByteCount = field->GetItemByteCount();
            cint totalArrayByteCount = structByteCount * len;
            cint last_off = totalArrayByteCount - structByteCount;
            StartArray();
            for (cint off = 0; off < totalArrayByteCount; off += structByteCount)
            {
                NewItem(off > 0);
                const DStruct* struct_ = reinterpret_cast<const DStruct*>(arrayStart + off);
                ObjectToJsonString<DStruct>(struct_, structClass);
            }
            CloseArray(arr->Length());
            break;
        }
        }

        break;
    }
    case EArchType::SET:
    {
        switch (f->KeyType)
        {
        case EKeyType::ENUM:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DSetField<OT, uint8>*>(f);
            DSet<uint8>* set = field->GetPtr(Obj);
            DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
            StartArray();
            cint i = 0;
            for (uint8 item : *set)
            {
                NewItem(i++ > 0);
                SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(enumClass, item, jstr);
            }
            CloseArray(set->Length());
            break;
        }
        case EKeyType::INT:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DSetField<OT, int32>*>(f);
            DSet<int32>* set = field->GetPtr(Obj);
            StartArray();
            cint i = 0;
            for (int32 item : *set)
            {
                NewItem(i++ > 0);
                SJsonConvert<int32>::AppendItemToJsonString(item, jstr);
            }
            CloseArray(set->Length());
            break;
        }
        case EKeyType::INT64:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DSetField<OT, int64>*>(f);
            DSet<int64>* set = field->GetPtr(Obj);
            StartArray();
            cint i = 0;
            for (int64 item : *set)
            {
                NewItem(i++ > 0);
                SJsonConvert<int64>::AppendItemToJsonString(item, jstr);
            }
            CloseArray(set->Length());
            break;
        }
        case EKeyType::STRING:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DSetField<OT, DString>*>(f);
            DSet<DString>* set = field->GetPtr(Obj);
            StartArray();
            cint i = 0;
            for (const DString& item : *set)
            {
                NewItem(i++ > 0);
                DString8 u8str =  DStrings::UTF16ToUTF8(item);
                SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
            }
            CloseArray(set->Length());
            break;
        }
        case EKeyType::STRING8:
        {
            if (AppendKey) AppendFieldName(NotFirst, fieldName);
            auto* field = static_cast<const DSetField<OT, DString8>*>(f);
            DSet<DString8>* set = field->GetPtr(Obj);
            StartArray();
            cint i = 0;
            for (const DString8& item : *set)
            {
                NewItem(i++ > 0);
                SJsonConvert<DString8>::AppendItemToJsonString(item, jstr);
            }
            CloseArray(set->Length());
            break;
        }
        }

        break;
    }
    case EArchType::MAP:
    {
        switch (f->KeyType)
        {
        case EKeyType::ENUM:
        {
            switch (f->ItemType)
            {
            case EItemType::BOOL:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, bool>* field = static_cast<const DMapField<OT, uint8, bool>*>(f);
                DMap<uint8, bool>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<bool>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::ENUM:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, uint8>* field = static_cast<const DMapField<OT, uint8, uint8>*>(f);
                DMap<uint8, uint8>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                DEnumClass* itemEnumClass = DEnumClass::FindEnumClass(field->GetItemTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(itemEnumClass, entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, int32>* field = static_cast<const DMapField<OT, uint8, int32>*>(f);
                DMap<uint8, int32>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<int32>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT64:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, int64>* field = static_cast<const DMapField<OT, uint8, int64>*>(f);
                DMap<uint8, int64>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<int64>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, float>* field = static_cast<const DMapField<OT, uint8, float>*>(f);
                DMap<uint8, float>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<float>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::DOUBLE:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, double>* field = static_cast<const DMapField<OT, uint8, double>*>(f);
                DMap<uint8, double>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<double>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, DString>* field = static_cast<const DMapField<OT, uint8, DString>*>(f);
                DMap<uint8, DString>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    DString8 u8value =  DStrings::UTF16ToUTF8(entry.GetValue());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8value, jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING8:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, DString8>* field = static_cast<const DMapField<OT, uint8, DString8>*>(f);
                DMap<uint8, DString8>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT_STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, uint8, float>* field = static_cast<const DMapField<OT, uint8, float>*>(f);
                DMap<uint8, float>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    const float* floats = reinterpret_cast<const float*>(&entry.GetValue());
                    StartArray();
                    for (cint float_i = 0; float_i < floatCount; ++float_i)
                    {
                        NewItem(float_i > 0);
                        SJsonConvert<float>::AppendItemToJsonString(floats[float_i], jstr);
                    }
                    CloseArray(floatCount);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DStructMapField<OT, uint8, DStruct>* field = static_cast<const DStructMapField<OT, uint8, DStruct>*>(f);
                DMap<uint8, DStruct>* map = field->GetPtr(Obj);
                DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
                DClass& fieldClass = field->GetItemStructClass();
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(keyEnumClass, entry.GetKey(), jstr);
                    NewMapValue();
                    ObjectToJsonString<DStruct>(&entry.GetValue(), fieldClass);
                }
                CloseObject(map->Length());
                break;
            }
            }
            break;
        }
        case EKeyType::STRING:
        {
            switch (f->ItemType)
            {
            case EItemType::BOOL:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, bool>* field = static_cast<const DMapField<OT, DString, bool>*>(f);
                DMap<DString, bool>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str =  DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<bool>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::ENUM:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, uint8>* field = static_cast<const DMapField<OT, DString, uint8>*>(f);
                DMap<DString, uint8>* map = field->GetPtr(Obj);
                DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetItemTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str =  DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(enumClass, entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, int32>* field = static_cast<const DMapField<OT, DString, int32>*>(f);
                DMap<DString, int32>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<int32>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT64:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, int64>* field = static_cast<const DMapField<OT, DString, int64>*>(f);
                DMap<DString, int64>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<int64>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, float>* field = static_cast<const DMapField<OT, DString, float>*>(f);
                DMap<DString, float>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<float>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::DOUBLE:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, double>* field = static_cast<const DMapField<OT, DString, double>*>(f);
                DMap<DString, double>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<double>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, DString>* field = static_cast<const DMapField<OT, DString, DString>*>(f);
                DMap<DString, DString>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    DString8 u8value = DStrings::UTF16ToUTF8(entry.GetValue());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8value, jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING8:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, DString8>* field = static_cast<const DMapField<OT, DString, DString8>*>(f);
                DMap<DString, DString8>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT_STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString, float>* field = static_cast<const DMapField<OT, DString, float>*>(f);
                DMap<DString, float>* map = field->GetPtr(Obj);
                cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    const float* floats = reinterpret_cast<const float*>(&entry.GetValue());
                    StartArray();
                    for (cint float_i = 0; float_i < floatCount; ++float_i)
                    {
                        NewItem(float_i > 0);
                        SJsonConvert<float>::AppendItemToJsonString(floats[float_i], jstr);
                    }
                    CloseArray(floatCount);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DStructMapField<OT, DString, DStruct>* field = static_cast<const DStructMapField<OT, DString, DStruct>*>(f);
                DMap<DString, DStruct>* map = field->GetPtr(Obj);
                DClass& fieldClass = field->GetItemStructClass();
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    DString8 u8str = DStrings::UTF16ToUTF8(entry.GetKey());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8str, jstr);
                    NewMapValue();
                    ObjectToJsonString<DStruct>(&entry.GetValue(), fieldClass);
                }
                CloseObject(map->Length());
                break;
            }
            }
            break;
        }
        case EKeyType::STRING8:
        {
            switch (f->ItemType)
            {
            case EItemType::BOOL:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, bool>* field = static_cast<const DMapField<OT, DString8, bool>*>(f);
                DMap<DString8, bool>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<bool>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::ENUM:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, uint8>* field = static_cast<const DMapField<OT, DString8, uint8>*>(f);
                DMap<DString8, uint8>* map = field->GetPtr(Obj);
                DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetItemTypeName());
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<UINT8_ENUM>::AppendItemToJsonString(enumClass, entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, int32>* field = static_cast<const DMapField<OT, DString8, int32>*>(f);
                DMap<DString8, int32>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<int32>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::INT64:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, int64>* field = static_cast<const DMapField<OT, DString8, int64>*>(f);
                DMap<DString8, int64>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<int64>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, float>* field = static_cast<const DMapField<OT, DString8, float>*>(f);
                DMap<DString8, float>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<float>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::DOUBLE:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, double>* field = static_cast<const DMapField<OT, DString8, double>*>(f);
                DMap<DString8, double>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<double>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, DString>* field = static_cast<const DMapField<OT, DString8, DString>*>(f);
                DMap<DString8, DString>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    DString8 u8value = DStrings::UTF16ToUTF8(entry.GetValue());
                    SJsonConvert<DString8>::AppendItemToJsonString(u8value, jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRING8:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, DString8>* field = static_cast<const DMapField<OT, DString8, DString8>*>(f);
                DMap<DString8, DString8>* map = field->GetPtr(Obj);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetValue(), jstr);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::FLOAT_STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DMapField<OT, DString8, float>* field = static_cast<const DMapField<OT, DString8, float>*>(f);
                DMap<DString8, float>* map = field->GetPtr(Obj);
                cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    const float* floats = reinterpret_cast<const float*>(&entry.GetValue());
                    StartArray();
                    for (cint float_i = 0; float_i < floatCount; ++float_i)
                    {
                        NewItem(float_i > 0);
                        SJsonConvert<float>::AppendItemToJsonString(floats[float_i], jstr);
                    }
                    CloseArray(floatCount);
                }
                CloseObject(map->Length());
                break;
            }
            case EItemType::STRUCT:
            {
                if (AppendKey) AppendFieldName(NotFirst, fieldName);
                StartObject();
                const DStructMapField<OT, DString8, DStruct>* field = static_cast<const DStructMapField<OT, DString8, DStruct>*>(f);
                DMap<DString8, DStruct>* map = field->GetPtr(Obj);
                DClass& fieldClass = field->GetItemStructClass();
                cint i = 0;
                for (auto& entry : *map)
                {
                    NewMapKey(i++ > 0);
                    SJsonConvert<DString8>::AppendItemToJsonString(entry.GetKey(), jstr);
                    NewMapValue();
                    ObjectToJsonString<DStruct>(&entry.GetValue(), fieldClass);
                }
                CloseObject(map->Length());
                break;
            }
            }
            break;
        }
        }
    }
    }
}


void DJsonEncoder::ResetState()
{
    this->JStr = nullptr;
    this->IndentLevel = 0;
}


void DJsonEncoder::EncodeObject(shared<HObject> Obj, DString8& OutStr)
{
    this->ResetState();
    this->JStr = &OutStr;
    this->ObjectToJsonString(Obj.Ptr(), Obj->GetClass());
    this->ResetState();
}


void DJsonEncoder::EncodeStruct(const DStruct& Obj, const DClass& Cls, DString8& OutStr)
{
    this->ResetState();
    this->JStr = &OutStr;
    this->ObjectToJsonString(&Obj, Cls);
    this->ResetState();
}


void DJsonEncoder::EncodeObjectField(shared<HObject> Obj, const DString8& FieldName, DString8& OutStr)
{
    this->ResetState();
    this->JStr = &OutStr;
    DClass& c = Obj->GetClass();
    const DField* f = c.FindField(FieldName);
    if (f)
    {
        this->FieldToJsonString(Obj.Ptr(), Obj->GetClass(), FieldName, f, false, false);
    }
    this->ResetState();
}


void DJsonEncoder::EncodeStructField(const DStruct& Obj, const DClass& Cls, const DString8& FieldName, DString8& OutStr)
{
    this->ResetState();
    this->JStr = &OutStr;
    const DField* f = Cls.FindField(FieldName);
    if (f)
    {
        this->FieldToJsonString(&Obj, Cls, FieldName, f, false, false);
    }
    this->ResetState();
}


void DJsonEncoder::SetIndentation(cint Step)
{
    if (Step > 0)
    {
        this->Indent = true;
        this->IndentStep = Step;
    }
    else
    {
        this->Indent = false;
    }
    this->ResetState();
}
