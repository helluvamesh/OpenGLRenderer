#pragma once

#include "CoreTypes.h"


template<typename Nothing, typename OT, typename T>
struct SFieldSelector { };

// -----------------------------------------------------------------------------
// --------------------------------- SIMPLE ------------------------------------
// -----------------------------------------------------------------------------
    
template<typename OT> 
struct SFieldSelector<void, OT, bool> 
{ 
    using FIELD_TYPE = DBoolField<OT>; 
    using TYPE = bool; 
    static const EItemType ITEM_TYPE = EItemType::BOOL; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(bool); 
};

template<typename OT> 
struct SFieldSelector<void, OT, int32> 
{ 
    using FIELD_TYPE = DIntField<OT>; 
    using TYPE = int32; 
    static const EItemType ITEM_TYPE = EItemType::INT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int32); 
};
    
template<typename OT> 
struct SFieldSelector<void, OT, int64> 
{ 
    using FIELD_TYPE = DInt64Field<OT>; 
    using TYPE = int64; 
    static const EItemType ITEM_TYPE = EItemType::INT64; 
    static const EKeyType KEY_TYPE = EKeyType::INT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int64); 
};

template<typename OT> 
struct SFieldSelector<void, OT, float> 
{ 
    using FIELD_TYPE = DFloatField<OT>; 
    using TYPE = float; 
    static const EItemType ITEM_TYPE = EItemType::FLOAT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(float);
};

template<typename OT> 
struct SFieldSelector<void, OT, double> 
{ 
    using FIELD_TYPE = DDoubleField<OT>; 
    using TYPE = double;
    static const EItemType ITEM_TYPE = EItemType::DOUBLE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(double); 
};

template<typename OT> 
struct SFieldSelector<void, OT, uint8> 
{ 
    using FIELD_TYPE = DByteField<OT>; 
    using TYPE = uint8; 
    static const EItemType ITEM_TYPE = EItemType::BYTE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint8); 
};

template<typename OT> 
struct SFieldSelector<void, OT, int8> 
{ 
    using FIELD_TYPE = DSignedByteField<OT>; 
    using TYPE = int8; 
    static const EItemType ITEM_TYPE = EItemType::SBYTE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int8); 
};

template<typename OT> 
struct SFieldSelector<void, OT, uint16> 
{ 
    using FIELD_TYPE = DUnsignedShortField<OT>; 
    using TYPE = uint16; 
    static const EItemType ITEM_TYPE = EItemType::USHORT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint16); 
};

template<typename OT> 
struct SFieldSelector<void, OT, int16> 
{ 
    using FIELD_TYPE = DShortField<OT>; 
    using TYPE = int16; 
    static const EItemType ITEM_TYPE = EItemType::SHORT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int16); 
};

template<typename OT> 
struct SFieldSelector<void, OT, uint32> 
{ 
    using FIELD_TYPE = DUnsignedIntField<OT>; 
    using TYPE = uint32; 
    static const EItemType ITEM_TYPE = EItemType::UINT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint32); 
};

template<typename OT> 
struct SFieldSelector<void, OT, uint64> 
{ 
    using FIELD_TYPE = DUnsignedInt64Field<OT>; 
    using TYPE = uint64; 
    static const EItemType ITEM_TYPE = EItemType::UINT64; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint64); 
};

template<typename OT> 
struct SFieldSelector<void, OT, DString> 
{ 
    using FIELD_TYPE = DStringField<OT>;
    using TYPE = DString;
    static const EItemType ITEM_TYPE = EItemType::STRING; 
    static const EKeyType KEY_TYPE = EKeyType::INT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString);
};

template<typename OT> 
struct SFieldSelector<void, OT, DString8> 
{ 
    using FIELD_TYPE = DString8Field<OT>; 
    using TYPE = DString8;
    static const EItemType ITEM_TYPE = EItemType::STRING8; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString8); 
};

template<typename OT, typename T> struct SFieldSelector<void, OT, DSharedPtr<T>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_convertible<T*, HObject*>::value,
        DSharedObjectField<OT>,
        void
    >::type; 
    using TYPE = DSharedPtr<HObject>;
    static const EItemType ITEM_TYPE = EItemType::SHARED_OBJECT; 
    static const EKeyType KEY_TYPE = EKeyType::INT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DSharedPtr<HObject>);
};

template<typename OT, typename T> struct SFieldSelector<void, OT, DWeakPtr<T>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_convertible<T*, HObject*>::value,
        DWeakObjectField<OT>,
        void
    >::type; 
    using TYPE = DWeakPtr<HObject>; 
    static const EItemType ITEM_TYPE = EItemType::WEAK_OBJECT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DWeakPtr<HObject>);
};

template<typename OT, typename T> struct SFieldSelector<void, OT, T> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_base_of<DStruct, T>::value, 
        DStructField<OT, T>,
        typename std::conditional<
            std::is_enum<T>::value,
            DEnumField<OT, T>,
            typename std::conditional<
                std::is_base_of<FFloatStruct, T>::value,
                DFloatStructField<OT, T>,
                void
            >::type
        >::type
    >::type;
    using TYPE = T;
    static const EItemType ITEM_TYPE = 
        std::is_enum<T>::value? 
            EItemType::ENUM :   
            (std::is_base_of<DStruct, T>::value? EItemType::STRUCT : EItemType::FLOAT_STRUCT); 
    static const EKeyType KEY_TYPE = EKeyType::INT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(T);
};

// -----------------------------------------------------------------------------
// --------------------------------- DARRAY ------------------------------------
// -----------------------------------------------------------------------------

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<bool, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, bool, AllocatorT>; 
    using TYPE = DArray<bool, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BOOL;
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(bool);
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<int32, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, int32, AllocatorT>; 
    using TYPE = DArray<int32, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::INT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int32); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<int64, AllocatorT>>
{ 
    using FIELD_TYPE = DArrayField<OT, int64, AllocatorT>; 
    using TYPE = DArray<int64, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::INT64; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int64); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<float, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, float, AllocatorT>; 
    using TYPE = DArray<float, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::FLOAT;
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(float); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<double, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, double, AllocatorT>; 
    using TYPE = DArray<double, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::DOUBLE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(double); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<uint8, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, uint8, AllocatorT>; 
    using TYPE = DArray<uint8, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BYTE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint8); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<int8, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, int8, AllocatorT>; 
    using TYPE = DArray<int8, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::SBYTE; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int8); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<uint16, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, uint16, AllocatorT>; 
    using TYPE = DArray<uint16, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::USHORT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint16); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<int16, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, int16, AllocatorT>; 
    using TYPE = DArray<int16, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::SHORT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int16); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<uint32, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, uint32, AllocatorT>; 
    using TYPE = DArray<uint32, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::UINT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint32); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<uint64, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, uint64, AllocatorT>; 
    using TYPE = DArray<uint64, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::UINT64; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(uint64); 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<DString, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, DString, AllocatorT>; 
    using TYPE = DArray<DString, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::STRING; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString);
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<DString8, AllocatorT>> 
{ 
    using FIELD_TYPE = DArrayField<OT, DString8, AllocatorT>; 
    using TYPE = DArray<DString8, AllocatorT>;
    static const EItemType ITEM_TYPE = EItemType::STRING8; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString8); 
};

template<typename OT, typename T, class AllocatorT>
struct SFieldSelector<void, OT, DArray<DSharedPtr<T>, AllocatorT>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_convertible<T*, HObject*>::value,
        DArrayField<OT, DSharedPtr<HObject>, AllocatorT>,
        void
    >::type; 
    using TYPE = DArray<DSharedPtr<HObject>, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::SHARED_OBJECT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DSharedPtr<HObject>);
};

template<typename OT, typename T, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<DWeakPtr<T>, AllocatorT>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_convertible<T*, HObject*>::value,
        DArrayField<OT, DWeakPtr<HObject>, AllocatorT>,
        void
    >::type; 
    using TYPE = DArray<DWeakPtr<HObject>, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::WEAK_OBJECT; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DWeakPtr<HObject>);
};

template<typename OT, typename T, class AllocatorT> 
struct SFieldSelector<void, OT, DArray<T, AllocatorT>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_base_of<DStruct, T>::value, 
        DStructArrayField<OT, T, AllocatorT>,
        typename std::conditional<
            std::is_enum<T>::value || std::is_base_of<FFloatStruct, T>::value,
            DArrayField<OT, T, AllocatorT>,
            void
        >::type
    >::type;
    using TYPE = DArray<T, AllocatorT>;
    static const EItemType ITEM_TYPE = 
        std::is_enum<T>::value? 
            EItemType::ENUM :
            (std::is_base_of<DStruct, T>::value? EItemType::STRUCT : EItemType::FLOAT_STRUCT); 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(T);
};

// -----------------------------------------------------------------------------
// --------------------------------- DSET --------------------------------------
// -----------------------------------------------------------------------------

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DSet<int32, AllocatorT>> 
{ 
    using FIELD_TYPE = DSetField<OT, int32, AllocatorT>;
    using TYPE = DSet<int32, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BOOL; 
    static const EKeyType KEY_TYPE = EKeyType::INT; 
    static const int32 ITEM_BYTE_COUNT = 0; 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DSet<int64, AllocatorT>> 
{ 
    using FIELD_TYPE = DSetField<OT, int64, AllocatorT>; 
    using TYPE = DSet<int64, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BOOL; 
    static const EKeyType KEY_TYPE = EKeyType::INT64; 
    static const int32 ITEM_BYTE_COUNT = 0; 
};

template<typename OT, class AllocatorT> 
struct SFieldSelector<void, OT, DSet<DString, AllocatorT>> 
{ 
    using FIELD_TYPE = DSetField<OT, DString, AllocatorT>; 
    using TYPE = DSet<DString, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BOOL;
    static const EKeyType KEY_TYPE = EKeyType::STRING;
    static const int32 ITEM_BYTE_COUNT = 0; 
};

template<typename OT, class AllocatorT>
struct SFieldSelector<void, OT, DSet<DString8, AllocatorT>> 
{ 
    using FIELD_TYPE = DSetField<OT, DString8, AllocatorT>;
    using TYPE = DSet<DString8, AllocatorT>;
    static const EItemType ITEM_TYPE = EItemType::BOOL;
    static const EKeyType KEY_TYPE = EKeyType::STRING8; 
    static const int32 ITEM_BYTE_COUNT = 0; 
};

template<typename OT, typename T, class AllocatorT> 
struct SFieldSelector<void, OT, DSet<DSharedPtr<T>, AllocatorT>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_convertible<T*, HObject*>::value,
        DSetField<OT, DSharedPtr<HObject>, AllocatorT>,
        void
    >::type; 
    using TYPE = DSet<DSharedPtr<HObject>, AllocatorT>; 
    static const EItemType ITEM_TYPE = EItemType::BOOL; 
    static const EKeyType KEY_TYPE = EKeyType::SHARED_OBJECT; 
    static const int32 ITEM_BYTE_COUNT = 0;
};

template<typename OT, typename T, class AllocatorT> 
struct SFieldSelector<void, OT, DSet<T, AllocatorT>> 
{ 
    using FIELD_TYPE = typename std::conditional<
        std::is_enum<T>::value,
        DSetField<OT, T, AllocatorT>,
        void
    >::type;
    using TYPE = DSet<T, AllocatorT>;
    static const EItemType ITEM_TYPE = EItemType::BOOL;
    static const EKeyType KEY_TYPE = EKeyType::ENUM;
    static const int32 ITEM_BYTE_COUNT = 0;
};

// -----------------------------------------------------------------------------
// --------------------------------- DMAP --------------------------------------
// -----------------------------------------------------------------------------
    
template<typename Nothing, typename K>
struct SMapKeySelector { };

template<typename Nothing, typename K>
struct SMapValueSelector { };

template<> 
struct SMapKeySelector<void, int32> 
{ 
    using TYPE = int32; 
    static const EKeyType KEY_TYPE = EKeyType::INT;
};

template<> 
struct SMapKeySelector<void, int64> 
{ 
    using TYPE = int64;
    static const EKeyType KEY_TYPE = EKeyType::INT64;
};

template<> 
struct SMapKeySelector<void, DString> 
{ 
    using TYPE = DString; 
    static const EKeyType KEY_TYPE = EKeyType::STRING;
};

template<> 
struct SMapKeySelector<void, DString8>
{ 
    using TYPE = DString8; 
    static const EKeyType KEY_TYPE = EKeyType::STRING8;
};

template<typename T> 
struct SMapKeySelector<void, DSharedPtr<T>> 
{ 
    using TYPE = typename std::enable_if<
        std::is_convertible<T*, HObject*>::value,
        DSharedPtr<T>
    >::type; 
    static const EKeyType KEY_TYPE = EKeyType::SHARED_OBJECT;
};

template<typename T> 
struct SMapKeySelector<void, T>
{ 
    using TYPE = typename std::enable_if<
        std::is_enum<T>::value,
        T
    >::type; 
    static const EKeyType KEY_TYPE = EKeyType::ENUM;
};

template<> 
struct SMapValueSelector<void, bool> 
{ 
    using TYPE = bool;
    static const EItemType ITEM_TYPE = EItemType::BOOL;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(bool);
};

template<> 
struct SMapValueSelector<void, int32>
{ 
    using TYPE = int32; 
    static const EItemType ITEM_TYPE = EItemType::INT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int32);
};

template<> 
struct SMapValueSelector<void, int64> 
{ 
    using TYPE = int64;
    static const EItemType ITEM_TYPE = EItemType::INT64;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(int64);
};

template<> 
struct SMapValueSelector<void, float> 
{ 
    using TYPE = float; 
    static const EItemType ITEM_TYPE = EItemType::FLOAT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(float);
};

template<> 
struct SMapValueSelector<void, double> 
{ 
    using TYPE = double; 
    static const EItemType ITEM_TYPE = EItemType::DOUBLE;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(double);
};

template<> 
struct SMapValueSelector<void, DString>
{ 
    using TYPE = DString; 
    static const EItemType ITEM_TYPE = EItemType::STRING;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString);
};

template<> 
struct SMapValueSelector<void, DString8>
{ 
    using TYPE = DString8; 
    static const EItemType ITEM_TYPE = EItemType::STRING8;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DString8);
};

template<typename T>
struct SMapValueSelector<void, DSharedPtr<T>> 
{ 
    using TYPE = typename std::enable_if<
        std::is_convertible<T*, HObject*>::value,
        DSharedPtr<T>
    >::type; 
    static const EItemType ITEM_TYPE = EItemType::SHARED_OBJECT;
};

template<typename T>
struct SMapValueSelector<void, DWeakPtr<T>>
{ 
    using TYPE = typename std::enable_if<
        std::is_convertible<T*, HObject*>::value,
        DWeakPtr<T>
    >::type; 
    static const EItemType ITEM_TYPE = EItemType::WEAK_OBJECT;
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(DSharedPtr<HObject>);
};

template<typename T>
struct SMapValueSelector<void, T> 
{  
    using TYPE = typename std::enable_if<
        std::is_base_of<DStruct, T>::value || std::is_enum<T>::value || std::is_base_of<FFloatStruct, T>::value, 
        T
    >::type;
    static const EItemType ITEM_TYPE = 
        std::is_enum<T>::value? 
            EItemType::ENUM : 
            (std::is_base_of<DStruct, T>::value? EItemType::STRUCT : EItemType::FLOAT_STRUCT);
    static const cint ITEM_BYTE_COUNT = CINT_SIZEOF(T);
};


template<typename OT, typename K, typename V, class AllocatorT> 
struct SFieldSelector<void, OT, DMap<K, V, AllocatorT>> 
{
    using KS = typename SMapKeySelector<void, K>;
    using VS = typename SMapValueSelector<void, V>;
    using FIELD_TYPE = typename std::conditional<
        VS::ITEM_TYPE == EItemType::STRUCT,
        DStructMapField<OT, typename KS::TYPE, typename VS::TYPE, AllocatorT>,
        DMapField<OT, typename KS::TYPE, typename VS::TYPE, AllocatorT>
    >::type;
    using TYPE = DMap<typename KS::TYPE, typename VS::TYPE, AllocatorT>; 
    static const EKeyType KEY_TYPE = KS::KEY_TYPE;
    static const EItemType ITEM_TYPE = VS::ITEM_TYPE;
    static const int32 ITEM_BYTE_COUNT = VS::ITEM_BYTE_COUNT;
};


template<typename OT, typename T>
struct SMainFieldSelector : SFieldSelector<void, OT, T> { };


#define FIELD(__MemberName, ...) \
{ \
    typedef SMainFieldSelector<BaseObjectType, decltype(__MemberName)> selector; \
    ClassData.AddField( \
        u8#__MemberName, \
        new selector::FIELD_TYPE( \
            reinterpret_cast<selector::TYPE BaseObjectType::*>(&cls::__MemberName), selector::ITEM_TYPE, selector::KEY_TYPE, selector::ITEM_BYTE_COUNT, __VA_ARGS__ \
        ) \
    ); \
}


#define ALIAS_FIELD(__MemberName, __Alias, ...) \
{ \
    typedef SMainFieldSelector<BaseObjectType, decltype(__MemberName)> selector; \
    ClassData.AddField( \
        __Alias, \
        new selector::FIELD_TYPE( \
            reinterpret_cast<selector::TYPE BaseObjectType::*>(&cls::__MemberName), selector::ITEM_TYPE, selector::KEY_TYPE, selector::ITEM_BYTE_COUNT, __VA_ARGS__ \
        ) \
    ); \
}
