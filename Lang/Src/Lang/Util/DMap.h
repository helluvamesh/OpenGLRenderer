/*________________________________________________________________________

DMap
- implements an unordered hash map with chaining collision handling
- uses std::hash to calculate hash code
- supports enhanced for loop (foreach)
- supports initializer list
- supports move semantics

- Iterators are implemeted only as much so the container can be used in enhanced for loops (foreach),
- otherwise use enumerators, see Enumerators.h.

________________________________________________________________________*/


#pragma once


template<typename K, typename V>
struct DMapEntry
{
	template<typename K, typename V, class AllocatorT>
	friend class DMap;

private:
	DMapEntry<K, V>* Next;
	DMapEntry<K, V>* TempNext;
	K Key;
	V Value;

public:

	DMapEntry(const K& InKey, const V& InValue) : 
		Key(InKey),
		Value(InValue),
		Next(nullptr), 
		TempNext(nullptr) 
	{
	}

	DMapEntry(K&& InKey, V&& InValue) : 
		Key(std::move(InKey)),
		Value(std::move(InValue)),
		Next(nullptr), 
		TempNext(nullptr) 
	{
	}

	DMapEntry(const K& InKey, V&& InValue) :
		Key(InKey),
		Value(std::move(InValue)), 
		Next(nullptr), 
		TempNext(nullptr)
	{
	}

	DMapEntry(K&& InKey, const V& InValue) :
		Key(std::move(InKey)),
		Value(InValue), 
		Next(nullptr),
		TempNext(nullptr)
	{
	}

	template<typename... ConstructorArgs>
	DMapEntry(const K& InKey, ConstructorArgs&&... ValueArgs) : 
		Key(InKey), 
		Value(std::move(ValueArgs)...),
		Next(nullptr), 
		TempNext(nullptr) 
	{
	}

	template<typename... ConstructorArgs>
	DMapEntry(K&& InKey, ConstructorArgs&&... ValueArgs) :
		Key(std::move(InKey)), 
		Value(std::move(ValueArgs)...),
		Next(nullptr),
		TempNext(nullptr) 
	{ 
	}

	FORCEINLINE const K& GetKey() const
	{
		return this->Key;
	}

	FORCEINLINE V& GetValue()
	{
		return this->Value;
	}

	FORCEINLINE const V& GetValue() const
	{
		return this->Value;
	}
};


template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
class DMap
{
public:

	/*___________________________

	ITERATORS
	___________________________*/

	template<typename IK, typename IV>
	class Iterator
	{
	private:
		DMap<IK, IV, AllocatorT>* Map;
		cint Hash;
		DMapEntry<IK, IV>* Node;
		cint StoredModCount;

	public:

		Iterator(DMap<IK, IV, AllocatorT>& InMap, bool SeekBegin = true)
		{
			this->Map = &InMap;
			this->StoredModCount = InMap.ModCount;
			this->Hash = -1;
			this->Node = nullptr;
			if (SeekBegin)
			{
				this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash);
			}
		}

		bool operator==(const Iterator<IK, IV>& Other) const
		{
			return this->Node == Other.Node;
		}
		bool operator!=(const Iterator<IK, IV>& Other) const
		{
			return this->Node != Other.Node;
		}

		DMapEntry<IK, IV>& operator*() const
		{
			return *(this->Node);
		}

		DMapEntry<IK, IV>* operator->() const
		{
			return this->Node;
		}

		Iterator<IK, IV>& operator++()
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->Map->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash) == false)
			{
				this->Node = nullptr;
				this->Hash = -1;
			}

			return *this;
		}

		Iterator<IK, IV> operator++(cint) = delete;

		Iterator<IK, IV>& operator--() = delete;
		Iterator<IK, IV> operator--(cint) = delete;
	};


	template<typename IK, typename IV>
	class ConstIterator
	{
	private:
		const DMap<IK, IV, AllocatorT>* Map;
		cint Hash;
		DMapEntry<IK, IV>* Node;
		cint StoredModCount;

	public:

		ConstIterator(const DMap<IK, IV, AllocatorT>& InMap, bool SeekBegin = true)
		{
			this->Map = &InMap;
			this->StoredModCount = InMap.ModCount;
			this->Hash = -1;
			this->Node = nullptr;
			if (SeekBegin)
			{
				this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash);
			}
		}

		bool operator==(const ConstIterator<IK, IV>& Other) const
		{
			return this->Node == Other.Node;
		}
		bool operator!=(const ConstIterator<IK, IV>& Other) const
		{
			return this->Node != Other.Node;
		}

		const DMapEntry<IK, IV>& operator*() const
		{
			return *(this->Node);
		}

		const DMapEntry<IK, IV>* operator->() const
		{
			return this->Node;
		}

		ConstIterator<IK, IV>& operator++()
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->Map->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash) == false)
			{
				this->Node = nullptr;
				this->Hash = -1;
			}

			return *this;
		}

		ConstIterator<IK, IV> operator++(cint) = delete;

		ConstIterator<IK, IV>& operator--() = delete;
		ConstIterator<IK, IV> operator--(cint) = delete;
	};


	template<typename IK, typename IV>
	class Enumerator : public IEnumerator<DMapEntry<IK, IV>>
	{
	private:
		DMap<IK, IV, AllocatorT>* Map;
		cint Hash;
		DMapEntry<IK, IV>* Node;
		cint StoredModCount;

	public:

		explicit Enumerator(DMap<IK, IV, AllocatorT>& InMap) :
			Map(&InMap), 
			Hash(-1),
			Node(nullptr),
			StoredModCount(InMap.ModCount)
		{
		}

		DMapEntry<IK, IV>* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->Map->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash))
			{
				return this->Node;
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IK, typename IV>
	class ConstEnumerator : public IConstEnumerator<DMapEntry<IK, IV>>
	{
	private:
		const DMap<IK, IV, AllocatorT>* Map;
		cint Hash;
		DMapEntry<IK, IV>* Node;
		cint StoredModCount;

	public:

		explicit ConstEnumerator(const DMap<IK, IV, AllocatorT>& InMap) :
			Map(&InMap),
			Hash(-1), 
			Node(nullptr), 
			StoredModCount(InMap.ModCount)
		{
		}

		const DMapEntry<IK, IV>* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->Map->ModCount != this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash))
			{
				return this->Node;
			}
			else
			{
				return nullptr;
			}
		}
	};


	template<typename IK, typename IV>
	class RemovingEnumerator : public IRemovingEnumerator<DMapEntry<IK, IV>>
	{
	private:
		DMap<IK, IV, AllocatorT>* Map;
		cint Hash;
		DMapEntry<IK, IV>* Node;
		DMapEntry<IK, IV>* NextNode;
		cint NextHash;
		cint StoredModCount;

	public:

		explicit RemovingEnumerator(DMap<IK, IV, AllocatorT>& InMap) :
			Map(&InMap), Hash(-1), Node(nullptr), NextNode(nullptr), NextHash(-1), 
			StoredModCount(InMap.ModCount)
		{
		}

		DMapEntry<IK, IV>* Next() override
		{
#ifdef LANG_DEBUG_CONTAINERS
			if (this->Map->ModCount > this->StoredModCount)
			{
				throw DConcurrentModificationException();
			}
#endif
			if (this->NextNode)
			{
				this->Node = this->NextNode;
				this->Hash = this->NextHash;
				this->NextNode = nullptr;
				this->NextHash = -1;
				return this->Node;
			}
			else
			{
				if (this->Map->NextNode(this->Node, this->Hash, /*out*/ this->Node, /*out*/ this->Hash))
				{
					return this->Node;
				}
				else
				{
					return nullptr;
				}
			}
		}

		void Remove() override
		{
			this->Map->NextNode(this->Node, this->Hash, /*out*/ this->NextNode, /*out*/ this->NextHash);
			this->Map->Remove(this->Node->GetKey(), this->Hash);
			this->Node = nullptr;
			this->StoredModCount = this->Map->ModCount;
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
	
	cint ArrayCapacity;
	cint Size;
	float LoadFactor;
	cint ModCount = 0;
	DMapEntry<K, V>** Array;

public:

	/*___________________________

	CONSTRUCT
	___________________________*/

	DMap() : 
		Array(nullptr), 
		ArrayCapacity(0), 
		Size(0), 
		LoadFactor(0.75f)
	{
	}

	/*
	if 'Length' reaches the percentage of 'Capacity' specified by 'LoadFactor',
	the hash table needs to be rehashed (costly operation).
	Default 'LoadFactor' is 0.75f
	*/
	explicit DMap(cint NewCapacity, float NewLoadFactor = 0.75f)
	{
		if (NewLoadFactor <= .0f)
		{
			NewLoadFactor = 0.75f;
		}
		NewCapacity = (cint)(((double)NewCapacity) / NewLoadFactor);
		if (NewCapacity >= 1)
		{
			this->Array = AllocatorT::INSTANCE.AllocateArray(this->ArrayCapacity = NewCapacity);
			for (cint i = 0; i < NewCapacity; i++)
			{
				this->Array[i] = nullptr;
			}
			this->Size = 0;
			this->LoadFactor = NewLoadFactor;
		}
		else
		{
			this->Array = nullptr;
			this->ArrayCapacity = 0;
			this->Size = 0;
			this->LoadFactor = 0.75f;
		}
	}

	DMap(const std::initializer_list<DMapEntry<K, V>>& InitList)
	{
		this->Array = nullptr;
		this->ArrayCapacity = 0;
		this->Size = 0;
		this->LoadFactor = 0.75f;

		cint len = (cint)InitList.size();
		this->Reserve(SCounterUtil::Max(len, INITIAL_CAPACITY));
		auto end = InitList.end();
		for (auto iter = InitList.begin(); iter != end; ++iter)
		{
			this->Emplace(iter->Key, iter->Value);
		}
	}

	/*___________________________

	COPY
	___________________________*/

private:

	FORCEINLINE static DMapEntry<K, V>* CopyNodeChain(DMapEntry<K, V>* Node)
	{
		if (Node)
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(Node->Key, Node->Value);
			DMapEntry<K, V>* copyRoot = address;
			DMapEntry<K, V>* end = copyRoot;
			DMapEntry<K, V>* current = Node->Next;

			while (current)
			{
				DMapEntry<K, V>* currentAddress = AllocatorT::INSTANCE.AllocateNode();
				new (currentAddress) DMapEntry<K, V>(current->Key, current->Value);
				end = end->Next = currentAddress;
				current = current->Next;
			}

			return copyRoot;
		}
		else
		{
			return nullptr;
		}
	}

public:

	DMap(const DMap<K, V, AllocatorT>& Other)
	{
		this->LoadFactor = Other.LoadFactor;
		this->Array = AllocatorT::INSTANCE.AllocateArray(this->ArrayCapacity = Other.ArrayCapacity);
		this->Size = Other.Size;
		for (cint i = 0; i < this->ArrayCapacity; i++)
		{
			this->Array[i] = CopyNodeChain(Other.Array[i]);
		}
	}
	DMap& operator=(const DMap<K, V, AllocatorT>& Other)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->DeleteBuffer();
		this->Array = AllocatorT::INSTANCE.AllocateArray(this->ArrayCapacity = Other.ArrayCapacity);
		this->Size = Other.Size;
		for (cint i = 0; i < this->ArrayCapacity; i++)
		{
			this->Array[i] = CopyNodeChain(Other.Array[i]);
		}
		return *this;
	}

	/*___________________________

	MOVE
	___________________________*/

	DMap(DMap<K, V, AllocatorT>&& Other) noexcept
	{
		this->LoadFactor = Other.LoadFactor;
		this->Array = Other.Array;
		Other.Array = nullptr;
		this->ArrayCapacity = Other.ArrayCapacity;
		this->Size = Other.Size;
		Other.ArrayCapacity = Other.Size = 0;
	}
	DMap& operator=(DMap<K, V, AllocatorT>&& Other) noexcept
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

	FORCEINLINE void ClearBuffer()
	{
		for (cint i = 0; i < this->ArrayCapacity; ++i)
		{
			DMapEntry<K, V>* node = this->Array[i];
			while (node)
			{
				DMapEntry<K, V>* temp = node;
				node = node->Next;
				temp->~DMapEntry();
				AllocatorT::INSTANCE.DeallocateNode(temp);
			}
			this->Array[i] = nullptr;
		}
	}

	FORCEINLINE void DeleteBuffer()
	{
		this->ClearBuffer();
		AllocatorT::INSTANCE.DeallocateArray(this->Array, this->ArrayCapacity);
	}

public:

	~DMap()
	{
		this->DeleteBuffer();
	}

private:

	/* NewCapacity must be at least 1 */
	FORCEINLINE void Rehash(cint NewCapacity)
	{
		NewCapacity = (cint)(((double)NewCapacity) / this->LoadFactor);

		DMapEntry<K, V>** newArray = AllocatorT::INSTANCE.AllocateArray(NewCapacity);
		for (cint i = 0; i < NewCapacity; i++)
		{
			newArray[i] = nullptr;
		}

		for (cint i = 0; i < this->ArrayCapacity; i++)
		{
			DMapEntry<K, V>* node = this->Array[i];
			while (node)
			{
				cint newIndex = std::hash<K>()(node->Key) % NewCapacity;
				DMapEntry<K, V>* occupiedNode = newArray[newIndex];
				// array slot is empty, so simply put the node in the array
				if (occupiedNode == nullptr)
				{
					newArray[newIndex] = node;
				}
				// array slot is occupied, add 'node' at the end of the node chain starting with 'occupiedNode'
				else
				{
					DMapEntry<K, V>* next = occupiedNode->TempNext;
					if (next == nullptr)
					{
						occupiedNode->TempNext = node;
					}
					else
					{
						while (next)
						{
							if (next->TempNext == nullptr)
							{
								next->TempNext = node;
								break;
							}
							else
							{
								next = next->TempNext;
							}
						}
					}
				}
				node = node->Next;
			}
		}

		for (cint i = 0; i < NewCapacity; i++)
		{
			DMapEntry<K, V>* node = newArray[i];
			while (node)
			{
				node->Next = node->TempNext;
				node->TempNext = nullptr;
				node = node->Next;
			}
		}
		AllocatorT::INSTANCE.DeallocateArray(this->Array, this->ArrayCapacity);
		this->Array = newArray;
		this->ArrayCapacity = NewCapacity;
	}


	/* returns true, if the key was not present in the map before */
	FORCEINLINE bool AddNodeToChain(DMapEntry<K, V>* Node, const K& InKey, const V& InValue)
	{
		while (Node)
		{
			if (Node->Key == InKey)
			{
				// overwrite node with the new one
				Node->Value = InValue;
				return false;
			}
			else if (Node->Next == nullptr)
			{
				// add node to the end of the chain
				DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
				new (address) DMapEntry<K, V>(InKey, InValue);
				Node->Next = address;
				return true;
			}
			else Node = Node->Next;  // travel further down the chain
		}
		return false;  // to make the compiler happy
	}


	/* returns true, if the key was not present in the map before */
	FORCEINLINE bool AddNodeToChain(DMapEntry<K, V>* Node, K&& InKey, V&& InValue)
	{
		while (Node)
		{
			if (Node->Key == InKey)
			{
				// overwrite node with the new one
				Node->Value = std::move(InValue);
				return false;
			}
			else if (Node->Next == nullptr)
			{
				// add node to the end of the chain
				DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
				new (address) DMapEntry<K, V>(std::move(InKey), std::move(InValue));
				Node->Next = address;
				return true;
			}
			else Node = Node->Next;  // travel further down the chain
		}
		return false;  // to make the compiler happy
	}

	/* returns true, if the key was not present in the map before */
	FORCEINLINE bool AddNodeToChain(DMapEntry<K, V>* Node, K&& InKey, const V& InValue)
	{
		while (Node)
		{
			if (Node->Key == InKey)
			{
				// overwrite node with the new one
				Node->Value = InValue;
				return false;
			}
			else if (Node->Next == nullptr)
			{
				// add node to the end of the chain
				DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
				new (address) DMapEntry<K, V>(std::move(InKey), InValue);
				Node->Next = address;
				return true;
			}
			else Node = Node->Next;  // travel further down the chain
		}
		return false;  // to make the compiler happy
	}

	/* returns true, if the key was not present in the map before */
	FORCEINLINE bool AddNodeToChain(DMapEntry<K, V>* Node, const K& InKey, V&& InValue)
	{
		while (Node)
		{
			if (Node->Key == InKey)
			{
				// overwrite node with the new one
				Node->Value = std::move(InValue);
				return false;
			}
			else if (Node->Next == nullptr)
			{
				// add node to the end of the chain
				DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
				new (address) DMapEntry<K, V>(InKey, InValue);
				Node->Next = address;
				return true;
			}
			else Node = Node->Next;  // travel further down the chain
		}
		return false;  // to make the compiler happy
	}

	/* returns true, if the key was not present in the map before */
	template<typename... ConstructorArgs>
	FORCEINLINE bool AddNodeToChainConstruct(DMapEntry<K, V>* Node, DMapEntry<K, V>*& OutNode, const K& InKey, ConstructorArgs&&... ValueArgs)
	{
		while (Node)
		{
			if (Node->Key == InKey)
			{
				// overwrite node value with the new one
				(&Node->Value)->~V();
				new (&Node->Value) V(ValueArgs...);
				OutNode = Node;
				return false;
			}
			else if (Node->Next == nullptr)
			{
				// add node to the end of the chain
				DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
				new (address) DMapEntry<K, V>(InKey, std::move(ValueArgs)...);
				Node->Next = address;
				OutNode = Node->Next;
				return true;
			}
			else Node = Node->Next;  // travel further down the chain
		}
		OutNode = nullptr;
		return false;  // to make the compiler happy
	}

public:

	/*___________________________

	MAP MANIPULATION
	___________________________*/

	/* returns true, if this is a new key */
	bool Add(const K& Key, const V& Value)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChain(occupiedNode, Key, Value);
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(Key, Value);
			this->Array[index] = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return shouldRehash;
	}


	/* returns true, if this is a new key */
	bool Add(K&& Key, V&& Value)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChain(occupiedNode, std::move(Key), std::move(Value));
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(std::move(Key), std::move(Value));
			this->Array[index] = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return shouldRehash;
	}


	/* returns true, if this is a new key */
	bool Add(K&& Key, const V& Value)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChain(occupiedNode, std::move(Key), Value);
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(std::move(Key), Value);
			this->Array[index] = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return shouldRehash;
	}


	/* returns true, if this is a new key */
	bool Add(const K& Key, V&& Value)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChain(occupiedNode, Key, std::move(Value));
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(Key, std::move(Value));
			this->Array[index] = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return shouldRehash;
	}


	V& AddDefault(const K& Key)
	{
		return this->Emplace(Key)->Value;
	}


	V& AddDefault(K&& Key)
	{
		return this->Emplace(std::move(Key))->Value;
	}


	template<typename... ConstructorArgs>
	DMapEntry<K, V>* Emplace(const K& Key, ConstructorArgs&&... ValueArgs)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		DMapEntry<K, V>* modifiedNode;

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChainConstruct(occupiedNode, /*out*/modifiedNode, Key, std::move(ValueArgs)...);
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(Key, std::move(ValueArgs)...);
			this->Array[index] = modifiedNode = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return modifiedNode;
	}


	template<typename... ConstructorArgs>
	DMapEntry<K, V>* Emplace(K&& Key, ConstructorArgs&&... ValueArgs)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity == 0)
		{
			this->Reserve(INITIAL_CAPACITY);
		}

		DMapEntry<K, V>* modifiedNode;

		cint index = std::hash<K>()(Key) % this->ArrayCapacity;
		DMapEntry<K, V>* occupiedNode = this->Array[index];

		bool shouldRehash;

		if (occupiedNode)
		{
			shouldRehash = this->AddNodeToChainConstruct(occupiedNode, /*out*/modifiedNode, std::move(Key), std::move(ValueArgs)...);
		}
		else
		{
			DMapEntry<K, V>* address = AllocatorT::INSTANCE.AllocateNode();
			new (address) DMapEntry<K, V>(std::move(Key), std::move(ValueArgs)...);
			this->Array[index] = modifiedNode = address;
			shouldRehash = true;
		}

		if (shouldRehash)
		{
			if (++this->Size / this->ArrayCapacity >= this->LoadFactor)
			{
				this->Rehash(this->ArrayCapacity * 2);
			}
		}

		return modifiedNode;
	}

private:

	FORCEINLINE V* GetPtrToValueOrNull(const K& Key) const
	{
		if (this->Array)
		{
			DMapEntry<K, V>* node = this->Array[std::hash<K>()(Key) % this->ArrayCapacity];
			while (node)
			{
				if (node->Key == Key)
				{
					return &node->Value;
				}
				else
				{
					node = node->Next;
				}
			}
		}
		return nullptr;
	}


	FORCEINLINE V GetCopyOfValueOrDefault(const K& Key) const
	{
		if (this->Array)
		{
			DMapEntry<K, V>* node = this->Array[std::hash<K>()(Key) % this->ArrayCapacity];
			while (node)
			{
				if (node->Key == Key)
				{
					return node->Value;
				}
				else
				{
					node = node->Next;
				}
			}
		}
		return V();
	}

public:


	V* Find(const K& Key)
	{
		return this->GetPtrToValueOrNull(Key);
	}


	const V* Find(const K& Key) const
	{
		return this->GetPtrToValueOrNull(Key);
	}


	/* if map values are pointers, use 'FindPtr' instead of 'Find' to avoid pointer to pointer return value */
	V FindPtr(const K& Key)
	{
		return this->GetCopyOfValueOrDefault(Key);
	}


	/* if map values are pointers, use 'FindPtr' instead of 'Find' to avoid pointer to pointer return value */
	const V FindPtr(const K& Key) const
	{
		return this->GetCopyOfValueOrDefault(Key);
	}


	bool RemoveGet(const K& Key, V& OutValue)
	{
		if (this->Array)
		{
			cint hash = std::hash<K>()(Key) % this->ArrayCapacity;
			DMapEntry<K, V>* node = this->Array[hash];
			while (node)
			{
				if (node->Key == Key)
				{
					OutValue = std::move(node->Value);
					return this->Remove(Key, hash);
				}
				else
				{
					node = node->Next;
				}
			}
		}
		OutValue = V();
		return false;
	}


	bool ContainsKey(const K& Key) const
	{
		return this->GetPtrToValueOrNull(Key);
	}


	bool ContainsValue(const V& Value) const
	{
		for (auto& entry : *this)
		{
			if (entry.GetValue() == Value)
			{
				return true;
			}
		}
		return false;
	}


private:

	/* 
	if Hash is -1, it is calculated from Key
	returns true, if the key was found and removed; 
	returns false, if nothing was removed
	*/
	bool Remove(const K& Key, cint Hash)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		if (this->ArrayCapacity > 0)
		{
			if (Hash == -1)
			{
				Hash = std::hash<K>()(Key) % this->ArrayCapacity;
			}
			if (Hash >= 0 && Hash < this->ArrayCapacity)
			{
				DMapEntry<K, V>* node = this->Array[Hash];
				if (node)
				{
					DMapEntry<K, V>* prevNode = nullptr;
					while (node != nullptr)
					{
						// key found
						if (node->Key == Key)
						{
							// key found in the array
							if (prevNode == nullptr)
							{
								if (node->Next)
								{
									// replace node's array slot with the next node in the chain
									this->Array[Hash] = node->Next;
								}
								else
								{
									// no next node, null node's array slot
									this->Array[Hash] = nullptr;
								}
							}
							// key found in a chain
							else
							{
								// remove node from a chain
								prevNode->Next = node->Next;
							}
							node->~DMapEntry();
							AllocatorT::INSTANCE.DeallocateNode(node);
							this->Size--;
							return true;
						}
						// key not found, travel further down the chain
						else
						{
							prevNode = node;
							node = node->Next;
						}
					}
				}
			}
		}
		return false;
	}

public:

	/* returns true, if the key was found and removed; 
	returns false, if nothing was removed */
	bool Remove(const K& Key)
	{
		return this->Remove(Key, -1);
	}


	/* returns true, if the key was found and removed; 
	returns false, if nothing was removed */
	bool RemoveValue(const V& Value)
	{
		RemovingEnumerator<K, V> etor (*this);
		while (DMapEntry<K, V>* node = etor.Next())
		{
			if (node->GetValue() == Value)
			{
				etor.Remove();
				return true;
			}
		}
		return false;
	}

public:

	void Clear()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->ClearBuffer();
		this->Size = 0;
	}


	FORCEINLINE cint Length() const
	{
		return this->Size;
	}
	

	bool IsEmpty() const
	{
		return this->Size < 1;
	}


	cint Capacity() const
	{
		return this->ArrayCapacity;
	}

private:

	void _Reserve(cint NewCapacity)
	{
		if (NewCapacity / this->LoadFactor > this->ArrayCapacity)
		{
			this->Rehash(NewCapacity);
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


	/* reduce storage to number of items */
	void TrimCapacity()
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		this->Rehash(this->Size);
	}


	/*
	Pred: (const K& Key, V& Value) -> bool
	if Pred returns true, key - value pair is removed from the map
	*/
	template<CALLABLE(PredicateT, bool(const K&, V&))>
	void RemoveIf(PredicateT&& Pred)
	{
#ifdef LANG_DEBUG_CONTAINERS
		++this->ModCount;
#endif
		RemovingEnumerator<K, V> etor (*this);
		while (DMapEntry<K, V>* node = etor.Next())
		{
			if (Pred(node->GetKey(), node->GetValue()))
			{
				etor.Remove();
			}
		}
	}

private:

	/* 
	Only sets out params, if a next node is found
	Returns false, if no next node is found
	*/
	FORCEINLINE bool NextNode(DMapEntry<K, V>* PrevNode, cint PrevHash, DMapEntry<K, V>*& OutNode, cint& OutHash) const
	{
		if (PrevNode && PrevNode->Next)
		{
			OutNode = PrevNode->Next;
			OutHash = PrevHash;
			return true;
		}
		else
		{
			for (cint i = PrevHash + 1; i < this->ArrayCapacity; ++i)
			{
				DMapEntry<K, V>* node = this->Array[i];
				if (node)
				{
					OutNode = node;
					OutHash = i;
					return true;
				}
			}
		}
		return false;
	}

public:

	Iterator<K, V> begin()
	{
		return Iterator<K, V>(*this, /*SeekBegin*/true);
	}


	ConstIterator<K, V> begin() const
	{
		return ConstIterator<K, V>(*this, /*SeekBegin*/true);
	}


	Iterator<K, V> end()
	{
		return Iterator<K, V>(*this, /*SeekBegin*/false);
	}


	ConstIterator<K, V> end() const
	{
		return ConstIterator<K, V>(*this, /*SeekBegin*/false);
	}


	Enumerator<K, V> GetEnumerator()
	{
		return Enumerator<K, V>(*this);
	}


	ConstEnumerator<K, V> GetEnumerator() const
	{
		return ConstEnumerator<K, V>(*this);
	}


	ConstEnumerator<K, V> GetConstEnumerator() const
	{
		return ConstEnumerator<K, V>(*this);
	}


	RemovingEnumerator<K, V> GetRemovingEnumerator()
	{
		return RemovingEnumerator<K, V>(*this);
	}

};


template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
using DMapIterator = typename DMap<K, V, AllocatorT>::template Iterator<K, V>;

template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
using DMapConstIterator = typename DMap<K, V, AllocatorT>::template ConstIterator<K, V>;

template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
using DMapEnumerator = typename DMap<K, V, AllocatorT>::template Enumerator<K, V>;

template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
using DMapConstEnumerator = typename DMap<K, V, AllocatorT>::template ConstEnumerator<K, V>;

template<typename K, typename V, class AllocatorT = DNodeArrayAllocator<DMapEntry<K, V>>>
using DMapRemovingEnumerator = typename DMap<K, V, AllocatorT>::template RemovingEnumerator<K, V>;
