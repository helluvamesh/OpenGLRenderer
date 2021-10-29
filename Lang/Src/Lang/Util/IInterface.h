#pragma once

/*________________________________________________________
	
	interfaces 
	- should mostly have pure virtual functions
	  e.g.: virtual void DoSomething() = 0;
	- must not be subclassed deaper than 1 level!
________________________________________________________*/


class IInterface
{
public:

	// needed for deleting objects thru interface
	virtual ~IInterface() = default;

};