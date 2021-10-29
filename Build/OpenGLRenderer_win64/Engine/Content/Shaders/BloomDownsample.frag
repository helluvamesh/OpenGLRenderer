#version 400 compatibility

// IN
in vec2 UV;

uniform sampler2DMS HDRBrightBuffer;
uniform float Samples;

// OUT
layout(location = 0) out vec4 BrightColor;


void main()
{
    vec2 size = textureSize(HDRBrightBuffer);
    int s = int(Samples);

    vec3 brightColor = vec3(.0);
    for (int i = 0; i < s; i++)
    {
        brightColor += texelFetch(HDRBrightBuffer, ivec2(int(size.x * UV.x), int(size.y * UV.y)), i).rgb;
    }
    brightColor /= Samples;
    
    BrightColor = vec4(brightColor, 1.0);
}  