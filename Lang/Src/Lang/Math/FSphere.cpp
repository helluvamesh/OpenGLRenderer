#include "pch.h"
#include "FSphere.h"


FSphere::FSphere() :
	Radius(.0f)
{
}


FSphere::FSphere(const FVec3& InCenter, float InRadius) :
	Center(InCenter),
	Radius(InRadius)
{
}
