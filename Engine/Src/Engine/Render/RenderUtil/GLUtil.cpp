#include "Engine/pch.h"
#include "GLUtil.h"
#include "GL/glew.h"
#include <sstream>


DMap<DString8, DString8> _ErrorMap {

    { "500", "GL_INVALID_ENUM" },
    { "501", "GL_INVALID_VALUE" },
    { "502", "GL_INVALID_OPERATION" },
    { "503", "GL_STACK_OVERFLOW" },
    { "504", "GL_STACK_UNDERFLOW" },
    { "505", "GL_OUT_OF_MEMORY" },
    { "506", "GL_INVALID_FRAMEBUFFER_OPERATION" },
    { "507", "GL_CONTEXT_LOST" },  // version 4.5
    { "UNKNOWN", "unknown OpenGL error" }
};


DString8 _HexToString(uint32 Hex)
{
    std::stringstream stream;
    stream << std::hex << Hex;
    return stream.str().c_str();
}


void glClearErrors()
{
    while (glGetError()) {}
}


bool glCheckError(DStringView8 file, int32 line)
{
    while (GLenum error = glGetError())
    {
        DString8 errorNumber = _HexToString(error);
        DString8* message = _ErrorMap.Find(errorNumber);
        if (message == nullptr)
        {
            message = _ErrorMap.Find("UNKNOWN");
        }
        
        DString8 msg;
        msg.Append("OpenGL Error (");
        msg.Append(errorNumber);
        msg.Append("): ");
        msg.Append(*message);
        msg.Append(" in line ");
        DNumberFormat8::DEFAULT.AppendTo(line, msg);
        msg.Append(", file: ");
        msg.Append(file);

        SLog::Error(msg);
        return false;
    }
    return true;
}
