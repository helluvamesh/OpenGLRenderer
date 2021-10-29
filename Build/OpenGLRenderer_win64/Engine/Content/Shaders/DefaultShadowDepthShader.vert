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
uniform mat4 ViewPos;

// OUT
out vec2 UV_geom;
out vec2 UV2_geom;


void defaultShadowDepthShader(vec2 uv, vec2 uv2, vec3 localPos)
{
    UV_geom = VertUV;
    gl_Position = vec4(localPos, 1.0) * ModelMat;
}
