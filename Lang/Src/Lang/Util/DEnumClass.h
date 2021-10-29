#pragma once


enum class UINT8_ENUM : uint8 { __Length };


class DEnumClass
{
private:
    static DMap<const char*, DEnumClass> EnumClasses;
    static const DString8 UNKNOWN_ENUM_VALUE;
        
public:
    // ID = typeid(T).name()!!! - it won't work from any other string
    static DEnumClass* FindEnumClass(const char* ID);
    static const DString8& GetUnknownEnumName();

private:
    const char8* ItemString;
    DArray<DString8> ItemNames;
    uint8 Length;

public:
    DEnumClass(const char* ID, const char8* InItemString, uint8 InLength);

    uint8 GetLength();
    const DArray<DString8>& GetItemNames();
    const DString8& GetItemName(uint8 ItemValue);
    uint8 GetItemByName(const DString8& ItemName);
};


namespace SEnumClasses { }


#define DECLARE_ENUM(__EnumName, ...) \
    enum class __EnumName : uint8 { __VA_ARGS__, __Length }; \
    namespace SEnumClasses \
    { \
        const DEnumClass __EnumName (typeid(__EnumName).name(), u8#__VA_ARGS__, (uint8)__EnumName::__Length); \
    }


#define DECLARE_ENUM_API(__API, __EnumName, ...) \
    enum class __API __EnumName : uint8 { __VA_ARGS__, __Length }; \
    namespace SEnumClasses \
    { \
        const DEnumClass __EnumName (typeid(__EnumName).name(), u8#__VA_ARGS__, (uint8)__EnumName::__Length); \
    }
