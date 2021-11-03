#pragma once

/*________________________________________________________
	
	subclasses must have only pure virtual methods
	e.g.: virtual void DoSomething() = 0;

________________________________________________________*/


class IInterface
{
public:
	// needed for deleting objects thru interface
	virtual ~IInterface() = default;

};