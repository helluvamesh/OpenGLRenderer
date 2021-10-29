#version 400 compatibility

// IN
layout(location = 0) in vec3 VertPosition;
layout(location = 1) in vec2 VertUV;

// OUT
out vec2 UV;


void main()
{
    gl_Position = vec4(VertPosition, 1.0);
    UV = VertUV;
}