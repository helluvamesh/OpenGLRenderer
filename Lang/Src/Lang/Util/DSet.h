/*______________________________________________________________

DSet
- uses a hash map with chaining collision handling in the background
- uses std::hash to calculate hash code
- supports enhanced for loop (foreach)
- supports initializer list
- supports move semantics

- Iterators are implemeted only as much so the container can be used in enhanced for loops (foreach),
- otherwise use enumerators, see Enumerators.h.

______________________________________________________________*/


#pragma once


template<typename T, class AllocatorT = DNodeArrayAllocator<DMapEntry<T, void*>>>
class DSet
{
	/*___________________________

	ITERATORS
	___________________________*/

public:
	
	template<typename IT>
	class Iterator
	{
	private:
		DMapConstIterator<IT, void*> Iter;

	public:
		
		Iterator(const DSet<IT, AllocatorT>& InSet, bool SeekBegin = true) : 
			Iter(InSet.StorageMap, SeekBegin)
		{
		}

		bool operator==(const Iterator<IT>& Other)
		{
			return this->Iter == Other.Iter;
		}
		bool operator!=(const Iterator<IT>& Other)
		{
			return this->Iter != Other.Iter;
		}

		const IT& operator*()
		{
			return this->Iter->GetKey();
		}

		const IT* operator->()
		{
			return &this->Iter->GetKey();
		}

		Iterator<IT>& operator++()
		{
			++this->Iter;
			return *this;
		}

		Iterator<IT> operator++(cint) = delete;

		Iterator<IT&> operator--() = delete;
		Iterator<IT> operator--(cint) = delete;
	};


	template<typename IT>
	class Enumerator : public IConstEnumerator<IT>
	{
	private:
		DMapConstEnumerator<IT, void*> Etor;

	public:
		explicit Enumerator(const DSet<IT, AllocatorT>& InSet) : 
			Etor(InSet.StorageMap)
		{
		}

		const IT* Next() override
		{
			DMapEntry<IT, void*>* node = this->Etor.Next();
			if (node)
			{
				return &node->GetKey();
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IT>
	class RemovingEnumerator : public IConstRemovingEnumerator<IT>
	{
	private:
		DMapRemovingEnumerator<IT, void*> Etor;

	public:
		explicit RemovingEnumerator(DSet<IT, AllocatorT>& InSet) : 
			Etor(InSet.StorageMap)
		{
		}

		const IT* Next() override
		{
			DMapEntry<IT, void*>* node = this->Etor.Next();
			if (node)
			{
				return &node->GetKey();
			}
			else
			{
				return nullptr;
			}
		}

		void Remove() override
		{
			this->Etor.Remove();
		}
	};
	

	/*___________________________

	STATE
	___________________________*/

public:

	// the first buffer size after Capacity = 0
	// this is an optimization to avoid having to reallocate and rehash many times when size is low
	static constexpr const cint INITIAL_CAPACITY = 16;

private:
	DMap<T, void*, AllocatorT> StorageMap;

public:

	/*___________________________

	CONSTRUCT
	___________________________*/

	DSet()
	{
	}

	/*
	if 'Length' reaches the percentage of 'Capacity' specified by 'LoadFactor',
	the hash table needs to be rehashed (costly operation).
	Default 'LoadFactor' is 0.75f
	*/
	explicit DSet(cint NewCapacity, float NewLoadFactor = 0.75f) :
		StorageMap(NewCapacity, NewLoadFactor)
	{
	}

	DSet(const std::initializer_list<T>& InitList) :
		StorageMap((cint)InitList.size(), 0.75f)
	{
		cint len = (cint)InitList.size();
		this->StorageMap.Reserve(SCounterUtil::Max(len, INITIAL_CAPACITY));
		auto end = InitList.end();
		for (auto iter = InitList.begin(); iter != end; ++iter)
		{
			this->StorageMap.Emplace(*iter);
		}
	}

	/*___________________________

	COPY
	___________________________*/

	DSet(const DSet<T, AllocatorT>& Other)
	{
		this->StorageMap = Other.StorageMap;
	}
	DSet& operator=(const DSet<T, AllocatorT>& Other)
	{
		this->StorageMap = Other.StorageMap;
		return *this;
	}

	/*___________________________

	MOVE
	___________________________*/

	DSet(DSet<T, AllocatorT>&& Other) noexcept
	{
		this->StorageMap = std::move(Other.StorageMap);
	}
	DSet& operator=(DSet<T, AllocatorT>&& Other) noexcept
	{
		this->StorageMap = std::move(Other.StorageMap);
		return *this;
	}

	/*___________________________

	DESTRUCT
	___________________________*/

	~DSet()
	{
	}

	/*___________________________

	SET MANIPULATION
	___________________________*/

	/* returns true, if this is a new key */
	bool Add(const T& NewItem)
	{
		return this->StorageMap.Add(NewItem, nullptr);
	}


	/* returns true, if this is a new key */
	bool Add(T&& NewItem)
	{
		return this->StorageMap.Add(std::move(NewItem), nullptr);
	}


	bool Contains(const T& Item) const
	{
		return this->StorageMap.Find(Item);
	}


	/* returns true, if the item was found and removed; 
	returns false, if nothing was removed */
	bool Remove(const T& Item)
	{
		return this->StorageMap.Remove(Item);
	}


	void Clear()
	{
		this->StorageMap.Clear();
	}


	FORCEINLINE cint Length() const
	{
		return this->StorageMap.Length();
	}


	bool IsEmpty() const
	{
		return this->StorageMap.Length() < 1;
	}


	cint Capacity() const
	{
		return this->StorageMap.Capacity();
	}


	void Reserve(cint NewCapacity)
	{
		this->StorageMap.Reserve(NewCapacity);
	}


	void ReserveExtra(cint ExtraCapacity)
	{
		this->StorageMap.ReserveExtra(ExtraCapacity);
	}


	/* reduce storage to number of items */
	void TrimCapacity()
	{
		this->StorageMap.TrimCapacity();
	}


	/*
	Pred: (const T& Item) -> bool
	if Pred returns true, item is removed from the set
	*/
	template<CALLABLE(PredicateT, bool(const T&))>
	void RemoveIf(PredicateT&& Pred)
	{
		DMapRemovingEnumerator<T, void*> etor (this->StorageMap);
		while (DMapEntry<T, void*>* node = etor.Next())
		{
			if (Pred(node->GetKey()))
			{
				etor.Remove();
			}
		}
	}


	Iterator<T> begin() const
	{
		return Iterator<T>(*this, /*SeekBegin*/true);
	}


	Iterator<T> end() const
	{
		return Iterator<T>(*this, /*SeekBegin*/false);
	}


	Enumerator<T> GetEnumerator() const
	{
		return Enumerator<T>(*this);
	}


	RemovingEnumerator<T> GetRemovingEnumerator()
	{
		return RemovingEnumerator<T>(*this);
	}

};


template<typename T, class AllocatorT = DNodeArrayAllocator<DMapEntry<T, void*>>>
using DSetIterator = typename DSet<T, AllocatorT>::template Iterator<T>;

template<typename T, class AllocatorT = DNodeArrayAllocator<DMapEntry<T, void*>>>
using DSetEnumerator = typename DSet<T, AllocatorT>::template Enumerator<T>;

template<typename T, class AllocatorT = DNodeArrayAllocator<DMapEntry<T, void*>>>
using DSetRemovingEnumerator = typename DSet<T, AllocatorT>::template RemovingEnumerator<T>;
