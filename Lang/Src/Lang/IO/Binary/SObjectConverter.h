#pragma once

#include "SPrimitiveCopy.h"
#include "SCoreObjectCopy.h"


// SE: same endian
template<bool SE, typename BYTE, bool LENIENT>
class SObjectConverter
{
public:

    // OT: Object or Struct
    // returns number of bytes written
    template<typename OT>
    static cint ObjectToBytes(const OT* O, const DClass& Cls, BYTE* OutBytes, bool LengthAsCapacity)
    {
        BYTE* startPosition = OutBytes;

        if constexpr (LENIENT)
        {
            // write field count
            uint16 fieldCount = (uint16)Cls.GetFieldCount();
            OutBytes += SPrimitiveCopy<SE, BYTE, uint16>::ItemToBytes(&fieldCount, OutBytes);
        }

        DFieldEnumerator etor = Cls.GetFieldEnumerator();
        while (const DMapEntry<DString8, DField*>* node = etor.Next())
        {
            const DField* f = node->GetValue();
            switch (f->ArchType)
            {
            case EArchType::BOOL:
            {
                const DBoolField<OT>* field = static_cast<const DBoolField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, bool>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::ENUM:
            {
                const DEnumField<OT, uint8>* field = static_cast<const DEnumField<OT, uint8>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                uint8* enum_ = field->GetPtr(O);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes(enum_, OutBytes);
                break;
            }
            case EArchType::INT:
            {
                const DIntField<OT>* field = static_cast<const DIntField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, int32>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::INT64:
            {
                const DInt64Field<OT>* field = static_cast<const DInt64Field<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, int64>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::FLOAT:
            {
                const DFloatField<OT>* field = static_cast<const DFloatField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, float>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::DOUBLE:
            {
                const DDoubleField<OT>* field = static_cast<const DDoubleField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, double>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::BYTE:
            {
                const DByteField<OT>* field = static_cast<const DByteField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, uint8>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::SBYTE:
            {
                const DSignedByteField<OT>* field = static_cast<const DSignedByteField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, int8>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::USHORT:
            {
                const DUnsignedShortField<OT>* field = static_cast<const DUnsignedShortField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, uint16>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::SHORT:
            {
                const DShortField<OT>* field = static_cast<const DShortField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, int16>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::UINT:
            {
                const DUnsignedIntField<OT>* field = static_cast<const DUnsignedIntField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, uint32>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::UINT64:
            {
                const DUnsignedInt64Field<OT>* field = static_cast<const DUnsignedInt64Field<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, uint64>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::STRING:
            {
                const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SCoreObjectCopy<SE, BYTE, DString>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::STRING8:
            {
                const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(field->GetPtr(O), OutBytes);
                break;
            }
            case EArchType::FLOAT_STRUCT:
            {
                const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
                cint floatCount = field->GetByteCount() / CINT_SIZEOF(float);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                    uint8 fc = (uint8)floatCount;
                    OutBytes += SPrimitiveCopy<SE, BYTE, uint8>::ItemToBytes((uint8*)&fc, OutBytes);
                }
                OutBytes += SPrimitiveCopy<SE, BYTE, float>::ArrayToBytes(field->GetPtr(O), floatCount, OutBytes);
                break;
            }
            case EArchType::STRUCT:
            {
                const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
                if constexpr (LENIENT)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                }
                OutBytes += ObjectToBytes<DStruct>(field->GetPtr(O), field->GetStructClass(), OutBytes, LengthAsCapacity);
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
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, bool>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::ENUM:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    DArray<uint8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::INT:
                {
                    auto* field = static_cast<const DArrayField<OT, int32>*>(f);
                    DArray<int32>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, int32>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::INT64:
                {
                    auto* field = static_cast<const DArrayField<OT, int64>*>(f);
                    DArray<int64>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, int64>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::FLOAT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    DArray<float>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::DOUBLE:
                {
                    auto* field = static_cast<const DArrayField<OT, double>*>(f);
                    DArray<double>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, double>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::BYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
                    DArray<uint8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, uint8>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::SBYTE:
                {
                    auto* field = static_cast<const DArrayField<OT, int8>*>(f);
                    DArray<int8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, int8>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::SHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, int16>*>(f);
                    DArray<int16>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, int16>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::USHORT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint16>*>(f);
                    DArray<uint16>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, uint16>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::UINT:
                {
                    auto* field = static_cast<const DArrayField<OT, uint32>*>(f);
                    DArray<uint32>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, uint32>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::UINT64:
                {
                    auto* field = static_cast<const DArrayField<OT, uint64>*>(f);
                    DArray<uint64>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, uint64>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::STRING:
                {
                    auto* field = static_cast<const DArrayField<OT, DString>*>(f);
                    DArray<DString>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::STRING8:
                {
                    auto* field = static_cast<const DArrayField<OT, DString8>*>(f);
                    DArray<DString8>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ArrayToBytes(arr->Data(), len, OutBytes);
                    break;
                }
                case EItemType::FLOAT_STRUCT:
                {
                    auto* field = static_cast<const DArrayField<OT, float>*>(f);
                    DArray<float>* arr = field->GetPtr(O);
                    cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                        uint8 fc = (uint8)floatCount;
                        OutBytes += SPrimitiveCopy<SE, BYTE, uint8>::ItemToBytes((uint8*)&fc, OutBytes);
                    }
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ArrayToBytes(arr->Data(), floatCount * len, OutBytes);
                    break;
                }
                case EItemType::STRUCT:
                {
                    auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
                    DArray<DStruct>* arr = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                    }
                    BYTE* arrayStart = reinterpret_cast<BYTE*>(arr->GetDataUnsafe());
                    cint structByteCount = field->GetItemByteCount();
                    // write meta
                    cint len = arr->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? arr->Length() : arr->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    DClass& structClass = field->GetStructClass();
                    cint totalArrayByteCount = structByteCount * len;
                    for (cint off = 0; off < totalArrayByteCount; off += structByteCount)
                    {
                        DStruct* struct_ = reinterpret_cast<DStruct*>(arrayStart + off);
                        OutBytes += ObjectToBytes<DStruct>(struct_, structClass, OutBytes, LengthAsCapacity);
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
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write meta
                    cint len = set->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? set->Length() : set->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    for (uint8 item : *set)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes(&item, OutBytes);
                    }
                    break;
                }
                case EKeyType::INT:
                {
                    auto* field = static_cast<const DSetField<OT, int32>*>(f);
                    DSet<int32>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write meta
                    cint len = set->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? set->Length() : set->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    for (int32 item : *set)
                    {
                        OutBytes += SPrimitiveCopy<SE, BYTE, int32>::ItemToBytes(&item, OutBytes);
                    }
                    break;
                }
                case EKeyType::INT64:
                {
                    auto* field = static_cast<const DSetField<OT, int64>*>(f);
                    DSet<int64>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write meta
                    cint len = set->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? set->Length() : set->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    for (int64 item : *set)
                    {
                        OutBytes += SPrimitiveCopy<SE, BYTE, int64>::ItemToBytes(&item, OutBytes);
                    }
                    break;
                }
                case EKeyType::STRING:
                {
                    auto* field = static_cast<const DSetField<OT, DString>*>(f);
                    DSet<DString>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write meta
                    cint len = set->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? set->Length() : set->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write items
                    for (const DString& item : *set)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString>::ItemToBytes(&item, OutBytes);
                    }
                    break;
                }
                case EKeyType::STRING8:
                {
                    auto* field = static_cast<const DSetField<OT, DString8>*>(f);
                    DSet<DString8>* set = field->GetPtr(O);
                    if constexpr (LENIENT)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                        OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                    }
                    // write meta
                    cint len = set->Length();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
                    cint cap = LengthAsCapacity? set->Length() : set->Capacity();
                    OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
                    // write items
                    for (const DString8& item : *set)
                    {
                        OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&item, OutBytes);
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
                    MapToBytes<OT, uint8, UINT8_ENUM, false>(node, O, Cls, OutBytes, LengthAsCapacity);
                    break;
                }
                case EKeyType::INT:
                {
                    MapToBytes<OT, int32, int32, true>(node, O, Cls, OutBytes, LengthAsCapacity);
                    break;
                }
                case EKeyType::INT64:
                {
                    MapToBytes<OT, int64, int64, true>(node, O, Cls, OutBytes, LengthAsCapacity);
                    break;
                }
                case EKeyType::STRING:
                {
                    MapToBytes<OT, DString, DString, false>(node, O, Cls, OutBytes, LengthAsCapacity);
                    break;
                }
                case EKeyType::STRING8:
                {
                    MapToBytes<OT, DString8, DString8, false>(node, O, Cls, OutBytes, LengthAsCapacity);
                    break;
                }
                case EKeyType::SHARED_OBJECT:
                {
                    break;
                }
                }
            }
            break;
            }
        }

        return (cint)(OutBytes - startPosition);
    }

private:

    // returns byte array progress
    template<typename OT, typename K, typename K_COPY_TYPE, bool PRIMITIVE_KEY>
    static void MapToBytes(const DMapEntry<DString8, DField*>* node, const OT* O, const DClass& Cls, BYTE*& OutBytes, bool LengthAsCapacity)
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
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, bool>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, bool>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::ENUM:
        {
            const DMapField<OT, K, uint8>* field = static_cast<const DMapField<OT, K, uint8>*>(f);
            DMap<K, uint8>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::INT:
        {
            const DMapField<OT, K, int32>* field = static_cast<const DMapField<OT, K, int32>*>(f);
            DMap<K, int32>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, int32>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, int32>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::INT64:
        {
            const DMapField<OT, K, int64>* field = static_cast<const DMapField<OT, K, int64>*>(f);
            DMap<K, int64>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, int64>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, int64>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::FLOAT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            DMap<K, float>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::DOUBLE:
        {
            const DMapField<OT, K, double>* field = static_cast<const DMapField<OT, K, double>*>(f);
            DMap<K, double>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, double>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, double>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::STRING:
        {
            const DMapField<OT, K, DString>* field = static_cast<const DMapField<OT, K, DString>*>(f);
            DMap<K, DString>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString>::ItemToBytes(&entry.GetValue(), OutBytes);
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
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&entry.GetValue(), OutBytes);
                }
            }
            break;
        }
        case EItemType::FLOAT_STRUCT:
        {
            const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
            DMap<K, float>* map = field->GetPtr(O);
            cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
                uint8 fc = (uint8)floatCount;
                OutBytes += SPrimitiveCopy<SE, BYTE, uint8>::ItemToBytes((uint8*)&fc, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ArrayToBytes(&entry.GetValue(), floatCount * len, OutBytes);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += SPrimitiveCopy<SE, BYTE, float>::ArrayToBytes(&entry.GetValue(), floatCount * len, OutBytes);
                }
            }
            break;
        }
        case EItemType::STRUCT:
        {
            const DStructMapField<OT, K, DStruct>* field = static_cast<const DStructMapField<OT, K, DStruct>*>(f);
            DMap<K, DStruct>* map = field->GetPtr(O);
            if constexpr (LENIENT)
            {
                OutBytes += SCoreObjectCopy<SE, BYTE, DString8>::ItemToBytes(&node->GetKey(), OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ArchType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->KeyType, OutBytes);
                OutBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::ItemToBytes((uint8*)&f->ItemType, OutBytes);
            }
            // write meta
            cint len = map->Length();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&len, OutBytes);
            cint cap = LengthAsCapacity? map->Length() : map->Capacity();
            OutBytes += SPrimitiveCopy<SE, BYTE, cint>::ItemToBytes(&cap, OutBytes);
            // write items
            DClass& structClass = field->GetItemStructClass();
            if constexpr (PRIMITIVE_KEY)
            {
                for (auto& entry : *map)
                {
                    OutBytes += SPrimitiveCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += ObjectToBytes(&entry.GetValue(), structClass, OutBytes, LengthAsCapacity);
                }
            }
            else
            {
                for (auto& entry : *map)
                {
                    OutBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::ItemToBytes(&entry.GetKey(), OutBytes);
                    OutBytes += ObjectToBytes(&entry.GetValue(), structClass, OutBytes, LengthAsCapacity);
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
