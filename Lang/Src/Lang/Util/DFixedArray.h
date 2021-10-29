/*_______________________________________________

	DFixedArray
	- stack allocated fixed-sized array
_______________________________________________*/

#pragma once


template<typename T, cint L>
class DFixedArray
{
private:

	T Array[L];

public:

	DFixedArray()
	{
		for (cint i = 0; i < L; i++)
		{
			new (this->Array + i) T();
		}
	}

	explicit DFixedArray(const T& Value)
	{
		for (cint i = 0; i < L; i++)
		{
			this->Array[i] = Value;
		}
	}

	DFixedArray(const std::initializer_list<T>& InitList)
	{
		cint overlap = SCounterUtil::Min(L, (cint)InitList.size());
		auto start = InitList.begin();
		for (cint i = 0; i < overlap; i++)
		{
			new (this->Array + i) T(*(start + i));
		}
		for (cint i = overlap; i < L; i++)
		{
			new (this->Array + i) T();
		}
	}


	void Reset()
	{
		for (cint i = 0; i < L; i++)
		{
			this->Array[i] = T();
		}
	}


	void Fill(const T& Value)
	{
		for (cint i = 0; i < L; i++)
		{
			this->Array[i] = Value;
		}
	}


	T& operator[](cint Index)
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= L || Index < 0)
		{
			throw DIndexException(Index, L);
		}
#endif
		return this->Array[Index];
	}


	const T& operator[](cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= L || Index < 0)
		{
			throw DIndexException(Index, L);
		}
#endif
		return this->Array[Index];
	}


	T& At(cint Index)
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= L || Index < 0)
		{
			throw DIndexException(Index, L);
		}
#endif
		return this->Array[Index];
	}


	const T& At(cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= L || Index < 0)
		{
			throw DIndexException(Index, L);
		}
#endif
		return this->Array[Index];
	}


	void Sort()
	{
		std::sort(this->Array, this->Array + L);
	}


	/* 
	Pred: (T& A, T& B) -> bool
	return value: is B greater than A? 
	*/
	template<CALLABLE(PredicateT, bool(T&, T&))>
	void Sort(PredicateT&& Pred)
	{
		std::sort(this->Array, this->Array + L, Pred);
	}


	FORCEINLINE bool IsValidIndex(cint Index) const
	{
		return Index >= 0 && Index < L;
	}


	FORCEINLINE cint Length() const
	{
		return L;
	}


	FORCEINLINE const T* Data() const
	{
		return this->Array;
	}

	
	FORCEINLINE T* GetDataUnsafe()
	{
		return this->Array;
	}

	
	T* begin() { return this->Array; }
	const T* begin() const { return this->Array; }

	T* end() { return this->Array + L; }
	const T* end() const { return this->Array + L; }


	operator DArrayView<T>() const
	{
		return DArrayView<T>(this->Array, L);
	}


	DArrayView<T> ViewRange(cint Start, cint Stop) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (SCounterUtil::RangeError(Start, Stop, L))
		{
			throw DIndexException(Start, Stop, L);
		}
#endif
		return DArrayView<T>(this->Array + Start, Stop - Start);
	}

};