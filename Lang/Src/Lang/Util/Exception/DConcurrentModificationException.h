#pragma once

#include "DRuntimeException.h"


class DConcurrentModificationException : public DRuntimeException
{
public:
	DConcurrentModificationException();

};