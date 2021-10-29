#pragma once


DECLARE_ENUM(
    EArchType,
    BOOL, ENUM, INT, INT64, FLOAT, DOUBLE, 
    STRING, STRING8, 
    FLOAT_STRUCT, 
    STRUCT, SHARED_OBJECT, WEAK_OBJECT,
    ARRAY, SET, MAP,
    BYTE, SBYTE, USHORT, SHORT, UINT, UINT64
)

DECLARE_ENUM(
    EItemType,
    BOOL, ENUM, INT, INT64, FLOAT, DOUBLE, 
    STRING, STRING8, 
    FLOAT_STRUCT,
    STRUCT, SHARED_OBJECT, WEAK_OBJECT,
    BYTE, SBYTE, USHORT, SHORT, UINT, UINT64
)

DECLARE_ENUM(
    EKeyType,
    ENUM, INT, INT64, 
    STRING, STRING8,
    SHARED_OBJECT
)



class DField
{
public:
    const EArchType ArchType;
    const EItemType ItemType;
    const EKeyType KeyType;

    DField(EArchType InArchType);
    DField(EArchType InArchType, EItemType InItemType);
    DField(EArchType InArchType, EKeyType InKeyType);
    DField(EArchType InArchType, EItemType InItemType, EKeyType InKeyType);

    // make class polymorphic
    virtual ~DField() = default;
};


class DClass
{
public:
    class FieldEnumerator : public IConstEnumerator<DMapEntry<DString8, DField*>>
    {
    private:
        const DClass* Cls;
        DArrayConstEnumerator<DMapEntry<DString8, DField*>*> Etor;

    public:
        FieldEnumerator(const DClass* InCls);
        const DMapEntry<DString8, DField*>* Next() override;
    };


private:
    DClass* SuperClass;
    DString8 ClassName;
    DMap<DString8, DField*> Fields;
    DArray<DMapEntry<DString8, DField*>*> FieldArray;

public:
    DClass(const DString8& InClassName, DClass* InSuperClass);
    
    void AddField(const DString8& InName, DField* InField);
    DClass* Super();
    const DClass* Super() const;
    const DString8& GetClassName() const;
    const DField* FindField(const DString8& Name) const;
    FieldEnumerator GetFieldEnumerator() const;
    cint GetFieldCount() const;
};


using DFieldEnumerator = DClass::FieldEnumerator;


#define REFLECT_H(__ClassName, __SuperClassName) \
protected: \
    static bool StaticInit; \
    static DClass ClassData; \
public: \
    static DClass& GetDeclaredClass(); \
    DClass& GetClass() const; \
private: \
    typedef __ClassName cls; \
    typedef __SuperClassName super;


#define REFLECT(__ClassName) \
static_assert( \
    std::is_base_of<DStruct, __ClassName>::value == false || \
    (std::is_default_constructible<__ClassName>::value && std::is_polymorphic<__ClassName>::value == false), \
    "Reflected structs must be default constructible and must not be polymorphic (virtual methods are not allowed)" \
); \
DClass __ClassName::ClassData(u8#__ClassName, &__ClassName::super::ClassData); \
DClass& __ClassName::GetDeclaredClass() \
{ \
    return __ClassName::ClassData; \
} \
DClass& __ClassName::GetClass() const \
{ \
    return __ClassName::ClassData; \
} \
bool __ClassName::StaticInit = []() { 
    

#define REFLECT_END() \
    return true; \
}();
