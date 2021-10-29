#version 400 compatibility

// IN
in vec2 UV;

uniform sampler2D HDRBrightBuffer;
uniform bool Horizontal;

// OUT
layout(location = 0) out vec4 OutBrightColor;


const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);


void main()
{       
    vec2 texelOffset = 1.0 / textureSize(HDRBrightBuffer, 0);
    vec3 result = texture(HDRBrightBuffer, UV).rgb * weights[0];
    
    if (Horizontal)
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(HDRBrightBuffer, UV + vec2(texelOffset.x * i, 0.0)).rgb * weights[i];
            result += texture(HDRBrightBuffer, UV - vec2(texelOffset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; i++)
        {
            result += texture(HDRBrightBuffer, UV + vec2(0.0, texelOffset.y * i)).rgb * weights[i];
            result += texture(HDRBrightBuffer, UV - vec2(0.0, texelOffset.y * i)).rgb * weights[i];
        }
    }

    OutBrightColor = vec4(result, 1.0);
}  