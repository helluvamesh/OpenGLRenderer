#include "DefaultUnlitSurfaceShader.vert"

// IN
uniform float UVScale = 1.0;
uniform float UVOffsetX = .0;
uniform float UVOffsetY = .0;


void main()
{
    defaultUnlitSurfaceShader(
        VertUV * UVScale + vec2(UVOffsetX, UVOffsetY),  // uv
        VertUV2,  // uv2
        VertLocalPos,  // localPos
        VertLocalNormal  // localNormal
    );
}