/*______________________________________________________________

DArray
- implements a dynamic (resizable) array
- items are laid out contiguously in memory
- can reserve memory for future items to avoid constant resizing and moving
- can construct objects directly inside the storage array (emplace)
- supports bracket-indexing syntax
- supports enhanced for loop (foreach)
- supports initializer list
- supports move semantics

- Iterators are implemeted only as much so the container can be used in enhanced for loops (foreach),
- otherwise use enumerators, see Enumerators.h.

______________________________________________________________*/


#pragma once


template<typename T, class AllocatorT = DArrayAllocator<T>>
class DArray
{
	/*___________________________

	ITERATORS
	___________________________*/

public:

#ifdef LANG_DEBUG_CONTAINERS

	template<typename IT>
	class Iterator
	{
	private:

		DArray<IT, AllocatorT>* Array;
		cint Index;
		cint StoredModCount;

	public:

		Iterator(DArray<IT, AllocatorT>& InArray, cint InIndex) : 
			Array(&InArray), 
			Index(InIndex), 
			StoredModCount(InArray.ModCount)
		{
		}

		bool operator==(const Iterator<IT>& Other) const
		{
			return this->Index == Other.Index;
		}
		bool operator!=(const Iterator<IT>& Other) const
		{
			return this->Index != Other.Index;
		}

		IT& operator*() const
		{
			return *(this->Array->Array + this->Index);
		}

		IT* operator->() const
		{
			return this->Array->Array + this->Index;
		}

		Iterator<IT>& operator++()
		{
			if (this->Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
			this->Index += 1;
			return *this;
		}

		Iterator<IT>& operator--() = delete;

		Iterator<IT> operator++(cint) = delete;
		Iterator<IT> operator--(cint) = delete;
	};


	template<typename IT>
	class ConstIterator
	{
	private:

		const DArray<IT, AllocatorT>* Array;
		cint Index;
		cint StoredModCount;

	public:

		ConstIterator(const DArray<IT, AllocatorT>& InArray, cint InIndex) : 
			Array(&InArray), 
			Index(InIndex), 
			StoredModCount(InArray.ModCount)
		{
		}

		bool operator==(const ConstIterator<IT>& Other) const
		{
			return this->Index == Other.Index;
		}
		bool operator!=(const ConstIterator<IT>& Other) const
		{
			return this->Index != Other.Index;
		}

		const IT& operator*() const
		{
			return *(this->Array->Array + this->Index);
		}

		const IT* operator->() const
		{
			return this->Array->Array + this->Index;
		}

		ConstIterator<IT>& operator++()
		{
			if (this->Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
			this->Index += 1;
			return *this;
		}

		ConstIterator<IT>& operator--() = delete;

		ConstIterator<IT> operator++(cint) = delete;
		ConstIterator<IT> operator--(cint) = delete;
	};

#else
	
	template<typename IT>
	using Iterator = IT*;

	template<typename IT>
	using ConstIterator = const IT*;

#endif

	template<typename IT>
	class Enumerator : public IEnumerator<IT>
	{
	private:
		DArray<IT, AllocatorT>* _Array;
		cint _Index;
		cint _Stop;
		cint StoredModCount;

	public:

		explicit Enumerator(DArray<IT, AllocatorT>& Array) :
			_Array(&Array), 
			_Index(-1), 
			_Stop(Array.Size),
			StoredModCount(Array.ModCount)
		{
		}

		Enumerator(DArray<IT, AllocatorT>& Array, cint Start, cint Stop) :
			_Array(&Array), 
			_Index(Start - 1),
			_Stop(Stop)
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (SCounterUtil::RangeError(Start, Stop, Array.Size))
			{
				throw DIndexException(Start, Stop, Array.Size);
			}
			this->StoredModCount = Array.ModCount;
#endif
		}

		IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->_Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (++this->_Index < this->_Stop)
			{
				return this->_Array->Array + this->_Index;
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IT>
	class ConstEnumerator : public IConstEnumerator<IT>
	{
	private:
		const DArray<IT, AllocatorT>* _Array;
		cint _Index;
		cint _Stop;
		cint StoredModCount;

	public:

		explicit ConstEnumerator(const DArray<IT, AllocatorT>& Array) :
			_Array(&Array),
			_Index(-1), 
			_Stop(Array.Size), 
			StoredModCount(Array.ModCount)
		{
		}

		ConstEnumerator(const DArray<IT, AllocatorT>& Array, cint Start, cint Stop) :
			_Array(&Array), _Index(Start - 1), _Stop(Stop)
		{
#ifdef LANG_DEBUG_CONTAINERS
			this->StoredModCount = Array.ModCount;
			if (SCounterUtil::RangeError(Start, Stop, Array.Size))
			{
				throw DIndexException(Start, Stop, Array.Size);
			}
#endif
		}

		const IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->_Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (++this->_Index < this->_Stop)
			{
				return this->_Array->Array + this->_Index;
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IT>
	class ReverseEnumerator : public IEnumerator<IT>
	{
	private:
		DArray<IT, AllocatorT>* _Array;
		cint _Index;
		cint _Stop;
		cint StoredModCount;

	public:

		explicit ReverseEnumerator(DArray<IT, AllocatorT>& Array) :
			_Array(&Array),
			_Index(Array.Size),
			_Stop(0),
			StoredModCount(Array.ModCount)
		{
		}

		ReverseEnumerator(DArray<IT, AllocatorT>& Array, cint Start, cint Stop) :
			_Array(&Array),
			_Index(Stop),
			_Stop(Start)
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (SCounterUtil::RangeError(Start, Stop, Array.Size))
			{
				throw DIndexException(Start, Stop, Array.Size);
			}
			this->StoredModCount = Array.ModCount;
#endif
		}

		IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->_Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (--this->_Index >= _Stop)
			{
				return this->_Array->Array + this->_Index;
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IT>
	class ConstReverseEnumerator : public IConstEnumerator<IT>
	{
	private:
		const DArray<IT, AllocatorT>* _Array;
		cint _Index;
		cint _Stop;
		cint StoredModCount;

	public:

		explicit ConstReverseEnumerator(const DArray<IT, AllocatorT>& Array) :
			_Array(&Array),
			_Index(Array.Size),
			_Stop(0), 
			StoredModCount(Array.ModCount)
		{
		}

		ConstReverseEnumerator(const DArray<IT, AllocatorT>& Array, cint Start, cint Stop) :
			_Array(&Array), 
			_Index(Stop),
			_Stop(Start)
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (SCounterUtil::RangeError(Start, Stop, Array.Size))
			{
				throw DIndexException(Start, Stop, Array.Size);
			}
			this->StoredModCount = Array.ModCount;
#endif
		}

		const IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->_Array->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (--this->_Index >= _Stop)
			{
				return this->_Array->Array + this->_Index;
			}
			else
			{
				return nullptr;
			}
		}
	};

	/*___________________________

	STATE
	___________________________*/

public:
	
	// the first buffer size after Capacity = 0
	// this is an optimization to avoid having to reallocate many times when size is low
	static constexpr const cint INITIAL_CAPACITY = 16;

private:
	
	cint ArrayCapacity;
	cint Size;
	cint ModCount = 0;
	T* Array;

	FORCEINLINE static void MoveToUninitialized(T* SourceArray, cint SourceStartIndex, cint SourceStopIndex, T* DestArrayStart)
	{
		cint len = SourceStopIndex - SourceStartIndex;
		for (cint i = 0; i < len; i++)
		{
			new (DestArrayStart + i) T(std::move(SourceArray[SourceStartIndex + i]));
		}
	}

public:

	/*___________________________

	CONSTRUCT
	___________________________*/

	DArray() : 
		Array(nullptr), 
		ArrayCapacity(0),
		Size(0)
	{ 
	}

	// OverwriteLength = true results in an uninitialized array - Unsafe
	explicit DArray(cint InCapacity, bool OverwriteLength = false)
	{
		if (InCapacity > 0)
		{
			this->Size = OverwriteLength? InCapacity : 0;
			this->Array = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = InCapacity);
		}
		else
		{
			this->Size = this->ArrayCapacity = 0;
			this->Array = nullptr;
		}
	}

	DArray(const std::initializer_list<T>& InitList)
	{
		this->Size = (cint)InitList.size();
		this->Array = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = SCounterUtil::Max(this->Size, INITIAL_CAPACITY));
		auto start = InitList.begin();
		for (cint i = 0; i < this->Size; i++)
		{
			new (this->Array + i) T(*(start + i));
		}
	}

	// Take over a buffer that is COMPATIBLE WITH this DArray's ALLOCATOR!!!
	static DArray TakeOverBufferUnsafe(T* BufferToTakeOver, cint BufferLength, cint BufferCapacity)
	{
		DArray<T, AllocatorT> arr;
		arr.Array = BufferToTakeOver;
		arr.Size = BufferLength;
		arr.ArrayCapacity = BufferCapacity;
		return arr;
	}

	/*___________________________

	COPY
	___________________________*/

	DArray(const DArray& Other)
	{
		this->Array = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = Other.ArrayCapacity);
		this->Size = Other.Size;
		for (cint i = 0; i < this->Size; i++)
		{
			new (this->Array + i) T(Other.Array[i]);
		}
	}

	DArray& operator=(const DArray& Other)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->_Reserve(Other.ArrayCapacity);
		this->Size = Other.Size;
		for (cint i = 0; i < this->Size; i++)
		{
			new (this->Array + i) T(Other.Array[i]);
		}
		return *this;
	}

	/*___________________________

	MOVE
	___________________________*/

	DArray(DArray&& Other) noexcept
	{
		this->Array = Other.Array;
		Other.Array = nullptr;
		this->ArrayCapacity = Other.ArrayCapacity;
		this->Size = Other.Size;
		Other.ArrayCapacity = Other.Size = 0;
	}

	DArray& operator=(DArray&& Other) noexcept
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->DeleteBuffer();
		this->Array = Other.Array;
		Other.Array = nullptr;
		this->ArrayCapacity = Other.ArrayCapacity;
		this->Size = Other.Size;
		Other.ArrayCapacity = Other.Size = 0;
		return *this;
	}
	
	/*___________________________

	DESTRUCT
	___________________________*/

private:

	FORCEINLINE void DeleteBuffer()
	{
		if (this->Array != nullptr)
		{
			for (cint i = 0; i < this->Size; ++i)
			{
				this->Array[i].~T();
			}
			AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			this->Array = nullptr;
		}
	}

public:

	~DArray()
	{
		this->DeleteBuffer();
	}

	/*___________________________

	ARRAY MANIPULATION
	___________________________*/

	void Add(const T& NewItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		cint newSize = this->Size + 1;
		if (newSize > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		new (this->Array + this->Size) T(NewItem);
		this->Size = newSize;
	}


	void Add(T&& NewItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		cint newSize = this->Size + 1;
		if (newSize > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		new (this->Array + this->Size) T(std::move(NewItem));
		this->Size = newSize;
	}


	// NewItem is added only if it is not already in the array
	// returns true, if NewItem was added
	bool AddUnique(const T& NewItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif

		for (cint i = 0; i < this->Size; i++)
		{
			if (this->Array[i] == NewItem)
			{
				return false;
			}
		}

		cint newSize = this->Size + 1;
		if (newSize > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		new (this->Array + this->Size) T(NewItem);
		this->Size = newSize;
		return true;
	}


	// NewItem is added only if it is not already in the array
	// returns true, if NewItem was added
	bool AddUnique(T&& NewItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif

		for (cint i = 0; i < this->Size; i++)
		{
			if (this->Array[i] == NewItem)
			{
				return false;
			}
		}

		cint newSize = this->Size + 1;
		if (newSize > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		new (this->Array + this->Size) T(std::move(NewItem));
		this->Size = newSize;
		return true;
	}


	/* 
	If items are objects (not pointers to object),
	this will fail, if the object has no parameterless constructor
	*/
	T& AddDefault()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->Size + 1 > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		return *(new (this->Array + this->Size++) T());
	}


	template<typename... ConstructorArgs>
	T& Emplace(ConstructorArgs&&... Args)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->Size + 1 > this->ArrayCapacity)
		{
			T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
			if (this->Size > 0)
			{
				MoveToUninitialized(this->Array, 0, this->Size, newArray);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
			}
			this->Array = newArray;
		}
		return *(new (this->Array + this->Size++) T(std::move(Args)...));
	}


	void Insert(cint Index, const T& NewItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (Index > this->Size || Index < 0)
		{
			throw DIndexException(Index, 0, this->Size + 1, false);
		}
#endif
		if (Index < this->Size)
		{
			cint newSize = this->Size + 1;
			if (newSize > this->ArrayCapacity)
			{
				T* newArray = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = this->ArrayCapacity > 0 ? this->ArrayCapacity * 2 : INITIAL_CAPACITY);
				MoveToUninitialized(this->Array, 0, Index, newArray);
				new (newArray + Index) T(NewItem);
				MoveToUninitialized(this->Array, Index, this->Size, newArray + Index + 1);
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
				this->Array = newArray;
				this->Size = newSize;
			}
			else
			{
				for (cint i = this->Size - 1; i >= Index; i--)
				{
					this->Array[i + 1] = std::move(this->Array[i]);
				}
				this->Array[Index] = NewItem;
				this->Size = newSize;
			}
		}
		else if (Index == this->Size)
		{
			this->Add(NewItem);
		}
	}


	void Remove(cint Index)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		this->Array[Index].~T();
		for (cint i = Index + 1; i < this->Size; i++)
		{
			this->Array[i - 1] = std::move(this->Array[i]);
		}
		this->Size--;
	}


	// returns true, if the item was found and removed;
	// returns false, if nothing was removed
	bool RemoveItem(const T& Item)
	{
		for (cint i = 0; i < this->Size; i++)
		{
			if (this->Array[i] == Item)
			{
				this->Array[i].~T();
				for (cint j = i + 1; j < this->Size; j++)
				{
					this->Array[j - 1] = std::move(this->Array[j]);
				}
				this->Size--;
				return true;
			}
		}
		return false;
	}


	void RemoveLast()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (this->Size <= 0)
		{
			throw DIndexException(this->Size - 1, this->Size);
		}
#endif
		this->Array[--this->Size].~T();
	}


	void RemoveRange(cint Start, cint Stop)
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (SCounterUtil::RangeError(Start, Stop, this->Size))
		{
			throw DIndexException(Start, Stop, this->Size);
		}
#endif
		cint removedItems = Stop - Start;
		for (cint i = Start; i < Stop; ++i)
		{
			this->Array[i].~T();
		}
		for (cint i = Stop; i < this->Size; ++i)
		{
			this->Array[i - removedItems] = std::move(this->Array[i]);
		}
		this->Size -= removedItems;
	}


	void RemoveGet(cint Index, T& OutItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		OutItem = this->Array[Index];
		this->Array[Index].~T();
		for (cint i = Index + 1; i < this->Size; i++)
		{
			this->Array[i - 1] = std::move(this->Array[i]);
		}
		this->Size--;
	}


	void RemoveGetLast(T& OutItem)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (this->Size <= 0)
		{
			throw DIndexException(this->Size - 1, this->Size);
		}
#endif
		OutItem = this->Array[--this->Size];
		this->Array[this->Size].~T();
	}


	T& operator[](cint Index)
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}


	const T& operator[](cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}


	T& At(cint Index)
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}


	const T& At(cint Index) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (Index >= this->Size || Index < 0)
		{
			throw DIndexException(Index, this->Size);
		}
#endif
		return this->Array[Index];
	}


	T& AtLast()
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size < 1)
		{
			throw DIndexException(this->Size - 1, this->Size);
		}
#endif
		return this->Array[this->Size - 1];
	}


	const T& AtLast() const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size < 1)
		{
			throw DIndexException(this->Size - 1, this->Size);
		}
#endif
		return this->Array[this->Size - 1];
	}


	FORCEINLINE bool IsValidIndex(cint Index) const
	{
		return Index >= 0 && Index < this->Size;
	}


	void Clear()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		for (cint i = 0; i < this->Size; ++i)
		{
			this->Array[i].~T();
		}
		this->Size = 0;
	}


	cint IndexOf(const T& Item, cint Start = 0) const
	{
		if (Start >= 0 && Start < this->Size)
		{
			for (cint i = Start; i < this->Size; i++)
			{
				if (this->Array[i] == Item)
				{
					return i;
				}
			}
		}
		return -1;
	}


	cint LastIndexOf(const T& Item, cint Start = -1) const
	{
		if (Start < 0) Start = this->Size - 1;
		if (Start >= 0 && Start < this->Size)
		{
			for (cint i = Start; i >= 0; i--)
			{
				if (this->Array[i] == Item)
				{
					return i;
				}
			}
		}
		return -1;
	}


	bool Contains(const T& Item) const
	{
		return this->IndexOf(Item) >= 0;
	}


	void Sort()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		std::sort(this->Array, this->Array + this->Size);
	}


	/* 
	* Pred: (T& A, T& B) -> bool
	* return value: is B greater than A? 
	*/
	template<CALLABLE(PredicateT, bool(T&, T&))>
	void Sort(PredicateT&& Pred)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		std::sort(this->Array, this->Array + this->Size, Pred);
	}

	/* 
	* Pred: (T& Item, cint Index) -> bool
	* if Pred returns true, item is removed from the array
	*/
	template<CALLABLE(PredicateT, bool(T&, cint))>
	void RemoveIf(PredicateT&& Pred)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		cint storedModCount = this->ModCount;
#endif
		cint removedItems = 0;
		for (cint i = 0; i < this->Size; i++)
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->ModCount != storedModCount)
			{
				for (cint ii = i; ii < this->Size; ii++)
				{
					this->Array[ii - removedItems] = std::move(this->Array[ii]);
				}
				for (cint ii = 0; ii < removedItems; ii++)
				{
					this->RemoveLast();
				}

				throw DConcurrentModificationException();
			}
#endif
			bool verdict;
			try 
			{ 
				verdict = Pred(this->Array[i], i); 
			}
			catch (std::exception e) 
			{
				for (cint ii = i; ii < this->Size; ii++)
				{
					this->Array[ii - removedItems] = std::move(this->Array[ii]);
				}
				for (cint ii = 0; ii < removedItems; ii++)
				{
					this->RemoveLast();
				}
				
				throw DRuntimeException(e.what());
			}

			if (verdict)
			{
				removedItems++;
			}
			else if (removedItems > 0)
			{
				this->Array[i - removedItems] = std::move(this->Array[i]);
			}
		}
		for (cint i = 0; i < removedItems; i++)
		{
			this->RemoveLast();
		}
	}


	void Reverse()
	{
		if (this->Size > 1)
		{
			cint middle = (Size % 2 == 0)? (Size / 2) : ((Size + 1) / 2);
			cint last = this->Size - 1;
			for (cint i = 0; i < middle; i++)
			{
				T temp = std::move(this->Array[i]);
				this->Array[i] = std::move(this->Array[last - i]);
				this->Array[last - i] = std::move(temp);
			}
		}
	}


	DArray<T> Reversed() const
	{
		if (this->Size > 0)
		{
			DArray<T> result;
			cint len = this->Size;
			result.Reserve(len);
			T* resultArr = result.Array;
			cint p = 0;
			for (cint i = len - 1; i >= 0; i--)
			{
				new (resultArr + p++) T(this->Array[i]);
			}
			result.Size = len;
			return result;
			
		}
		else
		{
			return {};
		}
	}


	FORCEINLINE cint Length() const
	{
		return this->Size;
	}


	cint Capacity() const
	{
		return this->ArrayCapacity;
	}


	bool IsEmpty() const
	{
		return this->Size < 1;
	}


	const T* Data() const
	{
		return this->Array;
	}

	
	T* GetDataUnsafe()
	{
		return this->Array;
	}

	
	void OverwriteLengthUnsafe(cint NewSize)
	{
		this->Size = NewSize;
	}


private:

	/* NewCapacity: should be >= size */
	FORCEINLINE void _SetCapacity(cint NewCapacity)
	{
		T* newArray = AllocatorT::INSTANCE.Allocate(NewCapacity);
		if (this->Size > 0)
		{
			MoveToUninitialized(this->Array, 0, this->Size, newArray);
		}
		AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
		this->Array = newArray;
		this->ArrayCapacity = NewCapacity;
	}

	FORCEINLINE void _Reserve(cint NewCapacity)
	{
		if (NewCapacity > this->ArrayCapacity)
		{
			this->_SetCapacity(NewCapacity);
		}
	}

public:

	void Reserve(cint NewCapacity)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (NewCapacity < 0)
		{
			throw DIndexException(NewCapacity, this->Size);
		}
#endif
		this->_Reserve(NewCapacity);
	}


	void ReserveExtra(cint ExtraCapacity)
	{
		this->_Reserve(this->Size + ExtraCapacity);
	}


	/* 
	If items are objects (not pointers to object),
	this will fail, if the object has no parameterless constructor
	*/
	void Resize(cint NewSize)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
		if (NewSize < 0)
		{
			throw DIndexException(NewSize, this->Size);
		}
#endif
		if (NewSize > this->ArrayCapacity)
		{
			this->_SetCapacity(NewSize);
		}
		for (cint i = this->Size; i < NewSize; i++)
		{
			new (this->Array + i) T();
		}
		this->Size = NewSize;
	}


	/* reduce storage to number of items */
	void TrimCapacity()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->_SetCapacity(this->Size);
	}


	// Reserve without copying data from the old buffer
	// CapacityAsLength = false sets length to 0
	// CapacityAsLength = true results in an uninitailized array - Unsafe
	void Reallocate(cint NewCapacity, bool CapacityAsLength = false)
	{
		if (this->Array)
		{
			for (cint i = 0; i < this->Size; ++i)
			{
				this->Array[i].~T();
			}
			if (this->ArrayCapacity < NewCapacity)
			{
				AllocatorT::INSTANCE.Deallocate(this->Array, this->Size);
				this->Array = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = NewCapacity);
			}
		}
		else
		{
			this->Array = AllocatorT::INSTANCE.Allocate(this->ArrayCapacity = NewCapacity);
		}
		this->Size = CapacityAsLength? NewCapacity : 0;
	}


#ifdef LANG_DEBUG_CONTAINERS

	Iterator<T> begin() { return Iterator<T>(*this, 0); }
	ConstIterator<T> begin() const { return ConstIterator<T>(*this, 0); }
	
	Iterator<T> end() { return Iterator<T>(*this, this->Size); }
	ConstIterator<T> end() const { return ConstIterator<T>(*this, this->Size); }

#else

	T* begin() { return this->Array; }
	const T* begin() const { return this->Array; }

	T* end() { return this->Array + this->Size; }
	const T* end() const { return this->Array + this->Size; }

#endif

	Enumerator<T> GetEnumerator()
	{
		return Enumerator<T>(*this);
	}


	ConstEnumerator<T> GetEnumerator() const
	{
		return ConstEnumerator<T>(*this);
	}


	ConstEnumerator<T> GetConstEnumerator() const
	{
		return ConstEnumerator<T>(*this);
	}


	Enumerator<T> GetEnumerator(cint Start, cint Stop)
	{
		return Enumerator<T>(*this, Start, Stop);
	}


	ConstEnumerator<T> GetEnumerator(cint Start, cint Stop) const
	{
		return ConstEnumerator<T>(*this, Start, Stop);
	}


	ConstEnumerator<T> GetConstEnumerator(cint Start, cint Stop) const
	{
		return ConstEnumerator<T>(*this, Start, Stop);
	}


	ReverseEnumerator<T> GetReverseEnumerator()
	{
		return ReverseEnumerator<T>(*this);
	}


	ConstReverseEnumerator<T> GetReverseEnumerator() const
	{
		return ConstReverseEnumerator<T>(*this);
	}


	ConstReverseEnumerator<T> GetConstReverseEnumerator() const
	{
		return ConstReverseEnumerator<T>(*this);
	}


	ReverseEnumerator<T> GetReverseEnumerator(cint Start, cint Stop)
	{
		return ReverseEnumerator<T>(*this, Start, Stop);
	}


	ConstReverseEnumerator<T> GetReverseEnumerator(cint Start, cint Stop) const
	{
		return ConstReverseEnumerator<T>(*this, Start, Stop);
	}


	ConstReverseEnumerator<T> GetConstReverseEnumerator(cint Start, cint Stop) const
	{
		return ConstReverseEnumerator<T>(*this, Start, Stop);
	}


	operator DArrayView<T>() const
	{
		return DArrayView<T>(this->Array, this->Size);
	}


	DArrayView<T> ViewRange(cint Start, cint Stop) const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (SCounterUtil::RangeError(Start, Stop, this->Size))
		{
			throw DIndexException(Start, Stop, this->Size);
		}
#endif
		return DArrayView<T>(this->Array + Start, Stop - Start);
	}

};


template<typename T, class AllocatorT = DArrayAllocator<T>>
using DArrayEnumerator = typename DArray<T, AllocatorT>::template Enumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<T>>
using DArrayConstEnumerator = typename DArray<T, AllocatorT>::template ConstEnumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<T>>
using DArrayReverseEnumerator = typename DArray<T, AllocatorT>::template ReverseEnumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<T>>
using DArrayConstReverseEnumerator = typename DArray<T, AllocatorT>::template ConstReverseEnumerator<T>;
