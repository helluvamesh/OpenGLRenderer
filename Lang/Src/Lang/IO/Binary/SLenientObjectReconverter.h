#pragma once

#include "SPrimitiveCopy.h"
#include "SCoreObjectCopy.h"


// SE: same endian
template<bool SE, typename BYTE>
class SLenientObjectReconverter
{
public:
	// OT: HObject or DStruct
	// returns number of bytes read
	template<typename OT>
	static cint BytesToObject(const BYTE* InBytes, OT* O, const DClass& Cls)
	{
        const BYTE* startPosition = InBytes;

        // fieldCount
        uint16 totalFieldCount;
        InBytes += SPrimitiveCopy<SE, BYTE, uint16>::BytesToItem(InBytes, &totalFieldCount);

        uint16 fieldIndex = 0;
        while (fieldIndex < totalFieldCount)
        {
            DString8 fieldName;
            InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, &fieldName);
            uint8 fieldArchType_v;
            InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItem(InBytes, &fieldArchType_v);
            EArchType readFieldArchType = (EArchType)fieldArchType_v;
            const DField* f = Cls.FindField(fieldName);
            bool fieldFound = f && f->ArchType == readFieldArchType;
                
            switch (readFieldArchType)
            {
            case EArchType::BOOL:
            {
                if (fieldFound)
                {
                    const DBoolField<OT>* field = static_cast<const DBoolField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, bool>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(bool);
                }
                break;
            }
            case EArchType::ENUM:
            {
                if (fieldFound)
                {
                    const DEnumField<OT, uint8>* field = static_cast<const DEnumField<OT, uint8>*>(f);
                    InBytes += SCoreObjectCopy<SE, BYTE, UINT8_ENUM>::BytesToItemChecked(InBytes, field->GetEnumLength(), field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(uint8);
                }
                break;
            }
            case EArchType::INT:
            {
                if (fieldFound)
                {
                    const DIntField<OT>* field = static_cast<const DIntField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, int32>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(int32);
                }
                break;
            }
            case EArchType::INT64:
            {
                if (fieldFound)
                {
                    const DInt64Field<OT>* field = static_cast<const DInt64Field<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, int64>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(int64);
                }
                break;
            }
            case EArchType::FLOAT:
            {
                if (fieldFound)
                {
                    const DFloatField<OT>* field = static_cast<const DFloatField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(float);
                }
                break;
            }
            case EArchType::DOUBLE:
            {
                if (fieldFound)
                {
                    const DDoubleField<OT>* field = static_cast<const DDoubleField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, double>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(double);
                }
                break;
            }
            case EArchType::BYTE:
            {
                if (fieldFound)
                {
                    const DByteField<OT>* field = static_cast<const DByteField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(uint8);
                }
                break;
            }
            case EArchType::SBYTE:
            {
                if (fieldFound)
                {
                    const DSignedByteField<OT>* field = static_cast<const DSignedByteField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, int8>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(int8);
                }
                break;
            }
            case EArchType::USHORT:
            {
                if (fieldFound)
                {
                    const DUnsignedShortField<OT>* field = static_cast<const DUnsignedShortField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint16>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(uint16);
                }
                break;
            }
            case EArchType::SHORT:
            {
                if (fieldFound)
                {
                    const DShortField<OT>* field = static_cast<const DShortField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, int16>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(int16);
                }
                break;
            }
            case EArchType::UINT:
            {
                if (fieldFound)
                {
                    const DUnsignedIntField<OT>* field = static_cast<const DUnsignedIntField<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint32>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(uint32);
                }
                break;
            }
            case EArchType::UINT64:
            {
                if (fieldFound)
                {
                    const DUnsignedInt64Field<OT>* field = static_cast<const DUnsignedInt64Field<OT>*>(f);
                    InBytes += SPrimitiveCopy<SE, BYTE, uint64>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += CINT_SIZEOF(uint64);
                }
                break;
            }
            case EArchType::STRING:
            {
                if (fieldFound)
                {
                    const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, DString>::SkipItemBytes(InBytes);
                }
                break;
            }
            case EArchType::STRING8:
            {
                if (fieldFound)
                {
                    const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
                    InBytes += SCoreObjectCopy<SE, BYTE, DString8>::BytesToItem(InBytes, field->GetPtr(O));
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, DString8>::SkipItemBytes(InBytes);
                }
                break;
            }
            case EArchType::FLOAT_STRUCT:
            {
                if (fieldFound)
                {
                    const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
                    cint floatCount = field->GetByteCount() / CINT_SIZEOF(float);
                    uint8 readFloatCount;
                    InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &readFloatCount);
                    if (floatCount == readFloatCount)
                    {
                        InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, floatCount, field->GetPtr(O));
                    }
                    else
                    {
                        InBytes += CINT_SIZEOF(float) * floatCount;  // skip
                    }
                }
                else
                {
                    // skip
                    uint8 floatCount;
                    InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &floatCount);
                    InBytes += CINT_SIZEOF(float) * floatCount;
                }
                break;
            }
            case EArchType::STRUCT:
            {
                if (fieldFound)
                {
                    const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
                    InBytes += BytesToObject<DStruct>(InBytes, field->GetPtr(O), field->GetStructClass());
                }
                else
                {
                    DStruct empty;
                    InBytes += BytesToObject<DStruct>(InBytes, &empty, DStruct::GetDeclaredClass());
                }
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
                uint8 arrayItemType_v;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &arrayItemType_v);
                EItemType arrayItemType = (EItemType)arrayItemType_v;
                if (fieldFound)
                {
                    fieldFound = arrayItemType == f->ItemType;
                }

                switch (arrayItemType)
                {
                case EItemType::BOOL:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(bool) * len;
                    }
                    break;
                }
                case EItemType::ENUM:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint8) * len;
                    }
                    break;
                }
                case EItemType::INT:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int32) * len;
                    }
                    break;
                }
                case EItemType::INT64:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int64) * len;
                    }
                    break;
                }
                case EItemType::FLOAT:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(float) * len;
                    }
                    break;
                }
                case EItemType::DOUBLE:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(double) * len;
                    }
                    break;
                }
                case EItemType::BYTE:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint8) * len;
                    }
                    break;
                }
                case EItemType::SBYTE:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int8) * len;
                    }
                    break;
                }
                case EItemType::SHORT:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int16) * len;
                    }
                    break;
                }
                case EItemType::USHORT:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint16) * len;
                    }
                    break;
                }
                case EItemType::UINT:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint32) * len;
                    }
                    break;
                }
                case EItemType::UINT64:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint64) * len;
                    }
                    break;
                }
                case EItemType::STRING:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(tchar) * len;
                    }
                    break;
                }
                case EItemType::STRING8:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(char8) * len;
                    }
                    break;
                }
                case EItemType::FLOAT_STRUCT:
                {
                    if (fieldFound)
                    {
                        auto* field = static_cast<const DArrayField<OT, float>*>(f);
                        // read meta
                        uint8 readFloatCount;
                        InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &readFloatCount);
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        // read items
                        cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                        if (readFloatCount == floatCount)
                        {
                            cint totalFloatCount = floatCount * len;
                            field->ClearAndReserve(O, cap);
                            InBytes += SPrimitiveCopy<SE, BYTE, float>::BytesToArray(InBytes, totalFloatCount, field->Data(O));
                            field->OverwriteLength(O, len);
                        }
                        else
                        {
                            InBytes += CINT_SIZEOF(float) * floatCount * len; // skip
                        }
                    }
                    else
                    {
                        // skip
                        uint8 floatCount;
                        InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &floatCount);
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(float) * floatCount * len;
                    }
                    break;
                }
                case EItemType::STRUCT:
                {
                    if (fieldFound)
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
                        field->Resize(O, len);
                        BYTE* arrayStart = reinterpret_cast<BYTE*>(field->Data(O));
                        cint totalArrayByteCount = structByteCount * len;
                        DClass& structClass = field->GetStructClass();
                        for (cint off = 0; off < totalArrayByteCount; off += structByteCount)
                        {
                            DStruct* struct_ = reinterpret_cast<DStruct*>(arrayStart + off);
                            InBytes += BytesToObject<DStruct>(InBytes, struct_, structClass);
                        }
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        DStruct empty;
                        DClass& structClass = DStruct::GetDeclaredClass();
                        for (cint i = 0; i < len; ++i)
                        {
                            InBytes += BytesToObject<DStruct>(InBytes, &empty, structClass);
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
                break;
            }
            case EArchType::SET:
            {
                uint8 setKeyType_v;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &setKeyType_v);
                EKeyType setKeyType = (EKeyType)setKeyType_v;
                if (fieldFound)
                {
                    fieldFound = setKeyType == f->KeyType;
                }

                switch (setKeyType)
                {
                case EKeyType::ENUM:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(uint8) * len;
                    }
                    break;
                }
                case EKeyType::INT:
                {
                    if (fieldFound)
                    {
                        auto* field = static_cast<const DSetField<OT, int32>*>(f);
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int32) * len;
                    }
                    break;
                }
                case EKeyType::INT64:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(int64) * len;
                    }
                    break;
                }
                case EKeyType::STRING:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(tchar) * len;
                    }
                    break;
                }
                case EKeyType::STRING8:
                {
                    if (fieldFound)
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
                    }
                    else
                    {
                        cint len;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                        cint cap;
                        InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                        InBytes += CINT_SIZEOF(char8) * len;
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
                uint8 mapKeyType_v;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &mapKeyType_v);
                EKeyType mapKeyType = (EKeyType)mapKeyType_v;

                uint8 mapItemType_v;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &mapItemType_v);
                EItemType mapItemType = (EItemType)mapItemType_v;

                if (fieldFound)
                {
                    fieldFound = mapKeyType == f->KeyType && mapItemType == f->ItemType;
                }

                switch (mapKeyType)
                {
                case EKeyType::ENUM:
                {
                    BytesToMap<OT, uint8, UINT8_ENUM, false>(mapItemType, fieldFound, f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::INT:
                {
                    BytesToMap<OT, int32, int32, true>(mapItemType, fieldFound, f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::INT64:
                {
                    BytesToMap<OT, int64, int64, true>(mapItemType, fieldFound, f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::STRING:
                {
                    BytesToMap<OT, DString, DString, false>(mapItemType, fieldFound, f, InBytes, O, Cls);
                    break;
                }
                case EKeyType::STRING8:
                {
                    BytesToMap<OT, DString8, DString8, false>(mapItemType, fieldFound, f, InBytes, O, Cls);
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

            fieldIndex++;
        }

        return static_cast<cint>(InBytes - startPosition);
	}
       
private:

    // returns byte array progress
    template<typename OT, typename K, typename K_COPY_TYPE, bool PRIMITIVE_KEY>
    static void BytesToMap(EItemType itemType, bool fieldFound, const DField* f, const BYTE*& InBytes, OT* O, const DClass& Cls)
    {
        switch (itemType)
        {
        case EItemType::BOOL:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(bool) * len;
            }
            break;
        }
        case EItemType::ENUM:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(uint8) * len;
            }
            break;
        }
        case EItemType::INT:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(int32) * len;
            }
            break;
        }
        case EItemType::INT64:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(int64) * len;
            }
            break;
        }
        case EItemType::FLOAT:
        {
            if (fieldFound)
            {
                const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
                DMap<K, float>* map = field->GetPtr(O);
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(float) * len;
            }
            break;
        }
        case EItemType::DOUBLE:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += CINT_SIZEOF(double) * len;
            }
            break;
        }
        case EItemType::STRING:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += SCoreObjectCopy<SE, BYTE, DString>::SkipArrayBytes(InBytes, len);
            }
            break;
        }
        case EItemType::STRING8:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                InBytes += SCoreObjectCopy<SE, BYTE, DString8>::SkipArrayBytes(InBytes, len);
            }
            break;
        }
        case EItemType::FLOAT_STRUCT:
        {
            if (fieldFound)
            {
                const DMapField<OT, K, float>* field = static_cast<const DMapField<OT, K, float>*>(f);
                // read meta
                uint8 readFloatCount;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &readFloatCount);
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
                if (floatCount == readFloatCount)
                {
                    // read items
                    field->ClearAndReserve(O, cap);
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
                }
                else
                {
                    // skip
                    InBytes += CINT_SIZEOF(float) * floatCount * len;
                }
            }
            else
            {
                // skip
                uint8 floatCount;
                InBytes += SPrimitiveCopy<SE, BYTE, uint8>::BytesToItem(InBytes, &floatCount);
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                if constexpr (PRIMITIVE_KEY)
                {
                    InBytes += CINT_SIZEOF(K) * len;
                }
                else
                {
                    InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipArrayBytes(InBytes, len);
                }
                // skip
                InBytes += CINT_SIZEOF(float) * floatCount * len;
            }
            break;
        }
        case EItemType::STRUCT:
        {
            if (fieldFound)
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
            }
            else
            {
                cint len;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &len);
                cint cap;
                InBytes += SPrimitiveCopy<SE, BYTE, cint>::BytesToItem(InBytes, &cap);
                DStruct empty;
                DClass& structClass = DStruct::GetDeclaredClass();
                if constexpr (PRIMITIVE_KEY)
                {
                    for (cint i = 0; i < len; ++i)
                    {
                        InBytes += CINT_SIZEOF(K);
                        InBytes += BytesToObject(InBytes, &empty, structClass);
                    }
                }
                else
                {
                    for (cint i = 0; i < len; ++i)
                    {
                        InBytes += SCoreObjectCopy<SE, BYTE, K_COPY_TYPE>::SkipItemBytes(InBytes);
                        InBytes += BytesToObject(InBytes, &empty, structClass);
                    }
                }
            }
            break;
        }
        }
    }
};
