#pragma once


/*_____________________________________________________________________________

	Base class for objects with reflection AND polymorphism

	Inherit from 'DStruct' instead, if polymorphism is undesired 
	but you still need reflection
_____________________________________________________________________________*/


class HObject : public DSharedFromThis<HObject>
{
public:
	// make class polymorphic
	virtual ~HObject() = default;

protected:
	typedef HObject BaseObjectType;
	static bool StaticInit;
	static DClass ClassData;

public:
	static DClass& GetDeclaredClass();
	virtual DClass& GetClass() const;

	virtual DString ToString() const;

};
