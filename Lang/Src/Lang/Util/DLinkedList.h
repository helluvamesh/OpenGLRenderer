/*_________________________________________________________

DLinkedList
- implements a doubly linked list
- can construct objects directly inside the nodes (emplace)
- supports enhanced for loop (foreach)
- supports move semantics
- supports initializer list

- Iterators are implemeted only as much so the container can be used in enhanced for loops (foreach),
- otherwise use enumerators, see Enumerators.h.

_________________________________________________________*/


#pragma once


template<typename T>
class DLinkedListEntry
{

	template<typename T, class AllocatorT>
	friend class DLinkedList;

private:

	DLinkedListEntry* Prev;
	DLinkedListEntry* Next;
	T Item;

public:

	DLinkedListEntry(DLinkedListEntry* InPrev, DLinkedListEntry* InNext, const T& InItem) :
		Item(InItem), 
		Prev(InPrev),
		Next(InNext)
	{
	}

	DLinkedListEntry(DLinkedListEntry* InPrev, DLinkedListEntry* InNext, const T&& InItem) : 
		Item(std::move(InItem)),
		Prev(InPrev), 
		Next(InNext)
	{
	}

	template<typename... ConstructorArgs>
	DLinkedListEntry(DLinkedListEntry* InPrev, DLinkedListEntry* InNext, ConstructorArgs&&... Args) :
		Item(std::move(Args)...),
		Prev(InPrev), 
		Next(InNext)
	{
	}

	FORCEINLINE T& GetItem()
	{
		return this->Item;
	}

};


template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
class DLinkedList
{
public:

	// ------------- ITERATORS

	template<typename IT>
	class Iterator
	{
	private:

		DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		Iterator(DLinkedList<IT, AllocatorT>& InList, bool End)
		{
			this->List = &InList;
			this->Node = End? nullptr : InList.Head;
			this->StoredModCount = InList.ModCount;
		}

		bool operator==(const Iterator<IT>& Other) const
		{
			return this->Node == Other.Node;
		}
		bool operator!=(const Iterator<IT>& Other) const
		{
			return this->Node != Other.Node;
		}

		IT& operator*() const
		{
			return this->Node->Item;
		}

		IT* operator->() const
		{
			return &this->Node->Item;
		}

		Iterator<IT>& operator++()
		{
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
			this->Node = this->Node->Next;
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

		const DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		ConstIterator(const DLinkedList<IT, AllocatorT>& InList, bool End)
		{
			this->List = &InList;
			this->Node = End? nullptr : InList.Head;
			this->StoredModCount = InList.ModCount;
		}

		bool operator==(const ConstIterator<IT>& Other) const
		{
			return this->Node == Other.Node;
		}
		bool operator!=(const ConstIterator<IT>& Other) const
		{
			return this->Node != Other.Node;
		}

		const IT& operator*() const
		{
			return this->Node->Item;
		}

		const IT* operator->() const
		{
			return &this->Node->Item;
		}

		ConstIterator<IT>& operator++()
		{
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
			this->Node = this->Node->Next;
			return *this;
		}

		ConstIterator<IT>& operator--() = delete;

		ConstIterator<IT> operator++(cint) = delete;
		ConstIterator<IT> operator--(cint) = delete;
	};


	template<typename IT>
	class Enumerator : public IEnumerator<IT>
	{
	private:
		DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		explicit Enumerator(DLinkedList<IT, AllocatorT>& InList) :
			List(&InList),
			Node(nullptr), 
			StoredModCount(InList.ModCount)
		{
		}

		IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Node)
			{
				this->Node = this->Node->Next;
			}
			else
			{
				this->Node = this->List->Head;
			}
			if (this->Node)
				return &this->Node->Item;
			else
				return nullptr;
		}
	};


	template<typename IT>
	class ConstEnumerator : public IConstEnumerator<IT>
	{
	private:
		const DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		explicit ConstEnumerator(const DLinkedList<IT, AllocatorT>& InList) :
			List(&InList), 
			Node(nullptr),
			StoredModCount(List.ModCount)
		{
		}

		const IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Node)
			{
				this->Node = this->Node->Next;
			}
			else
			{
				this->Node = this->List->Head;
			}
			if (this->Node)
				return &this->Node->Item;
			else
				return nullptr;
		}
	};


	template<typename IT>
	class RemovingEnumerator : public IRemovingEnumerator<IT>
	{
	private:
		DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		DLinkedListEntry<IT>* NextNode;
		cint StoredModCount;

	public:

		explicit RemovingEnumerator(DLinkedList<IT, AllocatorT>& InList) :
			List(&InList),
			Node(nullptr), 
			NextNode(nullptr), 
			StoredModCount(InList.ModCount)
		{
		}

		IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->NextNode)
			{
				this->Node = this->NextNode;
				this->NextNode = nullptr;
				return &this->Node->Item;
			}
			else
			{
				if (this->Node)
				{
					this->Node = this->Node->Next;
				}
				else
				{
					this->Node = this->List->Head;
				}
				if (this->Node)
					return &this->Node->Item;
				else
					return nullptr;
			}
		}

		void Remove() override
		{
			if (this->Node == nullptr)
			{
				this->NextNode = this->List->Head;
			}
			else if (this->Node->Next)
			{
				this->NextNode = this->Node->Next;
			}
			this->List->Remove(this->Node);
			this->StoredModCount = this->List->ModCount;
		}
	};


	template<typename IT>
	class ReverseEnumerator : public IEnumerator<IT>
	{
	private:
		DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		explicit ReverseEnumerator(DLinkedList<IT, AllocatorT>& InList) :
			List(&InList), 
			Node(nullptr),
			StoredModCount(InList.ModCount)
		{
		}

		IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Node)
			{
				this->Node = this->Node->Prev;
			}
			else
			{
				this->Node = this->List->Tail;
			}
			if (this->Node)
				return &this->Node->Item;
			else
				return nullptr;
		}
	};


	template<typename IT>
	class ConstReverseEnumerator : public IConstEnumerator<IT>
	{
	private:
		const DLinkedList<IT, AllocatorT>* List;
		DLinkedListEntry<IT>* Node;
		cint StoredModCount;

	public:

		explicit ConstReverseEnumerator(const DLinkedList<IT, AllocatorT>& InList) :
			List(&InList), 
			Node(nullptr),
			StoredModCount(InList.ModCount)
		{
		}

		const IT* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->List->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Node)
			{
				this->Node = this->Node->Prev;
			}
			else
			{
				this->Node = this->List->Tail;
			}
			if (this->Node)
				return &this->Node->Item;
			else
				return nullptr;
		}
	};


	// ------------- STATE -------------

private:

	cint Size;
	cint ModCount = 0;
	DLinkedListEntry<T>* Head;
	DLinkedListEntry<T>* Tail;

public:

	// ------------- CONSTRUCT -------------

	DLinkedList() : 
		Size(0), 
		Head(nullptr), 
		Tail(nullptr)
	{
	}

	DLinkedList(const std::initializer_list<T> InitList) : 
		Size(0),
		Head(nullptr),
		Tail(nullptr)
	{
		cint initSize = (cint)InitList.size();
		auto iter = InitList.begin();
		for (cint i = 0; i < initSize; i++)
		{
			this->AddLast(*(iter + i));
		}
	}

	// ------------- COPY -------------

	DLinkedList(const DLinkedList& Other) : 
		Size(0), 
		Head(nullptr),
		Tail(nullptr)
	{
		for (const T& item : Other)
		{
			this->AddLast(item);
		}
	}

	DLinkedList& operator=(const DLinkedList& Other)
	{
		for (const T& item : Other)
		{
			this->AddLast(item);
		}
		return *this;
	}

	// ------------- MOVE -------------

	DLinkedList(DLinkedList&& Other)
	{
		this->Head = Other.Head;
		this->Tail = Other.Tail;
		this->Size = Other.Size;
		Other.Head = Other.Tail = nullptr;
		Other.Size = 0;
	}

	DLinkedList& operator=(DLinkedList&& Other)
	{
		this->Clear();
		this->Head = Other.Head;
		this->Tail = Other.Tail;
		this->Size = Other.Size;
		Other.Head = Other.Tail = nullptr;
		Other.Size = 0;
		return *this;
	}

	// ------------- DESTRUCT -------------

	~DLinkedList()
	{
		this->Clear();
	}

	// ------------- MANIPULATE -------------

	void AddLast(const T& Item)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(this->Tail, nullptr, Item);
			this->Tail->Next = newNode;
			this->Tail = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, Item);
			this->Head = this->Tail = newNode;
		}
		this->Size++;
	}


	void AddLast(T&& Item)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(this->Tail, nullptr, std::move(Item));
			this->Tail->Next = newNode;
			this->Tail = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, std::move(Item));
			this->Head = this->Tail = newNode;
		}
		this->Size++;
	}


	T& AddLastDefault()
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		return this->EmplaceLast();
	}


	template<typename... ConstructorArgs>
	T& EmplaceLast(ConstructorArgs&&... Args)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(this->Tail, nullptr, std::move(Args)...);
			this->Tail->Next = newNode;
			this->Tail = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, std::move(Args)...);
			this->Head = this->Tail = newNode;
		}
		this->Size++;
		return newNode->Item;
	}


	void AddFirst(const T& Item)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(nullptr, this->Head, Item);
			this->Head->Prev = newNode;
			this->Head = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, Item);
			this->Head = this->Tail = newNode;
		}
		this->Size++;
	}


	void AddFirst(T&& Item)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(nullptr, this->Head, std::move(Item));
			this->Head->Prev = newNode;
			this->Head = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, std::move(Item));
			this->Head = this->Tail = newNode;
		}
		this->Size++;
	}


	T& AddFirstDefault()
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		return this->EmplaceFirst();
	}


	template<typename... ConstructorArgs>
	T& EmplaceFirst(ConstructorArgs&&... Args)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		DLinkedListEntry<T>* newNode = AllocatorT::INSTANCE.Allocate(1);
		if (Size > 0)
		{
			new (newNode) DLinkedListEntry<T>(nullptr, this->Head, std::move(Args)...);
			this->Head->Prev = newNode;
			this->Head = newNode;
		}
		else
		{
			new (newNode) DLinkedListEntry<T>(nullptr, nullptr, std::move(Args)...);
			this->Head = this->Tail = newNode;
		}
		this->Size++;
		return newNode->Item;
	}
	

	// returns true, if Item was found and removed
	bool Remove(DLinkedListEntry<T>* Node)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		if (this->Size >= 2)
		{
			if (Node->Prev)
			{
				Node->Prev->Next = Node->Next;
			}
			if (Node->Next)
			{
				Node->Next->Prev = Node->Prev;
			}
			if (this->Head == Node)
			{
				this->Head = Node->Next;
			}
			if (this->Tail == Node)
			{
				this->Tail = Node->Prev;
			}
			Node->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(Node, 1);
			this->Size--;
			return true;
		}
		else if (this->Size == 1)
		{
			this->Head = this->Tail = nullptr;
			Node->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(Node, 1);
			this->Size = 0;
			return true;
		}

		return false;
	}
	
public:

	// returns true if Item was found and removed
	bool Remove(const T& Item)
	{
		DLinkedListEntry<T>* node = this->Head;
		while (node)
		{
			if (node->Item == Item)
			{	
				this->Remove(node);
				return true;
			}
			else
			{
				node = node->Next;
			}
		}
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		return false;
	}


	void RemoveLast()
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		if (this->Size >= 2)
		{
			DLinkedListEntry<T>* nextToLast = this->Tail->Prev;
			this->Tail->Prev->Next = nullptr;
			this->Tail->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(this->Tail, 1);
			this->Tail = nextToLast;
			if (--this->Size == 1)
			{
				this->Head = this->Tail;
			}
		}
		else if (this->Size == 1)
		{
			this->Tail->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(this->Tail, 1);
			this->Head = this->Tail = nullptr;
			this->Size--;
		}
		else
		{
#ifdef LANG_DEBUG_CONTAINERS
			throw IndexException(0, this->Size);
#endif
		}
	}


	void RemoveFirst() 
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		if (this->Size >= 2)
		{
			DLinkedListEntry<T>* second = this->Head->Next;
			this->Head->Next->Prev = nullptr;
			this->Head->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(this->Head, 1);
			this->Head = second;
			if (--this->Size == 1)
			{
				this->Tail = this->Head;
			}
		}
		else if (this->Size == 1)
		{
			this->Tail->~DLinkedListEntry();
			AllocatorT::INSTANCE.Deallocate(this->Head, 1);
			this->Head = this->Tail = nullptr;
			this->Size--;
		}
		else
		{
#ifdef LANG_DEBUG_CONTAINERS
			throw IndexException(-1, this->Size);
#endif
		}
	}


	void RemoveGetLast(T& OutItem)
	{
		if (this->Size > 0)
		{
			OutItem = this->Tail->Item;
			this->RemoveLast();
		}
	}


	void RemoveGetFirst(T& OutItem) 
	{
		if (this->Size > 0)
		{
			OutItem = this->Head->Item;
			this->RemoveFirst();
		}
	}


	/*
	Pred: (T& Item) -> bool
	if Pred returns true, key - value pair is removed from the map
	*/
	template<CALLABLE(PredicateT, bool(T&))>
	void RemoveIf(PredicateT&& Pred)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		RemovingEnumerator<T> etor (*this);
		T* item;
		while (item = etor.Next())
		{
			if (Pred(*item))
			{
				etor.Remove();
			}
		}
	}


	void Clear()
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
#endif
		if (this->Size > 0)
		{
			DLinkedListEntry<T>* node = this->Head;
			while (node)
			{
				DLinkedListEntry<T>* nextNode = node->Next;
				node->~DLinkedListEntry();
				AllocatorT::INSTANCE.Deallocate(node, 1);
				node = nextNode;
			}
			this->Size = 0;
			this->Head = this->Tail = nullptr;
		}
	}


	T& AtFirst()
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size == 0)
		{
			throw IndexException(0, this->Size);
		}
#endif
		return this->Head->Item;
	}


	const T& AtFirst() const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size == 0)
		{
			throw IndexException(0, this->Size);
		}
#endif
		return this->Head->Item;
	}


	T& AtLast()
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size == 0)
		{
			throw IndexException(-1, this->Size);
		}
#endif
		return this->Tail->Item;
	}


	const T& AtLast() const
	{
#ifdef LANG_DEBUG_CONTAINERS
		if (this->Size == 0)
		{
			throw IndexException(-1, this->Size);
		}
#endif
		return this->Tail->Item;
	}


	void Resize(cint NewSize)
	{
#ifdef LANG_DEBUG_CONTAINERS
		this->ModCount++;
		if (NewSize < 0)
		{
			throw IndexException(NewSize, this->Size);
		}
#endif
		if (NewSize > this->Size)
		{
			for (cint i = Size; i < NewSize; i++)
			{
				this->EmplaceLast();
			}
		}
		else
		{
			cint itemsToRemove = Size - NewSize;
			for (cint i = 0; i < itemsToRemove; i++)
			{
				this->RemoveLast();
			}
		}
	}


	void Reverse()
	{
		if (this->Size > 1)
		{
			DLinkedListEntry<T>* node = this->Head;
			while (node)
			{
				DLinkedListEntry<T>* temp = node->Next;
				node->Next = node->Prev;
				node->Prev = temp;
				node = temp;
			}
			DLinkedListEntry<T>* temp = this->Head;
			this->Head = this->Tail;
			this->Tail = temp;
		}
	}


	DLinkedList<T> Reversed() const
	{
		DLinkedList<T> result = *this;
		result.Reverse();
		return result;
	}
	

	FORCEINLINE cint Length() const
	{
		return this->Size;
	}


	bool IsEmpty() const
	{
		return this->Size < 1;
	}


	Iterator<T> begin() { return Iterator<T>(*this, false); }
	ConstIterator<T> begin() const { return ConstIterator<T>(*this, false); }

	Iterator<T> end() { return Iterator<T>(*this, true); }
	ConstIterator<T> end() const { return ConstIterator<T>(*this, true); }


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


	RemovingEnumerator<T> GetRemovingEnumerator()
	{
		return RemovingEnumerator<T>(*this);
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

};


template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListIterator = typename DLinkedList<T, AllocatorT>::template Iterator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListConstIterator = typename DLinkedList<T, AllocatorT>::template ConstIterator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListEnumerator = typename DLinkedList<T, AllocatorT>::template Enumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListConstEnumerator = typename DLinkedList<T, AllocatorT>::template ConstEnumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListRemovingEnumerator = typename DLinkedList<T, AllocatorT>::template RemovingEnumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListReverseEnumerator = typename DLinkedList<T, AllocatorT>::template ReverseEnumerator<T>;

template<typename T, class AllocatorT = DArrayAllocator<DLinkedListEntry<T>>>
using DLinkedListConstReverseEnumerator = typename DLinkedList<T, AllocatorT>::template ConstReverseEnumerator<T>;
