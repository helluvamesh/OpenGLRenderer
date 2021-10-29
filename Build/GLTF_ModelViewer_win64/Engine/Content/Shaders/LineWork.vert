#version 400 compatibility

// IN
layout(location = 0) in vec3 VertLocalPos;

uniform mat4 ProjectMat;
uniform mat4 ViewMat;
uniform mat4 ModelMat;

// OUT
out vec3 WorldPos;


void main()
{
    WorldPos = vec3(vec4(VertLocalPos, 1.0) * ModelMat);
    gl_Position = vec4(WorldPos, 1.0) * ViewMat * ProjectMat;
}
