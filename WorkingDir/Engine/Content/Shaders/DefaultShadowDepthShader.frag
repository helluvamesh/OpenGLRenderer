#version 400 compatibility

// IN
in vec2 UV;
in vec2 UV2;

uniform int BlendMode = 0;


void defaultShadowDepthShader(float opacity)
{   
    switch (BlendMode)
    {
    case 1:   // Alpha Test
        if (opacity < .5) discard;
        break;
    }
}
