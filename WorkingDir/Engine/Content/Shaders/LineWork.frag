#version 400 compatibility

// IN
in vec3 WorldPos;

uniform vec3 ViewPos;
uniform vec4 Color = vec4(1.0, 1.0, 1.0, 1.0);


// OUT
layout(location = 0) out vec4 OutColor;


void main()
{
    OutColor = Color;
}
