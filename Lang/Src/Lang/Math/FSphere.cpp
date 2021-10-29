#include "pch.h"
#include "FSphere.h"


const FSphere FSphere::DEFAULT;


FSphere::FSphere() :
	Radius(.0f)
{
}


FSphere::FSphere(const FVec3& InCenter, float InRadius) :
	Center(InCenter),
	Radius(InRadius)
{
}
