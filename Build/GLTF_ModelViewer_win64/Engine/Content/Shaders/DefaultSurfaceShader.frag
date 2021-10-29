#version 400 compatibility

const float PI = 3.14159265359;
const int MAX_SHADOW_CASCADES = 4;
const int MAX_DIR_LIGHT_COUNT = 4;
const int MAX_POINT_LIGHT_COUNT = 10;
const int MAX_SPOT_LIGHT_COUNT = 10;
const int MAX_SHADOWED_SPOT_LIGHTS = 4;


// IN
in vec3 WorldPos;
in vec3 WorldNormal;
in vec2 UV;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 VertexColor;
in vec2 UV2;
in mat4 ViewMatrix;

uniform vec3 ViewPos;
uniform int BlendMode = 0;
uniform float BloomStart = 2;

// ambient lighting

uniform samplerCube IrrMap;  // irradiance
uniform samplerCube RefMap;  // prefiltered reflections
uniform vec3 AmbientColor = vec3(1.0, 1.0, 1.0);
uniform float DownOcclusion = 1.0;

// direct ligts

struct DirLight
{
    vec3 Direction;  // should be inverted and normalized on CPU side
    vec3 Color;
};
uniform int ShadowedDirLightIdx = -1;
uniform sampler2DArray DirLightShadowMap;
uniform mat4 DirLightSpaceMats[MAX_SHADOW_CASCADES];
uniform int DirLightCascadeCount = 3;
uniform float DirLightCascadeDistances[MAX_SHADOW_CASCADES];
uniform float DirLightShadowBiasesMin[MAX_SHADOW_CASCADES];
uniform float DirLightShadowBiasesMax[MAX_SHADOW_CASCADES];

struct PointLight
{
    vec3 Position;
    vec3 Color;
};

struct SpotLight
{
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    vec2 Cosines;  // cos of inner and outer cone angle
    int ShadowIdx;
};

uniform sampler2DArray SpotShadowMap_0;
uniform sampler2DArray SpotShadowMap_1;
uniform sampler2DArray SpotShadowMap_2;
uniform sampler2DArray SpotShadowMap_3;

uniform mat4 SpotLightSpaceMats[MAX_SHADOWED_SPOT_LIGHTS];
uniform float SpotShadowBiasesMin[MAX_SHADOWED_SPOT_LIGHTS];
uniform float SpotShadowBiasesMax[MAX_SHADOWED_SPOT_LIGHTS];

uniform DirLight[MAX_DIR_LIGHT_COUNT] DirLights = DirLight[](
    // direction, color
    DirLight(vec3(.0), vec3(.0)),
    DirLight(vec3(.0), vec3(.0)),
    DirLight(vec3(.0), vec3(.0)),
    DirLight(vec3(.0), vec3(.0))
);
uniform int DirLightCount = 0;

uniform PointLight[MAX_POINT_LIGHT_COUNT] PointLights = PointLight[](
    // position, color
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0)),
    PointLight(vec3(.0), vec3(.0))
);
uniform int PointLightCount = 1;

uniform SpotLight[MAX_SPOT_LIGHT_COUNT] SpotLights = SpotLight[](
    // position, direction, color, cone cosines
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1),
    SpotLight(vec3(.0), vec3(.0), vec3(.0), vec2(.0), -1)
);
uniform int SpotLightCount = 1;

// OUT
layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutBrightColor;  // for bloom


float fresnel(float ior, float d)
{
    return ior + (1.0 - ior) * pow(2.0, (-5.55473 * d - 6.98361) * d);
}

float fresnelRough(float ior, float d, float roughness)
{
    return (ior + (max(pow(1.0 - roughness, 2), ior) - ior) * pow(2, (-5.55473 * d - 6.98361) * d));
}

// reflections
vec3 calcIndirectSpecular(
    vec3 n, vec3 v,  // normal, view
    vec3 baseColor, float specular, float roughness, float metallic, float ao
)
{
    vec3 reflectionMap = textureLod(
        RefMap, 
        reflect(-v, n),
        roughness * 4
    ).rgb;
    float dot_nv = max(dot(n, v), 0.0);
    float f = fresnelRough(mix(0.04, 0.0, metallic), dot_nv, roughness);
    vec3 reflectionMetal = mix(
        mix(vec3(baseColor), vec3(1.0), f), 
        vec3(baseColor), 
        min(roughness * 2.0, 1.0)
    );
    vec3 reflectionNonMetal = vec3(f) * specular * 2;
    
    return reflectionMap * mix(reflectionNonMetal, reflectionMetal, metallic) * AmbientColor * ao;
}

vec3 calcDirectSpecular(
    vec3 n, vec3 v, vec3 l, float dot_nl,  // normal, view, light
    vec3 baseColor, float specular, float roughness, float metallic, float ao,
    vec3 lightColor
)
{
    vec3 h = normalize(v + l);

    float a2 = pow(roughness, 4);
    float dot_nh = max(dot(n, h), .0);
    float D = a2 / (PI * pow((dot_nh * dot_nh * (a2 - 1.0) + 1.0), 2));

    float dot_vh = max(dot(v, h), .0);
    float F = fresnel(mix(.04, .5, metallic), dot_vh);

    float k = pow((roughness + 1.0), 2) / 8.0;
    float dot_nv = max(dot(n, v), .0);
    
    float G = (dot_nv / (dot_nv * (1.0 - k) + k))  *  (dot_nl / (dot_nl * (1.0 - k) + k));
    float s = max((D * F * G) / (4.0 * dot_nl * dot_nv), .0) * dot_nl * (mix(specular, 1, metallic));
    
    vec3 color = lightColor * s;
    return mix(color, vec3(baseColor) * color, metallic) * ao;        
}

// ambient light
vec3 calcIndirectDiffuse(vec3 baseColor, float metallic, vec3 n, float ao)
{
    vec3 irr = texture(IrrMap, n).rgb;
    irr = pow(irr, vec3(.455));
    vec3 ambient = baseColor * irr * AmbientColor * ao;
    return mix(ambient, vec3(.0), metallic);
}

vec3 calcDirectDiffuse(vec3 bc, vec3 lightColor, float dot_nl)
{
    return bc * lightColor * dot_nl;
}

float calcShadowSpot(float dot_nl, mat4 lightSpaceMat, float biasMin, float biasMax, sampler2DArray shadowMap)
{
    vec4 posLightSpace_v4 = vec4(WorldPos, 1.0) * lightSpaceMat;
    vec3 posLightSpace = posLightSpace_v4.xyz / posLightSpace_v4.w;
    // to [0...1] range
    posLightSpace = posLightSpace * .5 + .5;
    // outside of shadow frustum
    if (posLightSpace.z > 1.0) return 1.0;

    float depthLightSpace = posLightSpace.z;
    if (depthLightSpace > 1.0) return 1.0;
    // apply more bias to surface facing away from the light
    float bias = max(biasMax * (1.0 - dot_nl), biasMin);
    // percentage-closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0).xy;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depthShadowMap = texture(
                shadowMap, 
                vec3(posLightSpace.xy + vec2(x, y) * texelSize, 0)
            ).r; 
            shadow += depthLightSpace - bias > depthShadowMap ? .0 : 1.0;        
        }
    }
    shadow /= 9.0;
    return shadow;
}

float calcShadowCSM(float dot_nl)
{
    vec4 posViewSpace = vec4(WorldPos, 1.0) * ViewMatrix;
    float depthViewSpace = abs(posViewSpace.z);
    // select cascade
    int layer = -1;
    for (int i = 0; i < DirLightCascadeCount; ++i)
    {
        if (depthViewSpace < DirLightCascadeDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1) return 1.0;

    vec4 posLightSpace_v4 = vec4(WorldPos, 1.0) * DirLightSpaceMats[layer];
    vec3 posLightSpace = posLightSpace_v4.xyz / posLightSpace_v4.w;
    // to [0...1] range
    posLightSpace = posLightSpace * 0.5 + 0.5;
    // outside of shadow frustum
    if (posLightSpace.z > 1.0) return 1.0;

    float depthLightSpace = posLightSpace.z;
    if (depthLightSpace > 1.0) return 1.0;
    // apply more bias to surface facing away from the light
    float bias = max(DirLightShadowBiasesMax[layer] * (1.0 - dot_nl), DirLightShadowBiasesMin[layer]);
    // percentage-closer filtering
    float shadow = .0;
    vec2 texelSize = 1.0 / vec2(textureSize(DirLightShadowMap, 0));
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depthShadowMap = texture(
                DirLightShadowMap,
                vec3(posLightSpace.xy + vec2(x, y) * texelSize, layer)
            ).r; 
            shadow += (depthLightSpace - bias) > depthShadowMap ? .0 : 1.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

void defaultSurfaceShader(
    vec3 baseColor,
    float specular,
    float roughness,
    float metallic,
    float ao,
    vec3 emission,
    float opacity,
    vec3 n  // should be normalized outside
)
{
    baseColor = clamp(baseColor, vec3(.0), vec3(1.0));
    specular = clamp(specular, .0, 1.0);
    roughness = clamp(roughness, .04, 1.0);
    metallic = clamp(metallic, .0, 1.0);
    ao = clamp(ao, .0, 1.0);
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

    vec3 v = normalize(ViewPos - WorldPos);
    
    vec3 bc = vec3(baseColor) / PI;
    
    vec3 color = vec3(.0);

    // ------------------ INDIRECT LIGHTING -------------------

    if (!alphaBlend || BlendMode == 2)  // ALPHA_BLEND step 1
    {
        // indirect diffuse
        color = calcIndirectDiffuse(baseColor, metallic, n, ao);
    }

    if (!alphaBlend || BlendMode == 3)  // ALPHA_BLEND step 2
    {
        // indirect specular
        color += calcIndirectSpecular(
            n, v, 
            baseColor, specular, roughness, metallic, ao
        );
    }

    // darken surface facing down
    float indirectLightMask = 1.0 - clamp(-n.y, .0, 1.0);
    color *= min(indirectLightMask + DownOcclusion, 1.0);

    // ------------------ DIRECTIONAL LIGHTS -------------------

    for (int i = 0; i < DirLightCount; i++)
    {
        vec3 l = DirLights[i].Direction;
        float dot_nl = max(dot(n, l), 0.0);
        float shadow;
        if (i == ShadowedDirLightIdx)
            shadow = calcShadowCSM(dot_nl);
        else
            shadow = 1.0;
        
        if (!alphaBlend)
        {
            // direct specular
            vec3 directSpecular = calcDirectSpecular(
                n, v, l, dot_nl, 
                bc, specular, roughness, metallic, ao,
                DirLights[i].Color
            );
            directSpecular *= shadow;
            
            // direct diffuse
            vec3 directDiffuse = calcDirectDiffuse(bc, DirLights[i].Color, dot_nl);
            directDiffuse *= shadow;
            
            // combine
            color += mix(directDiffuse + directSpecular, directSpecular * 4 * roughness, metallic);
        }
        else if (BlendMode == 2)  // ALPHA BLEND step 1
        {
            // direct diffuse
            color += calcDirectDiffuse(bc, DirLights[i].Color, dot_nl);
        }
        else if (BlendMode == 3)  // ALPHA BLEND step 2
        {
            // direct specular
            vec3 directSpecular = calcDirectSpecular(
                n, v, l, dot_nl, 
                bc, specular, roughness, metallic, ao,
                DirLights[i].Color
            );
            
            color += mix(directSpecular, directSpecular * 4 * roughness, metallic);
        }
    }

    // ------------------ POINT LIGHTS -------------------

    for (int i = 0; i < PointLightCount; i++)
    {
        vec3 l_unnorm = PointLights[i].Position - WorldPos;
        vec3 l = normalize(l_unnorm);
        float dot_nl = max(dot(n, l), 0.0);
        
        float distance = length(l_unnorm);
        // float radius = 10;
        // float mask = 1 - clamp(d - radius, 0, 1);
        // float attenuation = (1 / (distance * distance)) * mask;
        float attenuation = 1 / (distance * distance);
        vec3 light = PointLights[i].Color * attenuation;

        if (!alphaBlend)
        {
            // direct specular
            vec3 directSpecular = calcDirectSpecular(
                n, v, l, dot_nl, 
                bc, specular, roughness, metallic, ao,
                light
            );
            
            // direct diffuse
            vec3 directDiffuse = calcDirectDiffuse(bc, light, dot_nl);
            
            // combine
            color += mix(directDiffuse + directSpecular, directSpecular * 4 * roughness, metallic);
        }
        else if (BlendMode == 2)  // ALPHA BLEND step 1
        {
            // direct diffuse
            color += calcDirectDiffuse(bc, light, dot_nl);
        }
        else if (BlendMode == 3)  // ALPHA BLEND step 2
        {
            // direct specular
            vec3 directSpecular = calcDirectSpecular(
                n, v, l, dot_nl, 
                bc, specular, roughness, metallic, ao,
                light
            );
            
            color += mix(directSpecular, directSpecular * 4 * roughness, metallic);
        }
    }

    // ------------------ SPOT LIGHTS -------------------
    
    for (int i = 0; i < SpotLightCount; i++)
    {
        vec3 l_unnorm = SpotLights[i].Position - WorldPos;
        vec3 l = normalize(l_unnorm);

        float dot_ls = dot(l, normalize(-SpotLights[i].Direction));
        
        if (dot_ls > SpotLights[i].Cosines.y) 
        {       
            // diff between inner and outer cone angles
            float coneDiff = SpotLights[i].Cosines.x - SpotLights[i].Cosines.y;
            float coneFalloff = clamp((dot_ls - SpotLights[i].Cosines.y) / coneDiff, 0.0, 1.0); 

            float dot_nl = max(dot(n, l), 0.0);
        
            float distance = length(l_unnorm);
            float attenuation = 1.0 / (distance * distance);
            vec3 light = SpotLights[i].Color * attenuation * coneFalloff;

            float shadow;
            switch (SpotLights[i].ShadowIdx)
            {
            case 0:
                shadow = calcShadowSpot(
                    dot_nl, SpotLightSpaceMats[0], SpotShadowBiasesMin[0], SpotShadowBiasesMax[0], SpotShadowMap_0
                );
                break;
            case 1:
                shadow = calcShadowSpot(
                    dot_nl, SpotLightSpaceMats[1], SpotShadowBiasesMin[1], SpotShadowBiasesMax[1], SpotShadowMap_1
                );
                break;
            case 2:
                shadow = calcShadowSpot(
                    dot_nl, SpotLightSpaceMats[2], SpotShadowBiasesMin[2], SpotShadowBiasesMax[2], SpotShadowMap_2
                );
                break;
            case 3:
                shadow = calcShadowSpot(
                    dot_nl, SpotLightSpaceMats[3], SpotShadowBiasesMin[3], SpotShadowBiasesMax[3], SpotShadowMap_3
                );
                break;
            default:
                shadow = 1.0;
            }

            if (!alphaBlend)
            {
                // direct specular
                vec3 directSpecular = calcDirectSpecular(
                    n, v, l, dot_nl, 
                    bc, specular, roughness, metallic, ao,
                    light
                );
                
                // direct diffuse
                vec3 directDiffuse = calcDirectDiffuse(bc, light, dot_nl);
                
                // combine
                color += mix(directDiffuse + directSpecular, directSpecular * 4 * roughness, metallic) * shadow;
            }
            else if (BlendMode == 2)  // ALPHA BLEND step 1
            {
                // direct diffuse
                color += calcDirectDiffuse(bc, light, dot_nl);
            }
            else if (BlendMode == 3)  // ALPHA BLEND step 2
            {
                // direct specular
                vec3 directSpecular = calcDirectSpecular(
                    n, v, l, dot_nl, 
                    bc, specular, roughness, metallic, ao,
                    light
                );
                
                color += mix(directSpecular, directSpecular * 4 * roughness, metallic);
            }
        }
    }

    // --------------------------------------------------

    vec3 finalColor = color + emission;
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
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


    // vec4 posLightSpace = vec4(WorldPos, 1.0) * SpotLightSpaceMats[0];
    // // perform perspective divide
    // vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
    // // from [-1...1] to [0...1]
    // projCoords = projCoords * .5 + .5;
    
    // float pcfDepth = texture(SpotShadowMap_0, vec3(projCoords.xy, 0)).r; 
    // OutColor *= .00001;
    // OutColor += vec4(vec3(pcfDepth), 1);      
}


vec3 deriveNormalZ(vec2 norXY)
{
    return vec3(norXY.x, norXY.y, sqrt(1.0 - (norXY.x * norXY.x + norXY.y * norXY.y)));
}