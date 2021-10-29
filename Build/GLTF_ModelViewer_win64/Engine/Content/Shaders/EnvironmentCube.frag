#version 400 compatibility

// IN
in vec3 LocalPosition;

uniform vec3 ViewPos;
uniform samplerCube EnvMap;

// OUT
layout(location = 0) out vec4 OutColor;


void main()
{   
    OutColor = vec4(texture(EnvMap, LocalPosition).rgb, 1);
}