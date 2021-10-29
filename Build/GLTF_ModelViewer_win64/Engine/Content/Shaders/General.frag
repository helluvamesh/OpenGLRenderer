#include "DefaultSurfaceShader.frag"

// IN
uniform vec4 BaseColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float Specular = .5;
uniform float Roughness = .5;
uniform float Metallic = .0;
uniform float AO = 1.0;
uniform vec4 Emission = vec4(.0);
uniform float Opacity = 1.0;

uniform bool UseBaseColorTex = false;
uniform sampler2D BaseColorTex;

uniform bool UseORMTex = false;
uniform sampler2D ORMTex;
uniform float RoughnessMax = 1.0;

uniform bool UseNormalMap = false;
uniform sampler2D NormalMap;
uniform float NormalIntensity = 1.0;

uniform bool UseEmissionTex = false;
uniform sampler2D EmissionTex;


void main()
{
    vec3 texNorWS;
    if (UseNormalMap)
    {
        // convert from [0...1] o [-1...1] and calc blue channel of normalmap
        vec3 texNorTS = deriveNormalZ(texture(NormalMap, UV).rg * 2.0 - 1.0);
        // tangent space to world space
        texNorWS = normalize(texNorTS.x * Tangent + texNorTS.y * Bitangent + texNorTS.z * WorldNormal);
        // normalmap intensity
        texNorWS = mix(normalize(WorldNormal), texNorWS, NormalIntensity);
    }
    else
    {
        texNorWS = normalize(WorldNormal);
    }

    if (UseORMTex)
    {
        vec3 orm = texture(ORMTex, UV).rgb;
        
        if (UseBaseColorTex)
        {
            vec4 bca = texture(BaseColorTex, UV);

            defaultSurfaceShader(
                bca.rgb * BaseColor.rgb,  // BaseColor
                Specular,  // Specular
                clamp(mix(Roughness, RoughnessMax, orm.g), 0.0, 1.0),  // Roughness
                orm.b,  // Metallic
                mix(orm.r, 1.0, AO),  // AO
                UseEmissionTex? texture(EmissionTex, UV).rgb * Emission.rgb : Emission.rgb,  // Emission
                bca.a * Opacity,  // Opacity
                texNorWS
            );
        }
        else
        {
            defaultSurfaceShader(
                BaseColor.rgb,  // BaseColor
                Specular,  // Specular
                clamp(mix(Roughness, RoughnessMax, orm.g), 0.0, 1.0),  // Roughness
                orm.b,  // Metallic
                mix(orm.r, 1.0, AO),  // AO
                UseEmissionTex? texture(EmissionTex, UV).rgb * Emission.rgb : Emission.rgb,  // Emission
                Opacity,  // Opacity
                texNorWS
            );
        }
    }
    else
    {
        if (UseBaseColorTex)
        {
            vec4 bca = texture(BaseColorTex, UV);

            defaultSurfaceShader(
                bca.rgb * BaseColor.rgb,  // BaseColor
                Specular,  // Specular
                Roughness,  // Roughness
                Metallic,  // Metallic
                AO,  // AO
                UseEmissionTex? texture(EmissionTex, UV).rgb * Emission.rgb : Emission.rgb,  // Emission
                bca.a * Opacity,  // Opacity
                texNorWS
            );
        }
        else
        {
            defaultSurfaceShader(
                BaseColor.rgb,  // BaseColor
                Specular,  // Specular
                Roughness,  // Roughness
                Metallic,  // Metallic
                AO,  // AO
                UseEmissionTex? texture(EmissionTex, UV).rgb * Emission.rgb : Emission.rgb,  // Emission
                Opacity,  // Opacity
                texNorWS
            );
        }
    }
}