#pragma once


template<typename ReturnType, typename... ParamTypes>
class DSingleCastDelegate
{
private:

	static const uint64 INLINE_LAMBDA_BYTES = 56;

	template<typename ReturnType, typename... ParamTypes>
	class ILambdaCaller
	{
	public:
		virtual ~ILambdaCaller() = default;

		virtual ReturnType Call(ParamTypes... Arg) = 0;
	};


	template<typename LambdaT, typename ReturnType, typename... ParamTypes>
	class LambdaWrapper : public ILambdaCaller<ReturnType, ParamTypes...>
	{
	private:
		LambdaT Func;

	public:
		LambdaWrapper(LambdaT&& InFunc) : 
			Func(std::move(InFunc))
		{
		}

		ReturnType Call(ParamTypes... Arg) override
		{
			return this->Func(std::forward<ParamTypes>(Arg)...);
		}
	};

	enum class EStorageType : char { NONE, METHOD, INLINE_LAMBDA, HEAP_LAMBDA };

	struct MethodT
	{
		DWeakPtr<HObject> OwnerPtr;
		ReturnType (HObject::*MethodPtr)(ParamTypes...);

		MethodT() :
			OwnerPtr({}),
			MethodPtr(nullptr)
		{
		}
	};

	union StorageUnion
	{	
		MethodT Method;
		char Lambda[INLINE_LAMBDA_BYTES];
		DStrongPtr<ILambdaCaller<ReturnType, ParamTypes...>> HeapLambda;

		StorageUnion() :
			Method({})
		{
		}

		~StorageUnion() 
		{
		}
	};

	EStorageType StorageType;
	StorageUnion Storage;

public:

	DSingleCastDelegate() : 
		StorageType(EStorageType::NONE)
	{
	}

	~DSingleCastDelegate()
	{
		this->ResetStorage();
	}

private:

	inline void ResetStorage()
	{
		switch (this->StorageType)
		{
		case EStorageType::METHOD:
		{
			this->Storage.Method.OwnerPtr = nullptr;
			this->Storage.Method.MethodPtr = nullptr;
			break;
		}
		case EStorageType::HEAP_LAMBDA:
		{
			this->Storage.HeapLambda = nullptr;
			break;
		}
		}
	}

	inline void Copy(const DSingleCastDelegate& Other)
	{
		this->StorageType = Other.StorageType;
		switch (this->StorageType)
		{
		case EStorageType::METHOD:
		{
			this->Storage.Method = Other.Storage.Method;
			break;
		}
		case EStorageType::HEAP_LAMBDA:
		{
			this->Storage.HeapLambda = Other.Storage.HeapLambda;
			break;
		}
		case EStorageType::INLINE_LAMBDA:
		{
			for (cint i = 0; i < INLINE_LAMBDA_BYTES; i++)
			{
				this->Storage.Lambda[i] = Other.Storage.Lambda[i];
			}
			break;
		}
		}
	}

	inline void Move(DSingleCastDelegate&& Other)
	{
		this->StorageType = Other.StorageType;
		switch (this->StorageType)
		{
		case EStorageType::METHOD:
		{
			this->Storage.Method = std::move(Other.Storage.Method);
			break;
		}
		case EStorageType::HEAP_LAMBDA:
		{
			this->Storage.HeapLambda = std::move(Other.Storage.HeapLambda);
			break;
		}
		case EStorageType::INLINE_LAMBDA:
		{
			for (cint i = 0; i < INLINE_LAMBDA_BYTES; i++)
			{
				this->Storage.Lambda[i] = Other.Storage.Lambda[i];
			}
			break;
		}
		}
	}

public:

	DSingleCastDelegate(const DSingleCastDelegate& Other)
	{
		this->Copy(Other);
	}

	DSingleCastDelegate& operator=(const DSingleCastDelegate& Other)
	{
		this->ResetStorage();
		this->Copy(Other);
		return *this;
	}

	DSingleCastDelegate(DSingleCastDelegate&& Other) noexcept
	{
		this->Move(std::move(Other));
	}

	DSingleCastDelegate& operator=(DSingleCastDelegate&& Other) noexcept
	{
		this->ResetStorage();
		this->Move(std::move(Other));
		return *this;
	}

	template<SUBTYPE(OwnerType, HObject)>
	DSingleCastDelegate(DWeakPtr<OwnerType> InOwnerPtr, ReturnType (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->StorageType = EStorageType::METHOD;
		this->Storage.Method.OwnerPtr = InOwnerPtr;
		this->Storage.Method.MethodPtr = static_cast<ReturnType (HObject::*)(ParamTypes...)>(InMethodPtr);
	}

	template<SUBTYPE(OwnerType, HObject)>
	DSingleCastDelegate(DSharedPtr<OwnerType> InOwnerPtr, ReturnType (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->StorageType = EStorageType::METHOD;
		this->Storage.Method.OwnerPtr = InOwnerPtr;
		this->Storage.Method.MethodPtr = static_cast<ReturnType (HObject::*)(ParamTypes...)>(InMethodPtr);
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Bind(DWeakPtr<OwnerType> InOwnerPtr, ReturnType (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->ResetStorage();
		this->StorageType = EStorageType::METHOD;
		this->Storage.Method.OwnerPtr = InOwnerPtr;
		this->Storage.Method.MethodPtr = static_cast<ReturnType (HObject::*)(ParamTypes...)>(InMethodPtr);
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Bind(DSharedPtr<OwnerType> InOwnerPtr, ReturnType (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->ResetStorage();
		this->StorageType = EStorageType::METHOD;
		this->Storage.Method.OwnerPtr = InOwnerPtr;
		this->Storage.Method.MethodPtr = static_cast<ReturnType (HObject::*)(ParamTypes...)>(InMethodPtr);
	}

private:

	template<typename LambdaT>
	inline void BindLambda(LambdaT&& Func)
	{
		if constexpr (sizeof(LambdaT) < INLINE_LAMBDA_BYTES)
		{
			this->StorageType = EStorageType::INLINE_LAMBDA;
			new (this->Storage.Lambda) LambdaWrapper<LambdaT, ReturnType, ParamTypes...>(std::move(Func));
		}
		else
		{
			this->StorageType = EStorageType::HEAP_LAMBDA;
			this->Storage.HeapLambda = MakeStrong<LambdaWrapper<LambdaT, ReturnType, ParamTypes...>>(std::move(Func));
		}
	}

public:
	
	template<CALLABLE(LambdaT, ReturnType(ParamTypes...))>
	void Bind(LambdaT&& Func)
	{
		this->ResetStorage();
		this->BindLambda(std::move(Func));
	}

	template<CALLABLE(LambdaT, ReturnType(ParamTypes...))>
	DSingleCastDelegate(LambdaT&& Func)
	{
		this->BindLambda(std::move(Func));
	}

	template<CALLABLE(LambdaT, ReturnType(ParamTypes...))>
	DSingleCastDelegate& operator=(LambdaT&& Func)
	{
		this->ResetStorage();
		this->BindLambda(std::move(Func));
		return *this;
	}

	void Unbind()
	{
		this->ResetStorage();
		this->StorageType = EStorageType::NONE;
	}

	ReturnType Call(ParamTypes... Arg)
	{
		switch (this->StorageType)
		{
		case EStorageType::METHOD:
		{
			DSharedPtr<HObject> sptr = this->Storage.Method.OwnerPtr;
			HObject* owner = sptr.Ptr();
			if (owner)
			{
				return (owner->*(this->Storage.Method.MethodPtr))(std::forward<ParamTypes>(Arg)...);
			}
			else
			{
				this->ResetStorage();
				this->StorageType = EStorageType::NONE;
			}
			break;
		}
		case EStorageType::INLINE_LAMBDA:
		{
			return reinterpret_cast<ILambdaCaller<ReturnType, ParamTypes...>&>(this->Storage.Lambda).Call(std::forward<ParamTypes>(Arg)...);
		}
		case EStorageType::HEAP_LAMBDA:
		{
			return this->Storage.HeapLambda->Call(std::forward<ParamTypes>(Arg)...);
		}
		}

		return ReturnType();
	}

	bool IsBound() const
	{
		return this->StorageType != EStorageType::NONE;
	}
};


template<typename... ParamTypes>
class DMultiCastDelegate
{
private:

	template<typename... ParamTypes>
	struct Item
	{
		static const uint64 INLINE_LAMBDA_BYTES = 56;

		template<typename... ParamTypes>
		class ILambdaCaller
		{
		public:
			virtual ~ILambdaCaller() = default;

			virtual void Call(ParamTypes... Arg) = 0;
		};


		template<typename LambdaT, typename... ParamTypes>
		class LambdaWrapper : public ILambdaCaller<ParamTypes...>
		{
			LambdaT Func;

		public:
			LambdaWrapper(LambdaT&& InFunc) : 
				Func(std::move(InFunc))
			{
			}

			void Call(ParamTypes... Arg) override
			{
				this->Func(Arg...);
			}
		};

		enum class EStorageType : char { NONE, METHOD, INLINE_LAMBDA, HEAP_LAMBDA };

		struct MethodT
		{
			DWeakPtr<HObject> OwnerPtr;
			void (HObject::*MethodPtr)(ParamTypes...);

			MethodT() :
				OwnerPtr({}),
				MethodPtr(nullptr)
			{
			}
		};

		union StorageUnion
		{	
			MethodT Method;
			char Lambda[INLINE_LAMBDA_BYTES];
			DStrongPtr<ILambdaCaller<ParamTypes...>> HeapLambda;

			StorageUnion() : 
				Method({})
			{
			}

			~StorageUnion() 
			{
			}
		};

		EStorageType StorageType;
		StorageUnion Storage;
		bool Removed;

	public:

		Item() : 
			StorageType(EStorageType::NONE),
			Removed(false)
		{
		}

		~Item()
		{
			this->ResetStorage();
		}

	private:

		inline void ResetStorage()
		{
			switch (this->StorageType)
			{
			case EStorageType::METHOD:
			{
				this->Storage.Method.OwnerPtr = nullptr;
				this->Storage.Method.MethodPtr = nullptr;
				break;
			}
			case EStorageType::HEAP_LAMBDA:
			{
				this->Storage.HeapLambda = nullptr;
				break;
			}
			}
		}

		inline void Copy(const Item& Other)
		{
			this->StorageType = Other.StorageType;
			switch (this->StorageType)
			{
			case EStorageType::METHOD:
			{
				this->Storage.Method = Other.Storage.Method;
				break;
			}
			case EStorageType::HEAP_LAMBDA:
			{
				this->Storage.HeapLambda = Other.Storage.HeapLambda;
				break;
			}
			case EStorageType::INLINE_LAMBDA:
			{
				for (cint i = 0; i < INLINE_LAMBDA_BYTES; i++)
				{
					this->Storage.Lambda[i] = Other.Storage.Lambda[i];
				}
				break;
			}
			}
		}

		inline void Move(Item&& Other)
		{
			this->StorageType = Other.StorageType;
			switch (this->StorageType)
			{
			case EStorageType::METHOD:
			{
				this->Storage.Method = std::move(Other.Storage.Method);
				break;
			}
			case EStorageType::HEAP_LAMBDA:
			{
				this->Storage.HeapLambda = std::move(Other.Storage.HeapLambda);
				break;
			}
			case EStorageType::INLINE_LAMBDA:
			{
				for (cint i = 0; i < INLINE_LAMBDA_BYTES; i++)
				{
					this->Storage.Lambda[i] = Other.Storage.Lambda[i];
				}
				break;
			}
			}
		}

	public:

		Item(const Item& Other)
		{
			this->Copy(Other);
		}

		Item& operator=(const Item& Other)
		{
			this->ResetStorage();
			this->Copy(Other);
			return *this;
		}

		Item(Item&& Other) noexcept
		{
			this->Move(std::move(Other));
		}

		Item& operator=(Item&& Other) noexcept
		{
			this->ResetStorage();
			this->Move(std::move(Other));
			return *this;
		}

		template<SUBTYPE(OwnerType, HObject)>
		void Bind(DWeakPtr<OwnerType> InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
		{
			this->ResetStorage();
			this->StorageType = EStorageType::METHOD;
			this->Storage.Method.OwnerPtr = InOwnerPtr;
			this->Storage.Method.MethodPtr = static_cast<void (HObject::*)(ParamTypes...)>(InMethodPtr);
		}

		template<typename LambdaT>
		void Bind(LambdaT&& Func)
		{
			if constexpr (sizeof(LambdaT) < INLINE_LAMBDA_BYTES)
			{
				this->StorageType = EStorageType::INLINE_LAMBDA;
				new (this->Storage.Lambda) LambdaWrapper<LambdaT, ParamTypes...>(std::move(Func));
			}
			else
			{
				this->StorageType = EStorageType::HEAP_LAMBDA;
				this->Storage.HeapLambda = MakeStrong<LambdaWrapper<LambdaT, ParamTypes...>>(std::move(Func));
			}
		}

		void Unbind()
		{
			this->ResetStorage();
			this->StorageType = EStorageType::NONE;
		}

		void Call(ParamTypes... Arg)
		{
			switch (this->StorageType)
			{
			case EStorageType::METHOD:
			{
				DSharedPtr<HObject> sptr = this->Storage.Method.OwnerPtr;
				HObject* owner = sptr.Ptr();
				if (owner)
				{
					(owner->*(this->Storage.Method.MethodPtr))(std::forward<ParamTypes>(Arg)...);
					return;
				}
				else
				{
					this->ResetStorage();
					this->StorageType = EStorageType::NONE;
				}
				break;
			}
			case EStorageType::INLINE_LAMBDA:
			{
				reinterpret_cast<ILambdaCaller<ParamTypes...>&>(this->Storage.Lambda).Call(std::forward<ParamTypes>(Arg)...);
				return;
			}
			case EStorageType::HEAP_LAMBDA:
			{
				this->Storage.HeapLambda->Call(std::forward<ParamTypes>(Arg)...);
				return;
			}
			}
		}

		bool IsBound() const
		{
			return this->StorageType != EStorageType::NONE;
		}
	};


	DArray<Item<ParamTypes...>> Items;
	bool ShouldRemoveAll = false;

public:

	void Reserve(cint NewCapacity)
	{
		this->Items.Reserve(NewCapacity);
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Bind(DWeakPtr<OwnerType> InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		Item<ParamTypes...>& item = this->Items.AddDefault();
		item.Bind(InOwnerPtr, InMethodPtr);
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Bind(DSharedPtr<OwnerType> InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		Item<ParamTypes...>& item = this->Items.AddDefault();
		item.Bind((DWeakPtr<OwnerType>)InOwnerPtr, InMethodPtr);
	}

	template<CALLABLE(LambdaT, void(ParamTypes...))>
	void Bind(LambdaT&& Func)
	{
		Item<ParamTypes...>& item = this->Items.AddDefault();
		item.Bind(std::move(Func));
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Unbind(DWeakPtr<OwnerType> InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		for (cint item_i = 0; item_i < this->Items.Length(); item_i++)
		{
			Item<ParamTypes...>& item = this->Items[item_i];
			if (item.OwnerPtr == InOwnerPtr && item.MethodPtr == InMethodPtr)
			{
				item.Removed = true;
				break;
			}
		}
	}

	template<SUBTYPE(OwnerType, HObject)>
	void Unbind(DSharedPtr<OwnerType> InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->Unbind((DWeakPtr<OwnerType>)InOwnerPtr, InMethodPtr);
	}

	void UnbindAll()
	{
		this->ShouldRemoveAll = true;
	}

	void UnbindAllUnsafe()
	{
		this->Items.Clear();
	}

	void Call(ParamTypes... Arg)
	{
		if (this->ShouldRemoveAll)
		{
			this->Items.Clear();
			this->ShouldRemoveAll = false;
		}
		else
		{
			this->Items.RemoveIf([](const Item<ParamTypes...>& item, cint index){ return item.Removed; });
		}

		for (cint i = 0; i < this->Items.Length(); i++)
		{
			Item<ParamTypes...>& item = this->Items[i];
			item.Call(std::forward<ParamTypes>(Arg)...);
		}
	}

	bool IsBound() const
	{
		return this->Items.Length() > 0;
	}
};


/* 
Uses raw pointers, can't unbind callbacks
*/
template<typename... ParamTypes>
class DFastMultiCastDelegate
{
private:

	template<typename... ParamTypes>
	struct Item
	{
		HObject* OwnerPtr;
		void (HObject::*MethodPtr)(ParamTypes...);

		Item(HObject* InOwnerPtr, void (HObject::*InMethodPtr)(ParamTypes...)) : 
			OwnerPtr(InOwnerPtr), 
			MethodPtr(InMethodPtr)
		{
		}
	};


	DArray<Item<ParamTypes...>> Items;

public:

	// construct
	DFastMultiCastDelegate() = default;
	// copy
	DFastMultiCastDelegate(const DFastMultiCastDelegate& Other) = default;
	DFastMultiCastDelegate& operator=(const DFastMultiCastDelegate& Other) = default;
	// move
	DFastMultiCastDelegate(DFastMultiCastDelegate&& Other) = default;
	DFastMultiCastDelegate& operator=(DFastMultiCastDelegate&& Other) = default;
	// destruct
	~DFastMultiCastDelegate() = default;


	template<typename OwnerType>
	void Bind(HObject* InOwnerPtr, void (OwnerType::*InMethodPtr)(ParamTypes...))
	{
		this->Items.Emplace(
			InOwnerPtr,
			static_cast<void (HObject::*)(ParamTypes...)>(InMethodPtr)
		);
	}

	void Call(ParamTypes... Arg) const
	{
		for (cint i = 0; i < this->Items.Length(); i++)
		{
			const Item<ParamTypes...>& item = this->Items[i];
			(item.OwnerPtr->*(item.MethodPtr))(std::forward<ParamTypes>(Arg)...);
		}
	}

	bool IsBound() const
	{
		return this->Items.Length() > 0;
	}
};