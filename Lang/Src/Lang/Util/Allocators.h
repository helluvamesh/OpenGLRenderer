#pragma once


template<typename T>
class DArrayAllocator
{
public:

	static DArrayAllocator<T> INSTANCE;

	T* Allocate(cint Size)
	{
		return (T*)malloc(sizeof(T) * Size);
	}

	void Deallocate(T* Array, cint Size)
	{
		free(Array);
	}

};


template<typename T>
DArrayAllocator<T> DArrayAllocator<T>::INSTANCE;


template<typename T>
class DNodeArrayAllocator
{
public:

	static DNodeArrayAllocator<T> INSTANCE;

	T** AllocateArray(cint Size)
	{
		return (T**)malloc(sizeof(T**) * Size);
	}

	void DeallocateArray(T** Array, cint Size)
	{
		free(Array);
	}

	T* AllocateNode()
	{
		return (T*)malloc(sizeof(T));
	}

	void DeallocateNode(T* Node)
	{
		free(Node);
	}

};


template<typename T>
DNodeArrayAllocator<T> DNodeArrayAllocator<T>::INSTANCE;
