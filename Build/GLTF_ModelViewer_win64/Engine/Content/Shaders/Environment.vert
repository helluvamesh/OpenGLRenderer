#version 400 compatibility

// IN
layout(location = 0) in vec3 VertPosition;

uniform mat4 ProjectMat;
uniform mat4 ViewMat;
uniform mat4 ModelMat;

// OUT
out vec3 LocalPosition;


void main()
{
    gl_Position = vec4(VertPosition, 1.0) * ModelMat * ViewMat * ProjectMat;
    LocalPosition = VertPosition;
}