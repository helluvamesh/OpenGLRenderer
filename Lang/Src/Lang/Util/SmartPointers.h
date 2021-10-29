/*_______________________________________________________________________________________

- Shared/Weak/Unique/Strong smart pointers for automated memory management
using reference counting

- Unlike an std::shared_ptr in the standard library, creating/destroying a DSharedPtr does not have a high overhead,
so passing them around by value is significantly faster,
still, performance critical functions should receive raw C pointers instead of smart pointers.
Constructing a DStrongPtr is ca. 2.5x slower then constructing a DSharedPtr (because it uses virtual methods).

- Unlike std::make_shared<T>(constructor_args) in STL, MakeShared<T>(ConstructorArgs)
uses separate heap allocations for the managed object and the reference counter object
to prevent existing weak pointers from not releasing memory after the object is destructed.
The downside is that now you have two heap allocations (slower to create)

- Objects managed by DStrongPtrs can't have DWeakPtrs pointed to them,
therefore they can be embedded in the reference counter object sparing an extra heap allocation

- Caveats:
If there's a cycle in shared pointers, those objects will never be deleted (memory leak!).
The cycle needs to be broken by 'weak' pointers.
Example A:
- A references B with a shared pointer
- B references A with a shared pointer
- Solution: One of them must be a weak pointer
Example B:
- A references B with a shared pointer
- B references C with a shared pointer
- C references A with a shared pointer
- Solution: Again, one of them must be a weak pointer

- Objects managed by strong pointers can't have weak pointers to them so their use cases are limited.
Only use them, if the extra heap allocation that a shared pointer entails is not acceptable.

- Having a unique pointer as a member makes the class - and all the classes its a member of - not copy constructible
so never use them as a member of a DStruct!
They should only be used as a member of classes managed by pointers like HObject.

_______________________________________________________________________________________*/


#pragma once


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

REFERENCE COUNTER	

--------------------------------------------------------
--------------------------------------------------------
----------------------------------------------------- */


class __TemplateTestObject { };


template<class T>
class DObjectAllocator
{
public:

	static DObjectAllocator<T> INSTANCE;

	static T* Allocate()
	{
		return (T*)malloc(sizeof(T));
	}

	static void Deallocate(T* Obj)
	{
		free(Obj);
	}
};


template<class T>
class DReferenceCounterAllocator
{
public:

	static DReferenceCounterAllocator<T> INSTANCE;

	static T* Allocate()
	{
		return (T*)malloc(sizeof(T));
	}

	static void Deallocate(T* Obj)
	{
		free(Obj);
	}
};


template<class T>
DObjectAllocator<T> DObjectAllocator<T>::INSTANCE;

template<class T>
DReferenceCounterAllocator<T> DReferenceCounterAllocator<T>::INSTANCE;


template<class T>
class DReferenceCounter
{
public:
	T* Obj;
	int32 RefCount = 1;
	int32 WeakRefCount = 1;

	DReferenceCounter(T* InObj) : 
		Obj(InObj)
	{
	}

	void ReleaseObj()
	{
		this->Obj->~T();
		DObjectAllocator<T>::INSTANCE.Deallocate(this->Obj);
		this->Obj = nullptr;
	}

	FORCEINLINE void IncrementRefCount()
	{
		++this->RefCount;
	}

	FORCEINLINE void IncrementWeakRefCount()
	{
		++this->WeakRefCount;
	}

	FORCEINLINE void DecrementRefCount(bool& OutDeletedResource, bool& OutDeletedCounter)
	{
		if (--this->RefCount == 0)
		{
			this->ReleaseObj();
			this->DecrementWeakRefCount(OutDeletedCounter);
			OutDeletedResource = true;
			return;
		}
		OutDeletedResource = OutDeletedCounter = false;
	}

	FORCEINLINE void DecrementWeakRefCount(bool& OutDeletedCounter)
	{
		if (--this->WeakRefCount == 0)
		{
			this->~DReferenceCounter();
			DObjectAllocator<DReferenceCounter<T>>::Deallocate(this);
			OutDeletedCounter = true;
			return;
		}
		OutDeletedCounter = false;
	}
};


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

SHARED POINTER

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */


template<class T>
class DWeakPtr;


template<class T>
class DSharedPtr
{
	/*_____________________________

	STATE
	_____________________________*/

private:

	T* Pointer;
	DReferenceCounter<T>* Counter;

	FORCEINLINE void IncrementRefCount()
	{
		if (this->Counter)
		{
			this->Counter->IncrementRefCount();
		}
	}

	FORCEINLINE void DecrementRefCount()
	{
		if (this->Counter)
		{
			bool deletedResource;
			bool deletedCounter;
			this->Counter->DecrementRefCount(/*out*/ deletedResource, /*out*/ deletedCounter);
			if (deletedResource)
			{
				this->Pointer = nullptr;
			}
			if (deletedCounter)
			{
				this->Counter = nullptr;
			}
		}
	}

	/*_____________________________

	CONSTRUCT
	_____________________________*/

private:

	DSharedPtr(DReferenceCounter<T>* InCounter) : 
		Pointer(InCounter->Obj),
		Counter(InCounter)
	{
	}

	/* INCREMENTS REF COUNT! */
	DSharedPtr(DReferenceCounter<T>* InCounter, T* InPointer) : 
		Pointer(InPointer),
		Counter(InCounter)
	{
		this->IncrementRefCount();
	}

public:

	DSharedPtr() : 
		Pointer(nullptr), 
		Counter(nullptr)
	{
	}

	DSharedPtr(std::nullptr_t) : 
		Pointer(nullptr), 
		Counter(nullptr)
	{
	}

	DSharedPtr(const DWeakPtr<T>& InDWeakPtr)
	{
		if (InDWeakPtr.Counter && InDWeakPtr.Counter->RefCount > 0)
		{
			this->Pointer = InDWeakPtr.Pointer; 
			this->Counter = InDWeakPtr.Counter;
			this->IncrementRefCount();
		}
		else
		{
			this->Pointer = nullptr;
			this->Counter = nullptr;
		}
	}

	template<SUBTYPE(SubT, T)>
	DSharedPtr(const DWeakPtr<SubT>& InDWeakPtr) : 
		Pointer(InDWeakPtr.Pointer), 
		Counter(reinterpret_cast<DReferenceCounter<T>*>(InDWeakPtr.Counter))
	{
		if (InDWeakPtr.Counter && InDWeakPtr.Counter->RefCount)
		{
			this->Pointer = InDWeakPtr.Pointer; 
			this->Counter = InDWeakPtr.Counter;
			this->IncrementRefCount();
		}
		else
		{
			this->Pointer = nullptr;
			this->Counter = nullptr;
		}
	}

	/*_____________________________

	COPY
	_____________________________*/

	DSharedPtr(const DSharedPtr<T>& Other)
	{
		this->Pointer = Other.Pointer;
		this->Counter = Other.Counter;
		this->IncrementRefCount();
	}

	DSharedPtr<T>& operator=(const DSharedPtr<T>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Pointer = Other.Pointer;
			this->Counter = Other.Counter;
			this->IncrementRefCount();
		}
		return *this;
	}

	template<SUBTYPE(SubT, T)>
	DSharedPtr(const DSharedPtr<SubT>& Other)
	{
		this->Pointer = static_cast<T*>(Other.Pointer);
		this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		this->IncrementRefCount();
	}

	template<SUBTYPE(SubT, T)>
	DSharedPtr<T>& operator=(const DSharedPtr<SubT>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Pointer = static_cast<T*>(Other.Pointer);
			this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
			this->IncrementRefCount();
		}
		return *this;
	}

	/*_____________________________

	MOVE
	_____________________________*/

	template<SUBTYPE(SubT, T)>
	DSharedPtr(DSharedPtr<SubT>&& Other)
	{
		this->Pointer = static_cast<T*>(Other.Pointer);
		this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
	}

	template<SUBTYPE(SubT, T)>
	DSharedPtr<T>& operator=(DSharedPtr<SubT>&& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Pointer = static_cast<T*>(Other.Pointer);
			this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		}
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
		return *this;
	}

	/*_____________________________

	DESTRUCT
	_____________________________*/

	~DSharedPtr()
	{
		this->DecrementRefCount();
	}

	/*_____________________________

	GETTERS
	_____________________________*/

	template<class OtherT>
	bool operator==(const DSharedPtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	template<class OtherT>
	bool operator==(const DWeakPtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	bool operator==(std::nullptr_t) const
	{
		return this->Pointer == nullptr;
	}

	template<class OtherT>
	bool operator!=(const DSharedPtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	template<class OtherT>
	bool operator!=(const DWeakPtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	bool operator!=(std::nullptr_t) const
	{
		return this->Pointer != nullptr;
	}

	T* Ptr() const
	{
		return this->Pointer;
	}

	T& operator*() const
	{
		return *(this->Pointer);
	}

	T* operator->() const
	{
		return this->Pointer;
	}

	/* returns number of shared pointers */
	int32 RefCount() const
	{
		return this->Counter? this->Counter->RefCount : 0;
	}

	explicit operator bool() const
	{
		return this->Pointer;
	}

	/*_____________________________

	SETTERS
	_____________________________*/

	DSharedPtr<T>& operator=(std::nullptr_t)
	{
		this->DecrementRefCount();
		this->Pointer = nullptr;
		this->Counter = nullptr;
		return *this;
	}

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DSharedPtr<SubT> DynamicCast(const DSharedPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DSharedPtr<SubT> StaticCast(const DSharedPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DSharedPtr<SubT> ConstCast(const DSharedPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DSharedPtr<SubT> ReinterpretCast(const DSharedPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend bool Instanceof(const DSharedPtr<BaseT>& Other);

	template<typename T2, typename... ConstructorArgs>
	friend DSharedPtr<T2> MakeShared(ConstructorArgs&&... Args);

	template<typename T2>
	friend class DSharedPtr;

	template<typename T2>
	friend class DWeakPtr;
};


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

WEAK POINTER

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */


template<class T>
class DWeakPtr
{

	/*_____________________________

	STATE
	_____________________________*/

private:

	T* Pointer;
	DReferenceCounter<T>* Counter;

	FORCEINLINE void IncrementWeakRefCount()
	{
		if (this->Counter)
		{
			this->Counter->IncrementWeakRefCount();
		}
	}

	FORCEINLINE void DecrementWeakRefCount()
	{
		if (this->Counter)
		{
			bool deletedCounter;
			this->Counter->DecrementWeakRefCount(/*out*/ deletedCounter);
			if (deletedCounter)
			{
				this->Counter = nullptr;
				this->Pointer = nullptr;
			}
		}
	}

	/*_____________________________

	CONSTRUCT
	_____________________________*/

public:

	DWeakPtr() :
		Pointer(nullptr), 
		Counter(nullptr)
	{
	}

	DWeakPtr(std::nullptr_t) : 
		Pointer(nullptr), 
		Counter(nullptr)
	{
	}

	DWeakPtr(const DSharedPtr<T>& InDSharedPtr) :
		Pointer(InDSharedPtr.Pointer),
		Counter(InDSharedPtr.Counter)
	{
		this->IncrementWeakRefCount();
	}

	template<SUBTYPE(SubT, T)>
	DWeakPtr(const DSharedPtr<SubT>& InDSharedPtr) : 
		Pointer(InDSharedPtr.Pointer), 
		Counter(reinterpret_cast<DReferenceCounter<T>*>(InDSharedPtr.Counter))
	{
		this->IncrementWeakRefCount();
	}

private:

	/* INCREMENTS REF COUNT! */
	DWeakPtr(DReferenceCounter<T>* InCounter, T* InPtr) : 
		Pointer(InPtr), 
		Counter(InCounter)
	{
		this->IncrementWeakRefCount();
	}

public:

	/*_____________________________

	COPY
	_____________________________*/

	DWeakPtr(const DWeakPtr<T>& Other)
	{
		this->Pointer = Other.Pointer;
		this->Counter = Other.Counter;
		this->IncrementWeakRefCount();
	}

	DWeakPtr<T>& operator=(const DWeakPtr<T>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementWeakRefCount();
			this->Pointer = Other.Pointer;
			this->Counter = Other.Counter;
			this->IncrementWeakRefCount();
		}
		return *this;
	}

	template<SUBTYPE(SubT, T)>
	DWeakPtr(const DWeakPtr<SubT>& Other)
	{
		this->Pointer = static_cast<T*>(Other.Pointer);
		this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		this->IncrementWeakRefCount();
	}

	template<SUBTYPE(SubT, T)>
	DWeakPtr<T>& operator=(const DWeakPtr<SubT>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementWeakRefCount();
			this->Pointer = static_cast<T*>(Other.Pointer);
			this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
			this->IncrementWeakRefCount();
		}
		return *this;
	}

	/*_____________________________

	MOVE
	_____________________________*/

	template<SUBTYPE(SubT, T)>
	DWeakPtr(DWeakPtr<SubT>&& Other)
	{
		this->Pointer = static_cast<T*>(Other.Pointer);
		this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
	}

	template<SUBTYPE(SubT, T)>
	DWeakPtr<T>& operator=(DWeakPtr<SubT>&& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementWeakRefCount();
			this->Pointer = static_cast<T*>(Other.Pointer);
			this->Counter = reinterpret_cast<DReferenceCounter<T>*>(Other.Counter);
		}
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
		return *this;
	}

	/*_____________________________

	DESTRUCT
	_____________________________*/

	~DWeakPtr()
	{
		this->DecrementWeakRefCount();
	}

	/*_____________________________

	GETTERS
	_____________________________*/

	template<class OtherT>
	bool operator==(const DWeakPtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	template<class OtherT>
	bool operator==(const DSharedPtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	bool operator==(std::nullptr_t) const
	{
		return this->Counter? this->Counter->RefCount <= 0 : true; 
	}

	template<class OtherT>
	bool operator!=(const DWeakPtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	template<class OtherT>
	bool operator!=(const DSharedPtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	bool operator!=(std::nullptr_t) const
	{
		return this->Counter? this->Counter->RefCount > 0 : false; 
	}

	explicit operator bool() const
	{
		return this->Counter? this->Counter->RefCount > 0 : false;
	}

	/* returns number of shared pointers */
	int32 RefCount() const
	{
		return this->Counter? this->Counter->RefCount : 0;
	}

	/*_____________________________

	SETTERS
	_____________________________*/

	DWeakPtr<T>& operator=(std::nullptr_t)
	{
		this->DecrementWeakRefCount();
		this->Pointer = nullptr;
		this->Counter = nullptr;
		return *this;
	}


	template<typename T2>
	friend class DWeakPtr;

	template<typename T2>
	friend class DSharedPtr;


	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DWeakPtr<SubT> StaticCast(const DWeakPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DWeakPtr<SubT> ConstCast(const DWeakPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DWeakPtr<SubT> ReinterpretCast(const DWeakPtr<BaseT>& Other);

};


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

CASTING

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */


template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DSharedPtr<SubT> DynamicCast(const DSharedPtr<BaseT>& Other)
{
	SubT* ptr = dynamic_cast<SubT*>(Other.Pointer);
	if (ptr)
	{
		return DSharedPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), ptr);
	}
	return DSharedPtr<SubT>(nullptr);
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DSharedPtr<SubT> StaticCast(const DSharedPtr<BaseT>& Other)
{
	return DSharedPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), static_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DSharedPtr<SubT> ConstCast(const DSharedPtr<BaseT>& Other)
{
	return DSharedPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), const_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DSharedPtr<SubT> ReinterpretCast(const DSharedPtr<BaseT>& Other)
{
	return DSharedPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), reinterpret_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
bool Instanceof(const DSharedPtr<BaseT>& Other)
{
	return dynamic_cast<SubT*>(Other.Pointer);
}


template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DWeakPtr<SubT> StaticCast(const DWeakPtr<BaseT>& Other)
{
	return DWeakPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), static_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DWeakPtr<SubT> ConstCast(const DWeakPtr<BaseT>& Other)
{
	return DWeakPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), const_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DWeakPtr<SubT> ReinterpretCast(const DWeakPtr<BaseT>& Other)
{
	return DWeakPtr<SubT>(reinterpret_cast<DReferenceCounter<SubT>*>(Other.Counter), reinterpret_cast<SubT*>(Other.Pointer));
}


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

SHARED FROM THIS

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */


template<class T>
class DSharedFromThis
{
private:
	DWeakPtr<T> _DWeakPtr;

public:
	DSharedPtr<T> SharedThis() const
	{
		return this->_DWeakPtr;
	}

	DWeakPtr<T> WeakThis() const
	{
		return this->_DWeakPtr;
	}

	template<SUBTYPE(SubT, T)>
	DSharedPtr<SubT> SharedThisAs() const
	{
		return StaticCast<SubT>(DSharedPtr<T>(this->_DWeakPtr));
	}

	template<SUBTYPE(SubT, T)>
	DWeakPtr<SubT> WeakThisAs() const
	{
		return StaticCast<SubT>(this->_DWeakPtr);
	}

	template<typename T2, typename... ConstructorArgs>
	friend DSharedPtr<T2> MakeShared(ConstructorArgs&&... Args);
};


template <class T, class = void>
struct InheritsFromSharedFromThis : std::false_type {};
template<class T>
struct InheritsFromSharedFromThis<T, std::void_t<typename T::DSharedFromThis>>
	: std::is_convertible<std::remove_cv_t<T>*, typename T::DSharedFromThis*>::type {};


template<class T, typename... ConstructorArgs>
DSharedPtr<T> MakeShared(ConstructorArgs&&... Args)
{
	if constexpr (InheritsFromSharedFromThis<T>::value)
	{
		T* obj = DObjectAllocator<T>::INSTANCE.Allocate();
		new (obj) T(std::forward<ConstructorArgs>(Args)...);
		DReferenceCounter<T>* counter = DReferenceCounterAllocator<DReferenceCounter<T>>::Allocate();
		new (counter) DReferenceCounter<T>(obj);
		DSharedPtr<T> ret (counter);
		counter->Obj->_DWeakPtr = ret;
		return ret;
	}
	else
	{
		T* obj = DObjectAllocator<T>::INSTANCE.Allocate();
		new (obj) T(std::forward<ConstructorArgs>(Args)...); 
		return DSharedPtr<T>(new DReferenceCounter<T>(obj));
	}
}


namespace std 
{
	template<class T> 
	struct hash<DSharedPtr<T>>
	{
		size_t operator()(const DSharedPtr<T>& Key) const
		{
			return hash<void*>()(Key.Ptr());
		}
	};
}


/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

UNIQUE POINTER

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */


template<class T>
class DUniquePtr
{
	/*_____________________________

	STATE
	_____________________________*/

private:

	T* Pointer;

	FORCEINLINE void ReleasePointer()
	{
		if (this->Pointer)
		{
			this->Pointer->~T();
			DObjectAllocator<T>::INSTANCE.Deallocate(this->Pointer);
			this->Pointer = nullptr;
		}
	}

	/*_____________________________

	CONSTRUCT
	_____________________________*/

private:

	DUniquePtr(T* InPointer) :
		Pointer(InPointer)
	{
	}

public:

	DUniquePtr() : 
		Pointer(nullptr)
	{
	}

	DUniquePtr(std::nullptr_t) : 
		Pointer(nullptr)
	{
	}

	/*_____________________________

	COPY
	_____________________________*/

	DUniquePtr(const DUniquePtr<T>& Other) = delete;
	DUniquePtr<T>& operator=(const DUniquePtr<T>& Other) = delete;

	/*_____________________________

	MOVE
	_____________________________*/

	template<SUBTYPE(SubT, T)>
	DUniquePtr(DUniquePtr<SubT>&& Other)
	{
		this->Pointer = static_cast<T*>(Other.Pointer);
		Other.Pointer = nullptr;
	}

	template<SUBTYPE(SubT, T)>
	DUniquePtr<T>& operator=(DUniquePtr<SubT>&& Other)
	{
		this->ReleasePointer();
		this->Pointer = static_cast<T*>(Other.Pointer);
		Other.Pointer = nullptr;
		return *this;
	}

	/*_____________________________

	DESTRUCT
	_____________________________*/

	~DUniquePtr()
	{
		this->ReleasePointer();
	}

	/*_____________________________

	GETTERS
	_____________________________*/

	template<class OtherT>
	bool operator==(const DUniquePtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	bool operator==(std::nullptr_t) const
	{
		return this->Pointer == nullptr;
	}

	template<class OtherT>
	bool operator!=(const DUniquePtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	bool operator!=(std::nullptr_t) const
	{
		return this->Pointer != nullptr;
	}

	T* Ptr() const
	{
		return this->Pointer;
	}

	T& operator*() const
	{
		return *(this->Pointer);
	}

	T* operator->() const
	{
		return this->Pointer;
	}

	explicit operator bool() const
	{
		return this->Pointer;
	}

	/*_____________________________

	SETTERS
	_____________________________*/

	DUniquePtr<T>& operator=(std::nullptr_t)
	{
		this->ReleasePointer();
		this->Pointer = nullptr;
		return *this;
	}

	template<class T2, typename... ConstructorArgs>
	friend DUniquePtr<T2> MakeUnique(ConstructorArgs&&... Args);

	template<typename T2>
	friend class DUniquePtr;
};


template<class T, typename... ConstructorArgs>
DUniquePtr<T> MakeUnique(ConstructorArgs&&... Args)
{
	T* obj = DObjectAllocator<T>::INSTANCE.Allocate();
	new (obj) T(std::forward<ConstructorArgs>(Args)...); 
	return DUniquePtr<T>(obj);
}


namespace std 
{
	template<class T> 
	struct hash<DUniquePtr<T>>
	{
		size_t operator()(const DUniquePtr<T>& Key) const
		{
			return hash<void*>()(Key.Ptr());
		}
	};
}



/* ------------------------------------------------------
---------------------------------------------------------
---------------------------------------------------------

STRONG POINTER

--------------------------------------------------------
--------------------------------------------------------
------------------------------------------------------ */

class IStrongReferenceCounter
{
public:
	virtual ~IStrongReferenceCounter() = default;

	virtual void IncrementRefCount() = 0;
	virtual int32 DecrementRefCount() = 0;
	virtual int32 GetRefCount() = 0;
};


template<class T>
class DStrongReferenceCounter : public IStrongReferenceCounter
{
public:
	int32 RefCount = 0;
	T Obj;

	template<typename... ConstructorArgs>
	DStrongReferenceCounter(ConstructorArgs&&... Args) : 
		Obj(std::move(Args)...)
	{
	}

	void IncrementRefCount()
	{
		++this->RefCount;
	}

	int32 DecrementRefCount()
	{
		return --this->RefCount;
	}

	int32 GetRefCount()
	{
		return this->RefCount;
	}
};


template<typename T>
class DStrongPtr
{
	/*_____________________________

	STATE
	_____________________________*/

private:

	IStrongReferenceCounter* Counter;
	T* Pointer;

	FORCEINLINE void IncrementRefCount()
	{
		if (this->Counter)
		{
			this->Counter->IncrementRefCount();
		}
	}

	FORCEINLINE void DecrementRefCount()
	{
		if (this->Counter && this->Counter->DecrementRefCount() == 0)
		{
			delete this->Counter;
			this->Pointer = nullptr;
			this->Counter = nullptr;
		}
	}

	/*_____________________________

	CONSTRUCT
	_____________________________*/

private:

	DStrongPtr(IStrongReferenceCounter* InCounter, T* InPointer) : 
		Counter(InCounter),
		Pointer(InPointer)
	{
		this->IncrementRefCount();
	}

public:

	DStrongPtr() :
		Counter(nullptr),
		Pointer(nullptr)
	{
	}

	DStrongPtr(std::nullptr_t) :
		Counter(nullptr),
		Pointer(nullptr)
	{
	}

	/*_____________________________

	COPY
	_____________________________*/

	DStrongPtr(const DStrongPtr<T>& Other)
	{
		this->Counter = Other.Counter;
		this->Pointer = Other.Pointer;
		this->IncrementRefCount();
	}

	DStrongPtr<T>& operator=(const DStrongPtr<T>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Counter = Other.Counter;
			this->Pointer = Other.Pointer;
			this->IncrementRefCount();
		}
		return *this;
	}

	template<SUBTYPE(SubT, T)>
	DStrongPtr(const DStrongPtr<SubT>& Other)
	{
		this->Counter = Other.Counter;
		this->Pointer = static_cast<T*>(Other.Pointer);
		this->IncrementRefCount();
	}

	template<SUBTYPE(SubT, T)>
	DStrongPtr<T>& operator=(const DStrongPtr<SubT>& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Counter = Other.Counter;
			this->Pointer = static_cast<T*>(Other.Pointer);
			this->IncrementRefCount();
		}
		return *this;
	}

	/*_____________________________

	MOVE
	_____________________________*/

	template<SUBTYPE(SubT, T)>
	DStrongPtr(DStrongPtr<SubT>&& Other)
	{
		this->Counter = Other.Counter;
		this->Pointer = static_cast<T*>(Other.Pointer);
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
	}

	template<SUBTYPE(SubT, T)>
	DStrongPtr<T>& operator=(DStrongPtr<SubT>&& Other)
	{
		if (this->Pointer != Other.Pointer)
		{
			this->DecrementRefCount();
			this->Counter = Other.Counter;
			this->Pointer = static_cast<T*>(Other.Pointer);
		}
		Other.Pointer = nullptr;
		Other.Counter = nullptr;
		return *this;
	}

	/*_____________________________

	DESTRUCT
	_____________________________*/

	~DStrongPtr()
	{
		this->DecrementRefCount();
	}

	/*_____________________________

	GETTERS
	_____________________________*/

	template<class OtherT>
	bool operator==(const DStrongPtr<OtherT>& Other) const
	{
		return this->Pointer == Other.Pointer;
	}

	bool operator==(std::nullptr_t) const
	{
		return this->Pointer == nullptr;
	}

	template<class OtherT>
	bool operator!=(const DStrongPtr<OtherT>& Other) const
	{
		return this->Pointer != Other.Pointer;
	}

	bool operator!=(std::nullptr_t) const
	{
		return this->Pointer != nullptr;
	}

	T* Ptr() const
	{
		return this->Pointer;
	}

	T& operator*() const
	{
		return *(this->Pointer);
	}

	T* operator->() const
	{
		return this->Pointer;
	}

	/* returns number of shared pointers */
	int32 RefCount() const
	{
		return this->Counter? this->Counter->GetRefCount() : 0;
	}

	explicit operator bool() const
	{
		return this->Pointer;
	}

	/*_____________________________

	SETTERS
	_____________________________*/

	DStrongPtr<T>& operator=(std::nullptr_t)
	{
		this->DecrementRefCount();
		this->Counter = nullptr;
		this->Pointer = nullptr;
		return *this;
	}

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DStrongPtr<SubT> DynamicCast(const DStrongPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DStrongPtr<SubT> StaticCast(const DStrongPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DStrongPtr<SubT> ConstCast(const DStrongPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend DStrongPtr<SubT> ReinterpretCast(const DStrongPtr<BaseT>& Other);

	template<class SubT, class BaseT, EXTENDS_RD(SubT, BaseT)>
	friend bool Instanceof(const DStrongPtr<BaseT>& Other);

	template<class T2, typename... ConstructorArgs>
	friend DStrongPtr<T2> MakeStrong(ConstructorArgs&&... Args);

	template<typename T>
	friend class DStrongPtr;
};


template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DStrongPtr<SubT> DynamicCast(const DStrongPtr<BaseT>& Other)
{
	SubT* ptr = dynamic_cast<SubT*>(Other.Pointer);
	if (ptr)
	{
		return DStrongPtr<SubT>(Other.Counter, ptr);
	}
	return DStrongPtr<SubT>(nullptr);
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DStrongPtr<SubT> StaticCast(const DStrongPtr<BaseT>& Other)
{
	return DStrongPtr<SubT>(Other.Counter, static_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DStrongPtr<SubT> ConstCast(const DStrongPtr<BaseT>& Other)
{
	return DStrongPtr<SubT>(Other.Counter, const_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
DStrongPtr<SubT> ReinterpretCast(const DStrongPtr<BaseT>& Other)
{
	return DStrongPtr<SubT>(Other.Counter, reinterpret_cast<SubT*>(Other.Pointer));
}

template<class SubT, class BaseT, EXTENDS(SubT, BaseT)>
bool Instanceof(const DStrongPtr<BaseT>& Other)
{
	return dynamic_cast<SubT*>(Other.Pointer);
}

template<class T, typename... ConstructorArgs>
DStrongPtr<T> MakeStrong(ConstructorArgs&&... Args)
{
	auto* counter = DObjectAllocator<DStrongReferenceCounter<T>>::Allocate();
	new (counter) DStrongReferenceCounter<T>(std::forward<ConstructorArgs>(Args)...);
	return DStrongPtr<T>(counter, &counter->Obj);
}


namespace std 
{
	template<class T> 
	struct hash<DStrongPtr<T>>
	{
		size_t operator()(const DStrongPtr<T>& Key) const
		{
			return hash<void*>()(Key.Ptr());
		}
	};
}


template<class T>
using shared = DSharedPtr<T>;

template<class T>
using weak = DWeakPtr<T>;

template<class T>
using unique = DUniquePtr<T>;

template<class T>
using strong = DStrongPtr<T>;
