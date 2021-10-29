#include "DefaultUnlitSurfaceShader.frag"

// IN
uniform vec4 Emission = vec4(.0);
uniform float Opacity = 1.0;

uniform bool UseEmissionTex = false;
uniform sampler2D EmissionTex;


void main()
{
    defaultUnlitSurfaceShader(
        UseEmissionTex? texture(EmissionTex, UV).rgb * Emission.rgb : Emission.rgb,  // Emission
        Opacity  // Opacity
    );
}