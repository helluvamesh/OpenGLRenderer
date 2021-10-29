#pragma once

#include "Engine\Util\UtilTypes.h"


void glClearErrors();
bool glCheckError(DStringView8 file, int32 line);


#ifdef DEBUG_OPENGL

#define glCheck(line) \
    glClearErrors();\
    line;\
    glCheckError(__FILE__, __LINE__)

#else

#define glCheck(line) line;

#endif
