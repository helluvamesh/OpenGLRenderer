#pragma once

/*________________________________________________________
	
	subclasses must have only pure virtual methods
	e.g.: virtual void DoSomething() = 0;
	
	must not be subclassed deeper than 1 level
________________________________________________________*/


class IInterface
{
public:
	// needed for deleting objects thru interface
	virtual ~IInterface() = default;

};