#version 400 compatibility

// IN
in vec3 LocalPosition;

uniform vec3 ViewPos;
uniform samplerCube EnvMap;

// OUT
layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutBrightColor;


void main()
{   
    vec3 color = texture(EnvMap, LocalPosition).rgb;
    color = pow(color, vec3(.455));
    color = min(color, vec3(1.0));
    OutColor = vec4(color, 1.0);
    OutBrightColor = vec4(.0);
}
