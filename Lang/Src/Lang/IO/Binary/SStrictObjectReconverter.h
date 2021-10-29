#pragma once

#include "SPrimitiveCopy.h"
#include "SCoreObjectCopy.h"


// SE: same endian
template<bool SE, typename BYTE>
class SStrictObjectReconverter
{
public:

    // OT: HObject or DStruct
    // returns number of bytes read
    template<typename OT>
    static cint BytesToObject(const BYTE* InBytes, OT* O, const DClass& Cls)
    {
        const BYTE* startPosition = InBytes;

        DFieldEnumerator etor = Cls.GetFieldEnumerator();
        while (const DMapEntry<DString8, DField*>* node = etor.Next())
        {
            const DField* f = node->GetValue();
            switch (f->ArchType)
            {
            case EArchType::BOOL:
            {
                const DBoolField<OT>* field = static_cast<const DBoolField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, bool>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::ENUM:
            {
                const DEnumField<OT, uint8>* field = static_cast<const DEnumField<OT, uint8>*>(f);
                InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItemChecked(InBytes, field->GetEnumLength(), field->GetPtr(O));
                break;
            }
            case EArchType::INT:
            {
                const DIntField<OT>* field = static_cast<const DIntField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::INT64:
            {
                const DInt64Field<OT>* field = static_cast<const DInt64Field<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::FLOAT:
            {
                const DFloatField<OT>* field = static_cast<const DFloatField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::DOUBLE:
            {
                const DDoubleField<OT>* field = static_cast<const DDoubleField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, double>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::BYTE:
            {
                const DByteField<OT>* field = static_cast<const DByteField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::SBYTE:
            {
                const DSignedByteField<OT>* field = static_cast<const DSignedByteField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, int8>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::USHORT:
            {
                const DUnsignedShortField<OT>* field = static_cast<const DUnsignedShortField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, uint16>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::SHORT:
            {
                const DShortField<OT>* field = static_cast<const DShortField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, int16>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::UINT:
            {
                const DUnsignedIntField<OT>* field = static_cast<const DUnsignedIntField<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, uint32>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::UINT64:
            {
                const DUnsignedInt64Field<OT>* field = static_cast<const DUnsignedInt64Field<OT>*>(f);
                InBytes += SPrimitiveCopy<SE, BYTE, uint64>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::STRING:
            {
                const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
                InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::STRING8:
            {
                const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
                InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, field->GetPtr(O));
                break;
            }
            case EArchType::FLOAT_STRUCT:
            {
                const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
                cint floatCount = field->GetByteCount() / CINT_SIZEOF(float);
                InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, floatCount, field->GetPtr(O));
                break;
            }
            case EArchType::STRUCT:
            {
                const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
                InBytes += BytesToObject<DStruct>(InBytes, field->GetPtr(O), field->GetStructClass());
                break;
            }
            case EArchType::SHARED_OBJECT:
            {
                break;
            }
            case EArchType::WEAK_OBJECT:
            {
                break;
            }
            case EArchType::ARRAY:
            {
                switch (f->ItemType)
                {
                case EItemType::BOOL:
                {
                    auto* field = static_cast<const DArrayField<OT, bool>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, bool>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::ENUM:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToArrayChecked(InBytes, len, field->GetItemEnumLength(), field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::INT:
                {
                    auto* field = static_cast<const DArrayField<OT, int32>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::INT64:
                {
                    auto* field = static_cast<const DArrayField<OT, int64>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::FLOAT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::DOUBLE:
                {
                    auto* field = static_cast<const DArrayField<OT, double>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, double>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::BYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::SBYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, int8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, int8>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::SHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, int16>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, int16>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::USHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint16>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint16>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::UINT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint32>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint32>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::UINT64:
                {
                    auto* field = static_cast<const DArrayField<OT, uint64>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint64>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::STRING:
                {
                    auto* field = static_cast<const DArrayField<OT, DString>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::STRING8:
                {
                    auto* field = static_cast<const DArrayField<OT, DString8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToArray(InBytes, len, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::FLOAT_STRUCT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    cint totalFloatCount = (field->GetItemByteCount() / CINT_SIZEOF(float)) * len;
                    field->ClearAndReserve(O, cap);
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, totalFloatCount, field->Data(O));
                    field->OverwriteLength(O, len);
                    break;
                }
                case EItemType::STRUCT:
                {
                    auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
                    cint structByteCount = field->GetItemByteCount();
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    if (len > 0)
                        field->Resize(O, len);
                    BYTE* arrayStart = reinterpret_cast<BYTE*>(field->Data(O));
                    cint totalArrayByteCount = structByteCount * len;
                    DClass& structClass = field->GetStructClass();
                    for (cint off = 0; off < totalArrayByteCount; off += structByteCount)
                    {
                        DStruct* struct_ = reinterpret_cast<DStruct*>(arrayStart + off);
                        InBytes += BytesToObject<DStruct>(InBytes, struct_, structClass);
                    }
                    break;
                }
                case EItemType::SHARED_OBJECT:
                {
                    break;
                }
                case EItemType::WEAK_OBJECT:
                {
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
                    auto* field = static_cast<const DSetField<OT, uint8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    for (cint i = 0; i < len; ++i)
                    {
                        uint8 value;
                        InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItemChecked(InBytes, UINT8_MAX, &value);
                        field->Add(O, value);
                    }
                    break;
                }
                case EKeyType::INT:
                {
                    auto* field = static_cast<const DSetField<OT, cint>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    for (cint i = 0; i < len; ++i)
                    {
                        int32 value;
                        InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToItem(InBytes, &value);
                        field->Add(O, value);
                    }
                    break;
                }
                case EKeyType::INT64:
                {
                    auto* field = static_cast<const DSetField<OT, int64>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    for (cint i = 0; i < len; ++i)
                    {
                        int64 value;
                        InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToItem(InBytes, &value);
                        field->Add(O, value);
                    }
                    break;
                }
                case EKeyType::STRING:
                {
                    auto* field = static_cast<const DSetField<OT, DString>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    for (cint i = 0; i < len; ++i)
                    {
                        DString obj;
                        InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToItem(InBytes, &obj);
                        field->Add(O, std::move(obj));
                    }
                    break;
                }
                case EKeyType::STRING8:
                {
                    auto* field = static_cast<const DSetField<OT, DString8>*>(f);
                    // read meta
                    cint len;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                    cint cap;
                    InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                    // read items
                    field->ClearAndReserve(O, cap);
                    for (cint i = 0; i < len; ++i)
                    {
                        DString8 obj;
                        InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, &obj);
                        field->Add(O, std::move(obj));
                    }
                    break;
                }
                case EKeyType::SHARED_OBJECT:
                {
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
                    BytesToMap<OT, uint8, UINT8_ENUM, false>(f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::INT:
                {
                    BytesToMap<OT, int32, int32, true>(f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::INT64:
                {
                    BytesToMap<OT, int64, int64, true>(f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::STRING:
                {
                    BytesToMap<OT, DString, DString, false>(f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::STRING8:
                {
                    BytesToMap<OT, DString8, DString8, false>(f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::SHARED_OBJECT:
                {
                    break;
                }
                }
                break;
            }
            }
        }

        return (cint)(InBytes - startPosition);
    }

private:

    // returns byte array progress
    template<typename OT, typename K, typename K_COPY_TYPE, bool PRIMITIVE_KEY>
    static void BytesToMap(const DField* f, const BYTE*& InBytes, OT* O, const DClass& Cls)
    {
        switch (f->ItemType)
        {
        case EItemType::BOOL:
        {
            const DMapField<OT, K, bool>* field = static_cast<const DMapField<OT, K, bool>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    bool* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, bool>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    bool* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, bool>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::ENUM:
        {
            const DMapField<OT, K, uint8>* field = static_cast<const DMapField<OT, K, uint8>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    uint8* value = field->AddDefault(O, key);
                    InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItemChecked(InBytes, field->GetItemEnumLength(), value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    uint8* value = field->AddDefault(O, std::move(key));
                    InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItemChecked(InBytes, field->GetItemEnumLength(), value);
                }
            }
            break;
        }
        case EItemType::INT:
        {
            const DMapField<OT, K, int32>* field = static_cast<const DMapField<OT, K, int32>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    int32* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    int32* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::INT64:
        {
            const DMapField<OT, K, int64>* field = static_cast<const DMapField<OT, K, int64>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    int64* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    int64* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::FLOAT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    float* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    float* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::DOUBLE:
        {
            const DMapField<OT, K, double>* field = static_cast<const DMapField<OT, K, double>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    double* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, double>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    double* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, double>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::STRING:
        {
            const DMapField<OT, K, DString>* field = static_cast<const DMapField<OT, K, DString>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DString* value = field->AddDefault(O, key);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DString* value = field->AddDefault(O, std::move(key));
                    InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::STRING8:
        {
            const DMapField<OT, K, DString8>* field = static_cast<const DMapField<OT, K, DString8>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DString8* value = field->AddDefault(O, key);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DString8* value = field->AddDefault(O, std::move(key));
                    InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, value);
                }
            }
            break;
        }
        case EItemType::FLOAT_STRUCT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    float* value = field->AddDefault(O, key);
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, floatCount, value);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    float* value = field->AddDefault(O, std::move(key));
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, floatCount, value);
                }
            }
            break;
        }
        case EItemType::STRUCT:
        {
            const DStructMapField<OT, K, DStruct>* field = static_cast<const DStructMapField<OT, K, DStruct>*>(f);
            // read meta
            cint len;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
            cint cap;
            InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
            // read items
            field->ClearAndReserve(O, cap);
            DClass& structClass = field->GetItemStructClass();
            if constexpr (PRIMITIVE_KEY)
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DStruct* value = field->AddDefault(O, key);
                    InBytes += BytesToObject(InBytes, value, structClass);
                }
            }
            else
            {
                for (cint i = 0; i < len; ++i)
                {
                    K key;
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::BytesToItem(InBytes, &key);
                    DStruct* value = field->AddDefault(O, std::move(key));
                    InBytes += BytesToObject(InBytes, value, structClass);
                }
            }
            break;
        }
        case EItemType::SHARED_OBJECT:
        {
            break;
        }
        case EItemType::WEAK_OBJECT:
        {
            break;
        }
        }
    }

};