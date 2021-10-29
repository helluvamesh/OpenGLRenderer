#pragma once

#include "SPrimitiveCopy.h"
#include "SCoreObjectCopy.h"

	
// SE: same endian
template<bool SE, typename BYTE, bool LENIENT>
class SObjectTraveller
{
public:

	// OT: HObject or DStruct
	template<typename OT>
	static cint GetByteCount(const OT* O, const DClass& Cls)
	{
        cint totalByteCount = 0;

        if constexpr (LENIENT)
        {
            // field count
            totalByteCount += CINT_SIZEOF(uint16);
        }

        DFieldEnumerator etor = Cls.GetFieldEnumerator();
        while (const DMapEntry<DString8, DField*>* node = etor.Next())
        {
            const DField* f = node->GetValue();
            switch (f->ArchType)
            {
            case EArchType::BOOL:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(bool);
                break;
            }
            case EArchType::ENUM:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                break;
            }
            case EArchType::INT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(int32);
                break;
            }
            case EArchType::INT64:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(int64);
                break;
            }
            case EArchType::FLOAT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(float);
                break;
            }
            case EArchType::DOUBLE:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(double);
                break;
            }
            case EArchType::BYTE:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(uint8);
                break;
            }
            case EArchType::SBYTE:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(int8);
                break;
            }
            case EArchType::USHORT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(uint16);
                break;
            }
            case EArchType::SHORT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(int16);
                break;
            }
            case EArchType::UINT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(uint32);
                break;
            }
            case EArchType::UINT64:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                totalByteCount += CINT_SIZEOF(uint64);
                break;
            }
            case EArchType::STRING:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString>::GetByteCount(field->GetPtr(O));
                break;
            }
            case EArchType::STRING8:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(field->GetPtr(O));
                break;
            }
            case EArchType::FLOAT_STRUCT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                    totalByteCount += CINT_SIZEOF(uint8);  // floatCount
                }
                const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
                totalByteCount += field->GetByteCount();
                break;
            }
            case EArchType::STRUCT:
            {
                if constexpr (LENIENT)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount();
                }
                const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
                totalByteCount += GetByteCount(field->GetPtr(O), field->GetStructClass());
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
                    DArray<bool>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(bool) * arr->Length();
                    break;
                }
                case EItemType::ENUM:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    DArray<uint8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * arr->Length();
                    break;
                }
                case EItemType::INT:
                {
                    auto* field = static_cast<const DArrayField<OT, int32>*>(f);
                    DArray<int32>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int32) * arr->Length();
                    break;
                }
                case EItemType::INT64:
                {
                    auto* field = static_cast<const DArrayField<OT, int64>*>(f);
                    DArray<int64>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int64) * arr->Length();
                    break;
                }
                case EItemType::FLOAT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    DArray<float>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(float) * arr->Length();
                    break;
                }
                case EItemType::DOUBLE:
                {
                    auto* field = static_cast<const DArrayField<OT, double>*>(f);
                    DArray<double>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(double) * arr->Length();
                    break;
                }
                case EItemType::BYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    DArray<uint8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(uint8) * arr->Length();
                    break;
                }
                case EItemType::SBYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, int8>*>(f);
                    DArray<int8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int8) * arr->Length();
                    break;
                }
                case EItemType::USHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint16>*>(f);
                    DArray<uint16>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(uint16) * arr->Length();
                    break;
                }
                case EItemType::SHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, int16>*>(f);
                    DArray<int16>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int16) * arr->Length();
                    break;
                }
                case EItemType::UINT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint32>*>(f);
                    DArray<uint32>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(uint32) * arr->Length();
                    break;
                }
                case EItemType::UINT64:
                {
                    auto* field = static_cast<const DArrayField<OT, uint64>*>(f);
                    DArray<uint64>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(uint64) * arr->Length();
                    break;
                }
                case EItemType::STRING:
                {
                    auto* field = static_cast<const DArrayField<OT, DString>*>(f);
                    DArray<DString>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    cint len = arr->Length();
                    totalByteCount += CINT_SIZEOF(cint) * 2;
                    for (cint i = 0; i < len; ++i)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString>::GetByteCount(&arr->At(i));
                    }
                    break;
                }
                case EItemType::STRING8:
                {
                    auto* field = static_cast<const DArrayField<OT, DString8>*>(f);
                    DArray<DString8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    cint len = arr->Length();
                    totalByteCount += CINT_SIZEOF(cint) * 2;
                    for (cint i = 0; i < len; ++i)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&arr->At(i));
                    }
                    break;
                }
                case EItemType::FLOAT_STRUCT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    DArray<float>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                        totalByteCount += CINT_SIZEOF(uint8);  // floatCount
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + field->GetItemByteCount() * arr->Length();
                    break;
                }
                case EItemType::STRUCT:
                {
                    auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
                    DArray<DStruct>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    BYTE* arrayStart = reinterpret_cast<BYTE*>(arr->GetDataUnsafe());
                    cint structByteCount = field->GetItemByteCount();
                    cint len = arr->Length();
                    cint totalArrayByteCount = structByteCount * len;
                    DClass& structClass = field->GetStructClass();
                    totalByteCount += CINT_SIZEOF(cint) * 2;
                    for (cint off = 0; off < totalArrayByteCount; off += structByteCount)
                    {
                        DStruct* struct_ = reinterpret_cast<DStruct*>(arrayStart + off);
                        totalByteCount += GetByteCount(struct_, structClass);
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
                    DSet<uint8>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * set->Length();
                    break;
                }
                case EKeyType::INT:
                {
                    auto* field = static_cast<const DSetField<OT, int32>*>(f);
                    DSet<int32>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int32) * set->Length();
                    break;
                }
                case EKeyType::INT64:
                {
                    auto* field = static_cast<const DSetField<OT, int64>*>(f);
                    DSet<int64>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2 + CINT_SIZEOF(int64) * set->Length();
                    break;
                }
                case EKeyType::STRING:
                {
                    auto* field = static_cast<const DSetField<OT, DString>*>(f);
                    DSet<DString>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2;
                    for (const DString& item : *set)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString>::GetByteCount(&item);
                    }
                    break;
                }
                case EKeyType::STRING8:
                {
                    auto* field = static_cast<const DSetField<OT, DString8>*>(f);
                    DSet<DString8>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                        totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 2;
                    }
                    totalByteCount += CINT_SIZEOF(cint) * 2;
                    for (const DString8& item : *set)
                    {
                        totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&item);
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
                    GetByteCountOfMapEntry<OT, uint8, UINT8_ENUM, false>(node, O, Cls, totalByteCount);
                    break;
                }
                case EKeyType::INT:
                {
                    GetByteCountOfMapEntry<OT, int32, int32, true>(node, O, Cls, totalByteCount);
                    break;
                }
                case EKeyType::INT64:
                {
                    GetByteCountOfMapEntry<OT, int64, int64, true>(node, O, Cls, totalByteCount);
                    break;
                }
                case EKeyType::STRING:
                {
                    GetByteCountOfMapEntry<OT, DString, DString, false>(node, O, Cls, totalByteCount);
                    break;
                }
                case EKeyType::STRING8:
                {
                    GetByteCountOfMapEntry<OT, DString8, DString8, false>(node, O, Cls, totalByteCount);
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
        return totalByteCount;
    }

private:

    template<typename OT, typename K, typename K_COPY_TYPE, bool PRIMITIVE_KEY>
    static void GetByteCountOfMapEntry(const DMapEntry<DString8, DField*>* node, const OT* O, const DClass& Cls, int32& totalByteCount)
    {
        const DField* f = node->GetValue();
        switch (f->ItemType)
        {
        case EItemType::BOOL:
        {
            const DMapField<OT, K, bool>* field = static_cast<const DMapField<OT, K, bool>*>(f);
            DMap<K, bool>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(bool) * map->Length();
            break;
        }
        case EItemType::ENUM:
        {
            const DMapField<OT, K, uint8>* field = static_cast<const DMapField<OT, K, uint8>*>(f);
            DMap<K, uint8>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(uint8) * map->Length();
            break;
        }
        case EItemType::INT:
        {
            const DMapField<OT, K, int32>* field = static_cast<const DMapField<OT, K, int32>*>(f);
            DMap<K, int32>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(cint) * map->Length();
            break;
        }
        case EItemType::INT64:
        {
            const DMapField<OT, K, int64>* field = static_cast<const DMapField<OT, K, int64>*>(f);
            DMap<K, int64>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(int64) * map->Length();
            break;
        }
        case EItemType::FLOAT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            DMap<K, float>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(float) * map->Length();
            break;
        }
        case EItemType::DOUBLE:
        {
            const DMapField<OT, K, double>* field = static_cast<const DMapField<OT, K, double>*>(f);
            DMap<K, double>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += CINT_SIZEOF(double) * map->Length();
            break;
        }
        case EItemType::STRING:
        {
            const DMapField<OT, K, DString>* field = static_cast<const DMapField<OT, K, DString>*>(f);
            DMap<K, DString>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString>::GetByteCount(&entry.GetValue());
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString>::GetByteCount(&entry.GetValue());
                }
            }
            break;
        }
        case EItemType::STRING8:
        {
            const DMapField<OT, K, DString8>* field = static_cast<const DMapField<OT, K, DString8>*>(f);
            DMap<K, DString8>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&entry.GetValue());
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                    totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&entry.GetValue());
                }
            }
            break;
        }
        case EItemType::FLOAT_STRUCT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            DMap<K, float>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
                totalByteCount += CINT_SIZEOF(uint8);  // floatCount
            }
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                }
            }
            totalByteCount += field->GetItemByteCount() * map->Length();
            break;
        }
        case EItemType::STRUCT:
        {
            const DStructMapField<OT, K, DStruct>* field = static_cast<const DStructMapField<OT, K, DStruct>*>(f);
            DMap<K, DStruct>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                totalByteCount += SCoreObjectCopy<SE, BYTE, DString8>::GetByteCount(&node->GetKey());
                totalByteCount += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::GetByteCount() * 3;
            }
            cint structByteCount = field->GetItemByteCount();
            DClass& structClass = field->GetItemStructClass();
            totalByteCount += CINT_SIZEOF(cint) * 2;
            if constexpr (PRIMITIVE_KEY)
            {
                totalByteCount += CINT_SIZEOF(K_COPY_TYPE) * map->Length();
                for (auto& entry : *map)
                {
                    totalByteCount += GetByteCount(&entry.GetValue(), structClass);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    totalByteCount += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::GetByteCount(&entry.GetKey());
                    totalByteCount += GetByteCount(&entry.GetValue(), structClass);
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
