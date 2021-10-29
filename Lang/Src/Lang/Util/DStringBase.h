/*______________________________________________________________

_DStringBase:
- implements a dynamic (resizable) string of characters
- items are laid out contiguously in memory
- can reserve memory for future characters to avoid constant resizing and moving
- supports bracket-indexing syntax
- supports enhanced for loop (foreach)

______________________________________________________________*/


#pragma once


// S: stack allocated buffer size in bytes
template<typename T, cint S, class AllocatorT>
class _DStringBase
{
protected:

	constexpr static const cint SHORT_STR_LIMIT = S / CINT_SIZEOF(T);

	union StorageUnion
	{
		T* Array;
		T ShortArray[SHORT_STR_LIMIT];
	};

	StorageUnion Storage;
	cint Size;
	/* must be greater than or equal to SHORT_STR_LIMIT */
	cint ArrayCapacity;


	FORCEINLINE T* Allocate(cint Size)
	{
		return AllocatorT::INSTANCE.Allocate(Size);
	}

	FORCEINLINE T* AllocateInPlace(cint Size)
	{
		if (Size > SHORT_STR_LIMIT)
		{
			return this->Storage.Array = AllocatorT::INSTANCE.Allocate(Size);
		}
		else
		{
			return this->Storage.ShortArray;
		}
	}

	FORCEINLINE void Deallocate(T* Buffer, cint Size)
	{
		AllocatorT::INSTANCE.Deallocate(Buffer, Size);
	}

	FORCEINLINE void DeallocateThis()
	{
		if (this->ArrayCapacity > SHORT_STR_LIMIT && this->Storage.Array)
		{
			AllocatorT::INSTANCE.Deallocate(this->Storage.Array, this->ArrayCapacity);
			this->Storage.Array = nullptr;
		}
	}


	FORCEINLINE void __Construct__()
	{
		this->Size = 0;
		this->ArrayCapacity = SHORT_STR_LIMIT;
		this->Storage.ShortArray[0] = '\0';
	}


	FORCEINLINE void __Construct__(cint InCapacity, bool OverwriteLength)
	{
		this->Size = OverwriteLength? InCapacity : 0;
		this->ArrayCapacity = SCounterUtil::Max(InCapacity + 1, SHORT_STR_LIMIT);
		T* arr = this->AllocateInPlace(this->ArrayCapacity);
		arr[this->Size] = '\0';
	}


	FORCEINLINE void __Construct__(const T* InCString)
	{
		this->Size = SCounterUtil::CStringLength(InCString);
		this->ArrayCapacity = SCounterUtil::Max(this->Size + 1, SHORT_STR_LIMIT);
		T* arr = this->AllocateInPlace(this->ArrayCapacity);
		for (cint i = 0; i < this->Size; i++)
		{
			arr[i] = InCString[i];
		}
		arr[this->Size] = '\0';
	}


	FORCEINLINE void __Construct__(const T* InBuffer, cint InBufferSize)
	{
		this->Size = InBufferSize;
		this->ArrayCapacity = SCounterUtil::Max(this->Size + 1, SHORT_STR_LIMIT);
		T* arr = this->AllocateInPlace(this->ArrayCapacity);
		for (cint i = 0; i < this->Size; i++)
		{
			arr[i] = InBuffer[i];
		}
		arr[this->Size] = '\0';
	}


	FORCEINLINE void __CopyConstruct__(const _DStringBase<T, S, AllocatorT>& Other)
	{
		this->Size = Other.Size;
		T* arr = this->AllocateInPlace(this->ArrayCapacity = Other.ArrayCapacity);
		const T* otherArr = Other._Data();
		for (cint i = 0; i < this->Size; i++)
		{
			arr[i] = otherArr[i];
		}
		arr[this->Size] = '\0';
	}


	FORCEINLINE void __CopyAssign__(const _DStringBase<T, S, AllocatorT>& Other)
	{
		this->_Reserve(Other.ArrayCapacity);
		this->Size = Other.Size;
		T* arr = this->_Data();
		const T* otherArr = Other._Data();
		for (cint i = 0; i < this->Size; i++)
		{
			arr[i] = otherArr[i];
		}
		arr[this->Size] = '\0';
	}


	FORCEINLINE void __MoveConstruct__(_DStringBase<T, S, AllocatorT>&& Other)
	{
		this->ArrayCapacity = Other.ArrayCapacity;
		this->Size = Other.Size;
		if (Other.ArrayCapacity > SHORT_STR_LIMIT)
		{
			this->Storage.Array = Other.Storage.Array;
			this->Storage.Array[this->Size] = '\0';
			Other.Storage.Array = nullptr;
			Other.Storage.ShortArray[0] = '\0';
		}
		else
		{
			T* arr = this->Storage.ShortArray;
			T* otherArr = Other.Storage.ShortArray;
			for (cint i = 0; i < this->Size; i++)
			{
				arr[i] = otherArr[i];
			}
			arr[this->Size] = '\0';
			otherArr[0] = '\0';
		}
		Other.Size = 0;
		Other.ArrayCapacity = SHORT_STR_LIMIT;
	}


	FORCEINLINE void __MoveAssign__(_DStringBase<T, S, AllocatorT>&& Other)
	{
		this->DeallocateThis();
		this->__MoveConstruct__(std::move(Other));
	}


	FORCEINLINE void __Destruct__()
	{
		this->DeallocateThis();
	}


	FORCEINLINE T* _Data()
	{
		if (this->ArrayCapacity > SHORT_STR_LIMIT)
		{
			return this->Storage.Array;
		}
		else
		{
			return this->Storage.ShortArray;
		}
	}


	FORCEINLINE const T* _Data() const
	{
		if (this->ArrayCapacity > SHORT_STR_LIMIT)
		{
			return this->Storage.Array;
		}
		else
		{
			return this->Storage.ShortArray;
		}
	}

	/*___________________________

		MANIPULATION
	___________________________*/

	FORCEINLINE void _Append(const T* OtherBuffer, cint OtherBufferSize)
	{
		cint newSize = this->Size + OtherBufferSize;
		cint prevSize = this->Size;
		this->_GrowIfNecessary(newSize);
		T* arr = this->_Data();
		for (cint i = 0; i < OtherBufferSize; i++)
		{
			arr[prevSize + i] = OtherBuffer[i];
		}
		arr[newSize] = '\0';
		this->Size = newSize;
	}


	FORCEINLINE void _Prepend(const T* OtherBuffer, cint OtherBufferSize)
	{
		cint prevSize = this->Size;
		cint newSize = this->Size + OtherBufferSize;
		this->_GrowIfNecessary(newSize);
		T* arr = this->_Data();
		// shift chars right
		for (cint i = prevSize; i >= 0; i--)
		{
			arr[i + OtherBufferSize] = arr[i];
		}
		// insert at beginning
		for (cint i = 0; i < OtherBufferSize; i++)
		{
			arr[i] = OtherBuffer[i];
		}
		arr[newSize] = '\0';
		this->Size = newSize;
	}


	FORCEINLINE static cint _IndexOf(
		const T* Buffer, cint BufferSize,
		const T* OtherBuffer, cint OtherSize, 
		cint Start, cint Stop
	)
	{
		if (Stop == -1) Stop = BufferSize;

		if (BufferSize > 0 && OtherSize > 0)
		{
			const T firstChar = OtherBuffer[0];
			for (cint i = Start; i + OtherSize - 1 < Stop; i++)
			{
				if (Buffer[i] == firstChar)
				{
					if (OtherSize == 1) 
					{
						return i;
					}
					for (cint m_i = 1; m_i < OtherSize; m_i++)
					{
						if (Buffer[i + m_i] != OtherBuffer[m_i])
						{
							goto break_sequence;
						}
					}
					return i;
				}
			break_sequence: {}
			}
		}
		return -1;
	}

	// Start should be greater than Stop!
	// Start is the last index, Stop is the first index ('cause it searches from the end)
	FORCEINLINE static cint _LastIndexOf(
		const T* Buffer, cint BufferSize,
		const T* OtherBuffer, cint OtherSize, 
		cint Start, cint Stop
	)
	{
		if (BufferSize > 0 && OtherSize > 0)
		{
			const T lastChar = OtherBuffer[OtherSize - 1];
			for (cint i = Start; i >= OtherSize - 1 + Stop; i--)
			{
				if (Buffer[i] == lastChar)
				{
					if (OtherSize == 1) 
					{
						return i;
					}
					for (cint m_i = OtherSize - 2; m_i >= 0; m_i--)
					{
						if (Buffer[i - (OtherSize - 1) + m_i] != OtherBuffer[m_i])
						{
							goto break_sequence;
						}
					}
					return i - (OtherSize - 1);
				}
			break_sequence: {}
			}
		}
		return -1;
	}


	/* Compare: (T CharA, T CharB) -> cint */
	template<typename CharComparer>
	FORCEINLINE static cint _IndexOfCustomCompare(
		const T* Buffer, cint BufferSize,
		const T* OtherBuffer, cint OtherSize, 
		CharComparer Compare, cint Start, cint Stop
	)
	{
		if (BufferSize > 0 && OtherSize > 0)
		{
			const T firstChar = OtherBuffer[0];
			for (cint i = Start; i + OtherSize - 1 < BufferSize; i++)
			{
				if (Compare(Buffer[i], firstChar) == 0)
				{
					if (OtherSize == 1) 
					{
						return i;
					}
					for (cint m_i = 1; m_i < OtherSize; m_i++)
					{
						if (Compare(Buffer[i + m_i], OtherBuffer[m_i]) != 0)
						{
							goto break_sequence;
						}
					}
					return i;
				}
			break_sequence: {}
			}
		}
		return -1;
	}


	/* Compare: (T CharA, T CharB) -> cint */
	template<typename CharComparer>
	FORCEINLINE static cint _LastIndexOfCustomCompare(
		const T* Buffer, cint BufferSize,
		const T* OtherBuffer, cint OtherSize, 
		CharComparer Compare, cint Start = -1, cint Stop = 0
	)
	{
		if (BufferSize > 0 && OtherSize > 0)
		{
			const T lastChar = OtherBuffer[OtherSize - 1];
			for (cint i = Start; i >= OtherSize - 1 + Stop; i--)
			{
				if (Compare(Buffer[i], lastChar) == 0)
				{
					if (OtherSize == 1) 
					{
						return i;
					}
					for (cint m_i = OtherSize - 2; m_i >= 0; m_i--)
					{
						if (Compare(Buffer[i - (OtherSize - 1) + m_i], OtherBuffer[m_i]) != 0)
						{
							goto break_sequence;
						}
					}
					return i - (OtherSize - 1);
				}
			break_sequence: {}
			}
		}
		return -1;
	}


	FORCEINLINE void _ReplaceRange(
		cint Start, cint Stop, 
		const T* OtherBuffer, cint OtherBufferSize
	)
	{
		cint insertedChars = OtherBufferSize;
		cint removedChars = Stop - Start;
		cint prevSize = this->Size;
		cint newSize = this->Size - (Stop - Start) + insertedChars;
		this->_GrowIfNecessary(newSize);
		T* arr = this->_Data();
		if (insertedChars > removedChars)
		{
			cint amount = insertedChars - removedChars;
			// shift tail right
			for (cint i = prevSize; i >= Stop; i--)
			{
				arr[i + amount] = arr[i];
			}
		}
		else if (insertedChars < removedChars)
		{
			cint amount = removedChars - insertedChars;
			// shift tail left
			for (cint i = Stop; i < prevSize; i++)
			{
				arr[i - amount] = arr[i];
			}
		}
		// insert chars;
		for (cint i = 0; i < insertedChars; i++)
		{
			arr[Start + i] = OtherBuffer[i];
		}
		arr[newSize] = '\0';
		this->Size = newSize;
	}


	FORCEINLINE static void _ReplaceRangeCopy(
		const T* Buffer, cint BufferSize, 
		cint Start, cint Stop, 
		const T* OtherBuffer, cint OtherBufferSize, 
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		cint insertedChars = OtherBufferSize;
		cint removedChars = Stop - Start;
		cint prevSize = BufferSize;
		cint newSize = BufferSize - (Stop - Start) + insertedChars;
		Result._Reserve(newSize);
		T* resultArr = Result._Data();
		// head
		for (cint i = 0; i < Start; i++)
		{
			resultArr[i] = Buffer[i];
		}
		// insertion
		for (cint i = 0; i < insertedChars; i++)
		{
			resultArr[Start + i] = OtherBuffer[i];
		}
		// tail
		cint resultProgress = Start + insertedChars;
		cint tailSize = prevSize - Stop;
		for (cint i = 0; i < tailSize; i++)
		{
			resultArr[resultProgress + i] = Buffer[Stop + i];
		}
		resultArr[newSize] = '\0';
		Result.Size = newSize;
	}


	FORCEINLINE void _ReplaceRangeWithChar(cint Start, cint Stop, T Char)
	{
		T* arr = this->_Data();
		for (cint i = Start; i < Stop; i++)
		{
			arr[i] = Char;
		}
	}


	FORCEINLINE static void _ReplaceRangeWithCharCopy(
		const T* Buffer, cint BufferSize,
		cint Start, cint Stop, T Char,
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		Result._Reserve(BufferSize);
		T* resultArr = Result._Data();
		for (cint i = 0; i < Start; i++)
		{
			resultArr[i] = Buffer[i];
		}
		for (cint i = Start; i < Stop; i++)
		{
			resultArr[i] = Char;
		}
		for (cint i = Stop; i < BufferSize; i++)
		{
			resultArr[i] = Buffer[i];
		}
		Result.Size = BufferSize;
		resultArr[BufferSize] = '\0';
	}


	FORCEINLINE void _RemoveRange(cint Start, cint Stop)
	{
		cint removedChars = Stop - Start;
		cint prevSize = this->Size;
		cint newSize = this->Size - (Stop - Start);
		T* arr = this->_Data();
		// shift tail left
		for (cint i = Stop; i < prevSize; i++)
		{
			arr[i - removedChars] = arr[i];
		}
		arr[newSize] = '\0';
		this->Size = newSize;
	}


	FORCEINLINE static void _RemoveRangeCopy(
		const T* Buffer, cint BufferSize,
		cint Start, cint Stop, 
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		cint removedChars = Stop - Start;
		cint prevSize = BufferSize;
		cint newSize = BufferSize - (Stop - Start);
		Result._Reserve(newSize);
		T* resultArr = Result._Data();
		// head
		for (cint i = 0; i < Start; i++)
		{
			resultArr[i] = Buffer[i];
		}
		// tail
		cint tailSize = prevSize - Stop;
		for (cint i = 0; i < tailSize; i++)
		{
			resultArr[Start + i] = Buffer[Stop + i];
		}
		resultArr[newSize] = '\0';
		Result.Size = newSize;
	}


	/* returns whether something was replaced */
	FORCEINLINE static bool _ReplaceAllCopy(
		const T* Buffer, cint BufferSize,
		const T* SearchBuffer, cint SearchSize, 
		const T* ReplacementBuffer, cint ReplacementSize, 
		_DStringBase<T, S, AllocatorT>& Result, cint Start, cint Stop
	)
	{
		cint found_i = -1;
		cint prev_i = 0;
		while ((found_i = _IndexOf(Buffer, BufferSize, SearchBuffer, SearchSize, found_i + 1, Stop)) >= 0)
		{
			Result._Append(Buffer + prev_i, found_i - prev_i);
			Result._Append(ReplacementBuffer, ReplacementSize);
			prev_i = found_i + SearchSize;
		}

		Result._Append(Buffer + prev_i, BufferSize - prev_i);
		return prev_i > 0;
	}


	/* returns whether something was replaced */
	FORCEINLINE bool _ReplaceFirst(
		const T* SearchBuffer, cint SearchSize, 
		const T* ReplacementBuffer, cint ReplacementSize, 
		cint Start, cint Stop
	)
	{
		cint i = _IndexOf(this->_Data(), this->Size, SearchBuffer, SearchSize, Start, Stop);
		if (i >= 0)
		{
			this->_ReplaceRange(i, i + SearchSize, ReplacementBuffer, ReplacementSize);
			return true;
		}
		return false;
	}


	/* returns whether something was replaced */
	FORCEINLINE static bool _ReplaceFirstCopy(
		const T* Buffer, cint BufferSize,
		const T* SearchBuffer, cint SearchSize, 
		const T* ReplacementBuffer, cint ReplacementSize, 
		_DStringBase<T, S, AllocatorT>& Result, cint Start, cint Stop
	)
	{
		cint i = _IndexOf(Buffer, BufferSize, SearchBuffer, SearchSize, Start, Stop);
		if (i >= 0)
		{
			_ReplaceRangeCopy(Buffer, BufferSize, i, i + SearchSize, ReplacementBuffer, ReplacementSize, Result);
			return true;
		}
		return false;
	}


	FORCEINLINE static cint _CountOf(
		const T* Buffer, cint BufferSize,
		const T* SearchBuffer, cint SearchSize, 
		cint Start, cint Stop
	)
	{
		cint count = 0;
		cint found_i = Start - 1;
		while ((found_i = _IndexOf(Buffer, BufferSize, SearchBuffer, SearchSize, found_i + 1, Stop)) >= 0)
		{
		    count++;
		}
		return count;
	}


	/* Compare: (T CharA, T CharB) -> cint */
	template<typename CharComparer>
	FORCEINLINE static cint _CountOfCustomCompare(
		const T* Buffer, cint BufferSize,
		const T* SearchBuffer, cint SearchSize, 
		CharComparer Compare, cint Start, cint Stop
	)
	{
		cint count = 0;
		cint found_i = Start - 1;
		while ((found_i = _IndexOfCustomCompare(Buffer, BufferSize, SearchBuffer, SearchSize, Compare, found_i + 1, Stop)) >= 0)
		{
			count++;
		}
		return count;
	}


	FORCEINLINE static void _SubstrRange(
		const T* Buffer, cint BufferSize,
		cint Start, cint Stop, 
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		Result._Reserve(Stop - Start);
		T* resultArr = Result._Data();
		cint len = 0;
		for (cint i = Start; i < Stop; i++)
		{
			resultArr[len++] = Buffer[i];
		}
		resultArr[len] = '\0';
		Result.Size = len;
	}


	FORCEINLINE static void _SubstrCount(
		const T* Buffer, cint BufferSize,
		cint Start, cint Count, 
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		_SubstrRange(Buffer, BufferSize, Start, Start + Count, Result);
	}


	FORCEINLINE static void _Substr(
		const T* Buffer, cint BufferSize,
		cint Start, 
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		_SubstrRange(Buffer, BufferSize, Start, BufferSize, Result);
	}


	FORCEINLINE void _Clear()
	{
		this->Size = 0;
	}


	FORCEINLINE void _Reverse()
	{
		if (this->Size > 1)
		{
			cint middle = (Size % 2 == 0)? (Size / 2) : ((Size + 1) / 2);
			cint last = this->Size - 1;
			T* arr = this->_Data();
			for (cint i = 0; i < middle; i++)
			{
				T temp = arr[i];
				arr[i] = arr[last - i];
				arr[last - i] = temp;
			}
		}
	}


	FORCEINLINE static void _Reversed(
		const T* Buffer, cint BufferSize,
		_DStringBase<T, S, AllocatorT>& Result
	)
	{
		if (BufferSize > 0)
		{
			cint len = BufferSize;
			Result._Reserve(len);
			T* resultArr = Result._Data();
			cint p = 0;
			for (cint i = len - 1; i >= 0; i--)
			{
				new (resultArr + p++) T(Buffer[i]);
			}
			Result.Size = len;
			resultArr[len] = '\0';
		}
	}


	FORCEINLINE cint _Capacity() const
	{
		// null terminator is included in ArrayCapacity
		if (this->ArrayCapacity > SHORT_STR_LIMIT)
		{
			return this->ArrayCapacity - 1;
		}
		else
		{
			return SHORT_STR_LIMIT - 1;
		}
	}


	FORCEINLINE void SetCapacityCountingNullTerminator(cint NewCapacity)
	{
		if (NewCapacity > SHORT_STR_LIMIT)
		{
			cint prevCapacity = this->ArrayCapacity;
			cint prevSize = this->Size;
			T* arr = this->_Data();
			T* newArray = Allocate(NewCapacity);
			if (NewCapacity - 1 < this->Size)
			{
				this->Size = NewCapacity - 1;
			}
			this->ArrayCapacity = NewCapacity;
			for (cint i = 0; i < prevSize; i++)
			{
				newArray[i] = arr[i];
			}
			newArray[this->Size] = '\0';
			if (prevCapacity > SHORT_STR_LIMIT)
			{
				Deallocate(this->Storage.Array, prevCapacity);
			}
			this->Storage.Array = newArray;
		}
		else
		{
			this->ArrayCapacity = SHORT_STR_LIMIT;
			if (NewCapacity - 1 < this->Size)
			{
				this->Size = NewCapacity - 1;
			}
			this->Storage.ShortArray[this->Size] = '\0';
		}
	}


	FORCEINLINE void _Reserve(cint NewCapacity)
	{
		NewCapacity++;  // make place for null terminator
		if (this->ArrayCapacity < NewCapacity)
		{
			this->SetCapacityCountingNullTerminator(NewCapacity);
		}
	}


	// Reserve without copying data from the old buffer
	// CapacityAsLength = false sets length to 0
	// CapacityAsLength = true results in an uninitailized string - Unsafe
	FORCEINLINE void _Reallocate(cint NewCapacity, bool CapacityAsLength = false)
	{
		// avoid unneccessary copying of old data
		this->Size = 0;
		this->_Data()[0] = '\0';
		
		cint newCapacityWoTerminator = NewCapacity;
		NewCapacity++;  // make place for null terminator
		if (this->ArrayCapacity < NewCapacity)
		{
			this->SetCapacityCountingNullTerminator(NewCapacity);
		}
		if (CapacityAsLength)
		{
			this->Size = newCapacityWoTerminator;
			this->_Data()[this->Size] = '\0';
		}
	}


	FORCEINLINE void _GrowIfNecessary(cint NewSize)
	{
		if (NewSize + 1 > this->ArrayCapacity)
		{
			this->_Reserve(SCounterUtil::Max(NewSize, (this->ArrayCapacity - 1) * 2));
		}
	}


	FORCEINLINE void _Resize(cint NewSize, T PaddingChar)
	{
		this->_Reserve(NewSize);
		T* arr = this->_Data();
		for (cint i = this->Size; i < NewSize; i++)
		{
			arr[i] = PaddingChar;
		}
		arr[NewSize] = '\0';
		this->Size = NewSize;
	}


	FORCEINLINE void _TrimCapacity()
	{
		if (this->Size + 1 < this->ArrayCapacity)
		{
			this->SetCapacityCountingNullTerminator(this->Size + 1);
		}
	}

};
