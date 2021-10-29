#version 400 compatibility

// IN
in vec2 UV;

uniform sampler2DMS HDRBuffer;
uniform sampler2D BloomBuffer_1;
uniform sampler2D BloomBuffer_2;
uniform sampler2D BloomBuffer_3;
uniform float Samples = 4;
uniform float Exposure = 1;
uniform float BloomIntensity1 = .5;
uniform float BloomIntensity2 = .5;
uniform float BloomIntensity3 = .5;

// OUT
layout(location = 0) out vec4 OutColor;


vec3 ACESFilmic(vec3 c)
{
    c *= Exposure;
    return vec3(
        c.r > .0? ((c.r * (2.51 * c.r + .03)) / (c.r * (2.43 * c.r + .59) + .14)) : .0,
        c.g > .0? ((c.g * (2.51 * c.g + .03)) / (c.g * (2.43 * c.g + .59) + .14)) : .0,
        c.b > .0? ((c.b * (2.51 * c.b + .03)) / (c.b * (2.43 * c.b + .59) + .14)) : .0
    );
}

vec3 Reinhard(vec3 c)
{
    c *= Exposure;
    return vec3(
        c.r > .0? c.r / (c.r + 1.0) : .0,
        c.g > .0? c.g / (c.g + 1.0) : .0,
        c.b > .0? c.b / (c.b + 1.0) : .0
    );
}

vec3 ExposureTonemap(vec3 c)
{
    return vec3(
        c.r > .0? 1.0 - exp(-c.r * Exposure) : .0,
        c.b > .0? 1.0 - exp(-c.g * Exposure) : .0,
        c.g > .0? 1.0 - exp(-c.b * Exposure) : .0
    );
}


void main()
{ 
    vec2 size = textureSize(HDRBuffer);
    int s = int(Samples);

    vec3 color = vec3(.0);
    for (int i = 0; i < s; i++)
    {
        color += texelFetch(HDRBuffer, ivec2(int(size.x * UV.x), int(size.y * UV.y)), i).rgb;
    }
    color /= Samples;
     
    vec3 bloom = texture(BloomBuffer_1, UV).rgb * BloomIntensity1;
    bloom     += texture(BloomBuffer_2, UV).rgb * BloomIntensity2;
    bloom     += texture(BloomBuffer_3, UV).rgb * BloomIntensity3;

    color = pow(ACESFilmic(color), vec3(2.2));
    color += ACESFilmic(bloom);
    
    OutColor = vec4(color, 1.0);
}  