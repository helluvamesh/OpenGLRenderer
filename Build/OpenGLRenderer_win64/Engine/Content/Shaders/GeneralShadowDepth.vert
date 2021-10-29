#include "DefaultShadowDepthShader.vert"


void main()
{
    defaultShadowDepthShader(
        VertUV,  // uv
        VertUV2,  // uv2
        VertLocalPos  // localPos
    );
}
