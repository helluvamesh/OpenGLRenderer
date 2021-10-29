#version 400 compatibility

// IN
layout(location = 0) in vec3 VertLocalPos;
layout(location = 1) in vec3 VertLocalNormal;
layout(location = 2) in vec2 VertUV;
layout(location = 3) in vec3 VertTangent;
layout(location = 4) in vec4 VertColor;
layout(location = 5) in vec2 VertUV2;

uniform mat4 ProjectMat;
uniform mat4 ViewMat;
uniform mat4 ModelMat;
uniform mat4 DirLightSpaceMat;
uniform vec3 ViewPos;

// OUT
out vec3 WorldPos;
out vec3 WorldNormal;
out vec2 UV;
out vec4 VertexColor;
out vec2 UV2;


void defaultUnlitSurfaceShader(vec2 uv, vec2 uv2, vec3 localPos, vec3 localNormal)
{
    WorldPos = vec3(vec4(localPos, 1.0) * ModelMat);
    gl_Position = vec4(WorldPos, 1.0) * ViewMat * ProjectMat;
    UV = uv;
    
    // transform normals to world space
    // removing the scaling from the model matrix
    WorldNormal = localNormal * mat3(inverse(transpose(ModelMat)));
    // alternative: WorldNormal = normalize(vec3(vec4(localNormal, .0) * ModelMat));
    
    VertexColor = VertColor;
    UV2 = uv2;
}