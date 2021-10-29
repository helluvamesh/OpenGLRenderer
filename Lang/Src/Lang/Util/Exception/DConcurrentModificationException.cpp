#include "pch.h"
#include "DConcurrentModificationException.h"


DConcurrentModificationException::DConcurrentModificationException()
	: DRuntimeException("Concurrent Modification Exception: collection was modified while being iterated")
{
}
