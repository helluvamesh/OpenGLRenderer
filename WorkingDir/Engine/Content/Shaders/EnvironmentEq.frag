#version 400 compatibility

// IN
in vec3 LocalPosition;

uniform vec3 ViewPos;
uniform bool VFlip;
uniform sampler2D EnvMap;
uniform float UOffset;

// OUT
layout(location = 0) out vec4 OutColor;


const float PI = 3.14159265359;
const vec2 INV_ATAN = vec2(1 / (2 * PI), 1 / PI);

vec2 sphericalProject(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x) + UOffset, asin(v.y));
    uv *= INV_ATAN;
    uv += 0.5;
    return uv;
}


void main()
{   
    vec2 uv = sphericalProject(normalize(LocalPosition));
    if (VFlip)
    {
        uv = vec2(uv.x, -uv.y);
    }
    OutColor = vec4(texture(EnvMap, uv).rgb, 1);
}