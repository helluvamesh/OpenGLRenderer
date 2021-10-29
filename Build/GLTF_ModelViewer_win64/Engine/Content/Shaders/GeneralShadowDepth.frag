#include "DefaultShadowDepthShader.frag"

// IN
uniform bool UseBaseColorTex = false;
uniform sampler2D BaseColorTex;
uniform float Opacity = 1.0;


void main()
{
    if (UseBaseColorTex)
    {
        defaultShadowDepthShader(
            texture(BaseColorTex, UV).a  // opacity
        );
    }
    else
    {
        defaultShadowDepthShader(Opacity);
    }
}
