#pragma once

// INTEGERS

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// CINT - counter integer

typedef int32_t cint;  // counter integer to store container length and indices
#define CINT_MAX INT32_MAX
#define CINT_SIZEOF(__X) static_cast<cint>(sizeof(__X))

// CHARS

typedef char16_t tchar;  // text char
typedef char char8;   // 8-bit text char
#define TX(str) u##str
#define TX8(str) u8##str

// BYTE

typedef uint8_t byte;

// structs with only float fields
// FloatStructs and Arrays of FloatStructs are serialized as a single flat float array for high speed
struct FFloatStruct { };


#define FORCEINLINE __forceinline


namespace STyping
{
	// SUBCLASS

	template<class _SubClass, class _BaseClass, bool = std::is_convertible<_SubClass*, _BaseClass*>::value>
	struct _SubClassOf
	{
		using type = _SubClass;
	};

	template<class _SubClass, class _BaseClass>
	struct _SubClassOf<_SubClass, _BaseClass, false>
	{
	};

	template<class _SubClass, class _BaseClass>
	using SubClassOf = typename _SubClassOf<_SubClass, _BaseClass>::type;


	// CALLABLE

	template<class _CallableT, class _Signature, bool = std::is_convertible<_CallableT&&, std::function<_Signature>>::value>
	struct _Callable
	{
		using type = _Callable;
	};

	template<class _CallableT, class _Signature>
	struct _Callable<_CallableT, _Signature, false>
	{
	};

	template<class _CallableT, class _Signature>
	using Callable = typename _Callable<_CallableT, _Signature>::type;


	// ENUM_TYPE

	template<class _EnumT, bool = std::is_enum<_EnumT>::value>
	struct _EnumType
	{
		using type = _EnumT;
	};

	template<class _EnumT>
	struct _EnumType<_EnumT, false>
	{
	};

	template<class _EnumT>
	using EnumType = typename _EnumType<_EnumT>::type;

}


// __SubClass is already declared outside
#define EXTENDS(__SubClass, __BaseClass) class = STyping::SubClassOf<__SubClass, __BaseClass>
// __SubClass is declared inside
#define SUBTYPE(__SubClass, __BaseClass) class __SubClass, class = STyping::SubClassOf<__SubClass, __BaseClass>
// __BaseClass is already declared outside
#define SUPER(__BaseClass, __SubClass) class = STyping::SubClassOf<__SubClass, __BaseClass>
// __BaseClass is declared inside
#define BASETYPE(__BaseClass, __SubClass) class, __BaseClass, class = STyping::SubClassOf<__SubClass, __BaseClass>

#define CALLABLE(__CallableT, __Signature) typename __CallableT, typename = STyping::Callable<__CallableT, __Signature>

#define ENUM_TYPE(__EnumT) typename __EnumT, typename = STyping::EnumType<__EnumT>
#define IS_ENUM_TYPE(__EnumT) typename = STyping::EnumType<__EnumT>

// ..._RD:: declarations with no default value assigned
// to be used in redeclarations, like friend statements, to avoid a compiler warning

// __SubClass is already declared outside
#define EXTENDS_RD(__SubClass, __BaseClass) class
// __SubClass is declared inside
#define SUBTYPE_RD(__SubClass, __BaseClass) class __SubClass, class
// __BaseClass is already declared outside
#define SUPER_RD(__BaseClass, __SubClass) class
// __BaseClass is declared inside
#define BASETYPE_RD(__BaseClass, __SubClass) class, __BaseClass, class

#define CALLABLE_RD(__CallableT, __Signature) typename __CallableT, typename 

#define ENUM_TYPE_RD(__EnumT) typename __EnumT, typename
#define IS_ENUM_TYPE_RD(__EnumT) typename
