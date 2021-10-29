#version 400 compatibility

const int MAX_SHADOW_CASCADES = 4;
const int MAX_VERTS = 12;  


// IN
layout(triangles) in;

in vec2 UV_geom[];
in vec2 UV2_geom[];
    
uniform mat4 LightSpaceMats[MAX_SHADOW_CASCADES];
uniform int Layers = 3;

// OUT
layout(triangle_strip, max_vertices = 12) out;  // max_verts = MAX_SHADOW_CASCADES * 3

out vec2 UV;
out vec2 UV2;


void defaultShadowDepthShader()
{   
    for (int layer_i = 0; layer_i < Layers; ++layer_i)
    {     
        for (int vert_i = 0; vert_i < 3; ++vert_i)
        {
            gl_Layer = layer_i;
            gl_Position = gl_in[vert_i].gl_Position * LightSpaceMats[layer_i];
            UV = UV_geom[vert_i];
            UV2 = UV_geom[vert_i];
            EmitVertex();
        }
        EndPrimitive();
    }
}
