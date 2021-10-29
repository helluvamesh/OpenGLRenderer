/*
* Enumerator interfaces are a safer and a lot simpler alternative to std iterators.
* The idea is that you call Next() and you get a pointer to the next item or nullptr, if there are no more items.
* The overhead of the virtual function calls will only apply, if you access the enumerator thru a pointer,
* otherwise they will be as fast as a non-virtual function call.
* Even if you don't actually use the enumerators thru the iterface, implementing it ensures that all your 
* enumerators are written in a unified manner, which makes usage predictable
* and avoids the head-ache of writing std iterators.
*/


#pragma once


template<typename T>
class IEnumerator
{
public:
	virtual ~IEnumerator() = default;
	virtual T* Next() = 0;
};


template<typename T>
class IRemovingEnumerator
{
public:
	virtual ~IRemovingEnumerator() = default;
	virtual T* Next() = 0;
	virtual void Remove() = 0;
};


template<typename T>
class IConstEnumerator
{
public:
	virtual ~IConstEnumerator() = default;
	virtual const T* Next() = 0;
};


template<typename T>
class IConstRemovingEnumerator
{
public:
	virtual ~IConstRemovingEnumerator() = default;
	virtual const T* Next() = 0;
	virtual void Remove() = 0;
};
