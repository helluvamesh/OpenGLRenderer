#pragma once


template<typename T>
struct DArrayView
{
private:
	const T* Array;
	cint Size;

public:
	DArrayView(const T* InArray, cint InSize) : 
		Array(InArray), 
		Size(InSize) 
	{
	}
	DArrayView() : 
		Array(nullptr), 
		Size(0) 
	{
	}

	bool IsValid() const
	{
		return this->Array && this->Size > 0;
	}

	const T* Data() const
	{
		return this->Array;
	}

	FORCEINLINE const cint Length() const
	{
		return this->Size;
	}

	const T& operator[](cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (!this->Array || Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}

};


template<>
struct DArrayView<tchar>
{
private:
	const tchar* Array;
	cint Size;

public:
	DArrayView(const tchar* InArray, cint InSize) : 
		Array(InArray), 
		Size(InSize) 
	{
	}

	DArrayView(const tchar* CString)
	{
		this->Array = CString;
		this->Size = SCounterUtil::CStringLength(CString);
	}

	DArrayView() : 
		Array(nullptr), 
		Size(0) 
	{
	}

	bool IsValid() const
	{
		return this->Array && this->Size > 0;
	}

	const tchar* Data() const
	{
		return this->Array;
	}

	FORCEINLINE const cint Length() const
	{
		return this->Size;
	}

	const tchar& operator[](cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (!this->Array || Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}

};


template<>
struct DArrayView<char8>
{
private:
	const char8* Array;
	cint Size;

public:
	DArrayView(const char8* InArray, cint InSize) : 
		Array(InArray), 
		Size(InSize) 
	{
	}

	DArrayView(const char8* CString)
	{
		this->Array = CString;
		this->Size = SCounterUtil::CStringLength(CString);
	}

	DArrayView() : 
		Array(nullptr), 
		Size(0) 
	{
	}

	bool IsValid() const
	{
		return this->Array && this->Size > 0;
	}

	const char8* Data() const
	{
		return this->Array;
	}

	FORCEINLINE const cint Length() const
	{
		return this->Size;
	}

	const char8& operator[](cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (!this->Array || Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}

};


using DStringView = DArrayView<tchar>;
using DStringView8 = DArrayView<char8>;
