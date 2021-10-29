#include "Engine/pch.h"
#include "HSceneQuad.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"


HSceneQuad::HSceneQuad()
{
    float verts[20] {
        
        // positions           // uvs
        -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,    1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,    1.0f, 0.0f
    };

    uint32 indices[6] {
        1, 2, 0,
        1, 3, 2
    };

    this->VertBufferLength = 20;
    this->IndexBufferLength = 6;

    int32 positionCount = 12;
    int32 uvCount = 8;

    // VERTEX ARRAY
    glCheck(glGenVertexArrays(1, &this->VertArrayId));
    glCheck(glBindVertexArray(this->VertArrayId));

    // VERTEX BUFFER
    glCheck(glGenBuffers(1, &this->VertBufferId));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, this->VertBufferId));
    glCheck(glBufferData(GL_ARRAY_BUFFER, this->VertBufferLength * sizeof(float), &verts, GL_STATIC_DRAW));

    // POSITION ATTRIBUTE
    glCheck(glEnableVertexAttribArray(0));
    glCheck(glVertexAttribPointer(
        0,                  // attribute index: position
        3,                  // size: 3d location (3 floats)
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [0.0...1.0]
        5 * sizeof(float),  // stride: bytes to jump to get to the next vertex (all attributes included)
        (void*)(intptr_t)0  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // UV ATTRIBUTE
    glCheck(glEnableVertexAttribArray(1));
    glCheck(glVertexAttribPointer(
        1,                  // attribute index: uv
        2,                  // size: 2d vector (2 floats)
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [0.0...1.0]
        5 * sizeof(float),  // stride: bytes to jump to get to the next item (all attributes included)
        (void*)(intptr_t)(3 * sizeof(float))  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // INDEX BUFFER
    glCheck(glGenBuffers(1, &this->IndexBufferId));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferId));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferLength * sizeof(uint32), &indices, GL_STATIC_DRAW));

    glCheck(glBindVertexArray(0));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


HSceneQuad::~HSceneQuad()
{
    glCheck(glDeleteVertexArrays(1, &this->VertArrayId));
}


void HSceneQuad::Draw() const
{
    glCheck(glBindVertexArray(this->VertArrayId));
    
    glCheck(glDrawElements(GL_TRIANGLES, this->IndexBufferLength, GL_UNSIGNED_INT, nullptr));
}