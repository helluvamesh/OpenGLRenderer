#pragma once


template<typename ObjectType>
class DBoolField : public DField
{
private:
    bool ObjectType::* Member;

public:
    DBoolField(bool ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) : 
        Member(InMember),
        DField(EArchType::BOOL) 
    {
    }

    bool* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType, typename T>
class DEnumField : public DField
{
private:
    T ObjectType::* Member;
    uint8 EnumLength;

public:
    DEnumField(T ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) : 
        Member(InMember), 
        EnumLength((uint8)T::__Length),
        DField(EArchType::ENUM) 
    { 
        static_assert(
            std::is_same<std::underlying_type<T>::type, uint8>::value,
            "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
            );
    }


    T* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    uint8 GetEnumLength() const
    {
        return this->EnumLength;
    }

    virtual const char* GetTypeName() const
    {
        return typeid(T).name();
    }
};


template<typename ObjectType>
class DIntField : public DField
{
private:
    int32 ObjectType::* Member;
    int32 UIMin;
    int32 UIMax;
    int32 UIIncrement;

public:
    DIntField(int32 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount, int32 uiMin = 0, int32 uiMax = 0, int32 uiIncrement = 1) : 
        Member(InMember), 
        UIMin(uiMin), 
        UIMax(uiMax),
        UIIncrement(uiIncrement), 
        DField(EArchType::INT) 
    {
    }

    int32* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    int32 GetUIMin() const
    {
        return this->UIMin;
    }

    int32 GetUIMax() const
    {
        return this->UIMax;
    }

    int32 GetUIIncrement() const
    {
        return this->UIIncrement;
    }
};


template<typename ObjectType>
class DInt64Field : public DField
{
private:
    int64 ObjectType::* Member;
    int64 UIMin;
    int64 UIMax;
    int64 UIIncrement;

public:
    DInt64Field(int64 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount, int64 uiMin = 0, int64 uiMax = 0, int64 uiIncrement = 1) :
        Member(InMember), 
        UIMin(uiMin),
        UIMax(uiMax), 
        UIIncrement(uiIncrement), 
        DField(EArchType::INT64) 
    {
    }

    int64* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    int64 GetUIMin() const
    {
        return this->UIMin;
    }

    int64 GetUIMax() const
    {
        return this->UIMax;
    }

    int64 GetUIIncrement() const
    {
        return this->UIIncrement;
    }
};


template<typename ObjectType>
class DFloatField : public DField
{
private:
    float ObjectType::* Member;
    float UIMin;
    float UIMax;
    float UIIncrement;

public:
    DFloatField(float ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount, float uiMin = 0.0f, float uiMax = 0.0f, float uiIncrement = 1.0f) :
        Member(InMember), 
        UIMin(uiMin), 
        UIMax(uiMax),
        UIIncrement(uiIncrement),
        DField(EArchType::FLOAT)
    {
    }

    float* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    float GetUIMin() const
    {
        return this->UIMin;
    }

    float GetUIMax() const
    {
        return this->UIMax;
    }

    float GetUIIncrement() const
    {
        return this->UIIncrement;
    }
};


template<typename ObjectType>
class DDoubleField : public DField
{
private:
    double ObjectType::* Member;
    double UIMin;
    double UIMax;
    double UIIncrement;

public:
    DDoubleField(double ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount, double uiMin = 0.0, double uiMax = 0.0, double uiIncrement = 1.0) :
        Member(InMember), 
        UIMin(uiMin),
        UIMax(uiMax),
        UIIncrement(uiIncrement),
        DField(EArchType::DOUBLE) 
    {
    }

    double* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    double GetUIMin() const
    {
        return this->UIMin;
    }

    double GetUIMax() const
    {
        return this->UIMax;
    }

    double GetUIIncrement() const
    {
        return this->UIIncrement;
    }
};


template<typename ObjectType>
class DByteField : public DField
{
private:
    uint8 ObjectType::* Member;

public:
    DByteField(uint8 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::BYTE) 
    {
    }

    uint8* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DSignedByteField : public DField
{
private:
    int8 ObjectType::* Member;

public:
    DSignedByteField(int8 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::SBYTE) 
    {
    }

    int8* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DUnsignedShortField : public DField
{
private:
    uint16 ObjectType::* Member;

public:
    DUnsignedShortField(uint16 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::USHORT) 
    {
    }

    uint16* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DShortField : public DField
{
private:
    int16 ObjectType::* Member;

public:
    DShortField(int16 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::SHORT) 
    {
    }

    int16* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DUnsignedIntField : public DField
{
private:
    uint32 ObjectType::* Member;

public:
    DUnsignedIntField(uint32 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::UINT) 
    {
    }

    uint32* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};



template<typename ObjectType>
class DUnsignedInt64Field : public DField
{
private:
    uint64 ObjectType::* Member;

public:
    DUnsignedInt64Field(uint64 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::UINT64) 
    {
    }

    uint64* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DStringField : public DField
{
private:
    DString ObjectType::* Member;
    uint8 UILineCount;

public:
    DStringField(DString ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount, uint8 uiLineCount = 0) :
        Member(InMember), 
        UILineCount(uiLineCount), 
        DField(EArchType::STRING)
    {
    }

    DString* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    uint8 GetUILineCount() const
    {
        return this->UILineCount;
    }
};


template<typename ObjectType>
class DString8Field : public DField
{
private:
    DString8 ObjectType::* Member;

public:
    DString8Field(DString8 ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember), 
        DField(EArchType::STRING8)
    {
    }

    DString8* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType, typename T>
class DFloatStructField : public DField
{
private:
    T ObjectType::* Member; 
    cint ByteCount;

public:

    DFloatStructField(T ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InByteCount) :
        Member(InMember), 
        ByteCount(InByteCount), 
        DField(EArchType::FLOAT_STRUCT)
    {
        static_assert(
            std::is_default_constructible<T>::value && std::is_polymorphic<T>::value == false,
            "Reflected structs must be default constructible and must not be polymorphic (virtual methods are not allowed)"
        );
    }

    T* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual const char* GetTypeName() const
    {
        return typeid(T).name();
    }

    cint GetByteCount() const
    {
        return this->ByteCount;
    }
};


template<typename ObjectType, typename T>
class DStructField : public DField
{
private:
    T ObjectType::* Member;
    cint ByteCount;

public:
    DStructField(T ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InByteCount) :
        Member(InMember), 
        ByteCount(InByteCount),
        DField(EArchType::STRUCT)
    {
    }

    DStruct* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual DClass& GetStructClass() const
    {
        return T::GetDeclaredClass();
    }

    cint GetByteCount() const
    {
        return this->ByteCount;
    }
};


template<typename ObjectType>
class DSharedObjectField : public DField
{
private:
    DSharedPtr<HObject> ObjectType::* Member;

public:
    DSharedObjectField(DSharedPtr<HObject> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType) :
        Member(InMember),
        DField(EArchType::SHARED_OBJECT) 
    {
    }

    DSharedPtr<HObject>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType>
class DWeakObjectField : public DField
{
private:
    DWeakPtr<HObject> ObjectType::* Member;

public:
    DWeakObjectField(DWeakPtr<HObject> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType) :
        Member(InMember),
        DField(EArchType::WEAK_OBJECT)
    {
    }

    DWeakPtr<HObject>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }
};


template<typename ObjectType, typename T, class AllocatorT = DArrayAllocator<T>>
class DArrayField : public DField
{
private:
    DArray<T, AllocatorT> ObjectType::* Member;
    cint ItemByteCount;
    uint8 ItemEnumLength;

public:
    DArrayField(DArray<T, AllocatorT> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        ItemByteCount(InItemByteCount), 
        DField(EArchType::ARRAY, InItemType) 
    {
        ItemEnumLength = 0;
        if constexpr (std::is_enum<T>::value)
        {
            static_assert(
                std::is_same<std::underlying_type<T>::type, uint8>::value,
                "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
                );
            ItemEnumLength = (uint8)T::__Length;
        }
        else if constexpr (std::is_base_of<FFloatStruct, T>::value)
        {
            static_assert(
                std::is_default_constructible<T>::value && std::is_polymorphic<T>::value == false,
                "Reflected structs must be default constructible and must not be polymorphic (virtual methods are not allowed)"
                );
        }
    }

    DArray<T, AllocatorT>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual const char* GetTypeName() const
    {
        return typeid(T).name();
    }

    virtual void Resize(const ObjectType* Obj, cint NewLength) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Resize(NewLength);
    }

    virtual void ClearAndReserve(const ObjectType* Obj, cint NewCapacity) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Reserve(NewCapacity);
    }

    virtual T* AddDefault(const ObjectType* Obj) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault());
    }

    virtual T* Data(const ObjectType* Obj) const
    {
        return ((*const_cast<ObjectType*>(Obj)).*(this->Member)).GetDataUnsafe();
    }

    virtual cint Length(const ObjectType* Obj) const
    {
        return ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Length();
    }

    virtual void OverwriteLength(const ObjectType* Obj, cint NewLength) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).OverwriteLengthUnsafe(NewLength);
    }

    cint GetItemByteCount() const
    {
        return this->ItemByteCount;
    }

    uint8 GetItemEnumLength() const
    {
        return this->ItemEnumLength;
    }
};


template<typename ObjectType, typename T, class AllocatorT = DArrayAllocator<T>>
class DStructArrayField : public DField
{
private:
    DArray<T, AllocatorT> ObjectType::* Member;
    cint ItemByteCount;

public:
    DStructArrayField(DArray<T, AllocatorT> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember), 
        ItemByteCount(InItemByteCount),
        DField(EArchType::ARRAY, EItemType::STRUCT) 
    {
    }

    DArray<T, AllocatorT>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual void Resize(const ObjectType* Obj, cint NewLength) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Resize(NewLength);
    }

    virtual void ClearAndReserve(const ObjectType* Obj, cint NewCapacity) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Reserve(NewCapacity);
    }

    virtual T* AddDefault(const ObjectType* Obj) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault());
    }

    virtual T* Data(const ObjectType* Obj) const
    {
        return ((*const_cast<ObjectType*>(Obj)).*(this->Member)).GetDataUnsafe();
    }

    virtual cint Length(const ObjectType* Obj) const
    {
        return ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Length();
    }

    virtual void OverwriteLength(const ObjectType* Obj, cint NewLength) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).OverwriteLengthUnsafe(NewLength);
    }

    virtual DClass& GetStructClass() const
    {
        return T::GetDeclaredClass();
    }

    cint GetItemByteCount() const
    {
        return this->ItemByteCount;
    }
};


// ItemType is ignored, KeyType is used
template<typename ObjectType, typename T, class AllocatorT = DNodeArrayAllocator<DMapEntry<T, void*>>>
class DSetField : public DField
{
private:
    DSet<T, AllocatorT> ObjectType::* Member;
    uint8 ItemEnumLength;

public:
    DSetField(DSet<T, AllocatorT> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        DField(EArchType::SET, InKeyType) 
    {
        if constexpr (std::is_enum<T>::value)
        {
            static_assert(
                std::is_same<std::underlying_type<T>::type, uint8>::value,
                "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
                );
            ItemEnumLength = (uint8)T::__Length;
        }
        else
        {
            ItemEnumLength = 0;
        }
    }

    DSet<T, AllocatorT>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual void Clear(const ObjectType* Obj) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
    }

    virtual void ClearAndReserve(const ObjectType* Obj, cint NewCapacity) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Reserve(NewCapacity);
    }

    virtual void Add(const ObjectType* Obj, T&& Key) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Add(std::move(Key));
    }

    virtual void Add(const ObjectType* Obj, const T& Key) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Add(Key);
    }

    virtual const char* GetTypeName() const
    {
        return typeid(T).name();
    }

    uint8 GetItemEnumLength() const
    {
        return this->ItemEnumLength;
    }
};


template<typename ObjectType, typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
class DMapField : public DField
{
private:
    DMap<K, V, AllocatorT> ObjectType::* Member;
    uint8 KeyEnumLength;
    uint8 ItemEnumLength;
    cint ItemByteCount;

public:
    DMapField(DMap<K, V, AllocatorT> ObjectType::* InMember, EItemType InValueType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember),
        ItemByteCount(InItemByteCount),
        DField(EArchType::MAP, InValueType, InKeyType)
    {
        KeyEnumLength = 0;
        ItemEnumLength = 0;
        if constexpr (std::is_enum<K>::value)
        {
            static_assert(
                std::is_same<std::underlying_type<K>::type, uint8>::value,
                "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
                );
            KeyEnumLength = (uint8)K::__Length;
        }
        if constexpr (std::is_enum<V>::value)
        {
            static_assert(
                std::is_same<std::underlying_type<V>::type, uint8>::value,
                "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
                );
            ItemEnumLength = (uint8)V::__Length;
        }
        else if constexpr (std::is_base_of<FFloatStruct, V>::value)
        {
            static_assert(
                std::is_default_constructible<V>::value && std::is_polymorphic<V>::value == false,
                "Reflected structs must be default constructible and must not be polymorphic (virtual methods are not allowed)"
                );
        }
    }

    DMap<K, V, AllocatorT>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual void Clear(const ObjectType* Obj) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
    }

    virtual void ClearAndReserve(const ObjectType* Obj, cint NewCapacity) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Reserve(NewCapacity);
    }

    virtual V* AddDefault(const ObjectType* Obj, K&& Key) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault(std::move(Key)));
    }

    virtual V* AddDefault(const ObjectType* Obj, const K& Key) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault(Key));
    }

    virtual const char* GetKeyTypeName() const
    {
        return typeid(K).name();
    }

    virtual const char* GetItemTypeName() const
    {
        return typeid(V).name();
    }

    uint8 GetKeyEnumLength() const
    {
        return this->KeyEnumLength;
    }

    uint8 GetItemEnumLength() const
    {
        return this->ItemEnumLength;
    }

    cint GetItemByteCount() const
    {
        return this->ItemByteCount;
    }
};


template<typename ObjectType, typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
class DStructMapField : public DField
{
private:
    DMap<K, V, AllocatorT> ObjectType::* Member;
    cint ItemByteCount;
    uint8 KeyEnumLength;

public:
    DStructMapField(DMap<K, V, AllocatorT> ObjectType::* InMember, EItemType InItemType, EKeyType InKeyType, cint InItemByteCount) :
        Member(InMember), 
        ItemByteCount(InItemByteCount), 
        DField(EArchType::MAP, EItemType::STRUCT, InKeyType)
    {
        KeyEnumLength = 0;
        if constexpr (std::is_enum<K>::value)
        {
            static_assert(
                std::is_same<std::underlying_type<K>::type, uint8>::value,
                "Reflected enums must have uint8 as underlying type and have '__Length' as last item"
                );
            KeyEnumLength = (uint8)K::__Length;
        }
    }

    DMap<K, V, AllocatorT>* GetPtr(const ObjectType* Obj) const
    {
        return &((*const_cast<ObjectType*>(Obj)).*(this->Member));
    }

    virtual void Clear(const ObjectType* Obj) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
    }

    virtual void ClearAndReserve(const ObjectType* Obj, cint NewCapacity) const
    {
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Clear();
        ((*const_cast<ObjectType*>(Obj)).*(this->Member)).Reserve(NewCapacity);
    }
    
    virtual V* AddDefault(const ObjectType* Obj, K&& Key) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault(std::move(Key)));
    }
    
    virtual V* AddDefault(const ObjectType* Obj, const K& Key) const
    {
        return &(((*const_cast<ObjectType*>(Obj)).*(this->Member)).AddDefault(Key));
    }
    
    virtual DClass& GetItemStructClass() const
    {
        return V::GetDeclaredClass();
    }
    
    virtual const char* GetKeyTypeName() const
    {
        return typeid(K).name();
    }

    cint GetItemByteCount() const
    {
        return this->ItemByteCount;
    }

    uint8 GetKeyEnumLength() const
    {
        return this->KeyEnumLength;
    }
};
