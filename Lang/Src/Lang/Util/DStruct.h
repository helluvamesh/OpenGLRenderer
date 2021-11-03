#pragma once


/*__________________________________________________________________________________

    Base struct for structs with reflection

    STRUCTS MUST NOT BE POLYMORPHIC TO KEEP THEM CONTIGUOUS IN MEMORY
    SO NO VIRTUAL METHODS!
    - inheriting to compose structs won't break this rule
    - static_cast/reinterpret_cast/const_cast pointers works
    - dynamic_cast would only work with a polymorphic struct

    Reflected structs must be non-polymorphic

    Reflected structs must be default constructible, 
    that is they must have a parameterless constructor or no explicitly defined constructor
    
    If a type needs both reflection and polymorphism, inherit from 'HObject' instead
__________________________________________________________________________________*/


struct DStruct
{
protected:
    typedef DStruct BaseObjectType;
    static bool StaticInit;
    static DClass ClassData;

public:
    static DClass& GetDeclaredClass();
    DClass& GetClass() const;

};
