#version 400 compatibility

// IN
in vec3 WorldPos;
in vec3 WorldNormal;
in vec2 UV;
in vec4 VertexColor;
in vec2 UV2;

uniform vec3 ViewPos;
uniform int BlendMode = 0;
uniform float BloomStart = 2;


// OUT
layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutBrightColor;  // for bloom


void defaultUnlitSurfaceShader(
    vec3 emission,
    float opacity
)
{
    emission = max(vec3(.0), emission);
    opacity = clamp(opacity, .0, 1.0);

    bool alphaBlend = BlendMode == 2 || BlendMode == 3;

    if (BlendMode == 0 || BlendMode == 3)  // OPAQUE || ALPHA_BLEND step 2
    {
        opacity = 1.0;
    }
    else
    {
        opacity = clamp(opacity, .0, 1.0);
    }

    vec3 finalColor = pow(emission, vec3(1.0 / 2.2));
    OutColor = vec4(finalColor, opacity);

    // for bloom
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > BloomStart)
    {
        OutBrightColor = vec4(finalColor, opacity);
    }
    else
    {
        OutBrightColor = vec4(.0, .0, .0, 1.0);
    }
}
