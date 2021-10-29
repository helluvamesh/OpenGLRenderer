#include "Engine\pch.h"
#include "HLineWork.h"
#include "GL\glew.h"
#include "Engine\Render\RenderUtil\GLUtil.h"


// ------------------------ HLineStroke ------------------------------


HLineStroke::HLineStroke(const DLineStrokeData& Data)
{
    this->IndexBufferLength = Data.Indices.Length();

    int32 totalBufferByteCount = Data.Points.Length() * 3 * sizeof(float);

    this->LineWidth = Data.LineWidth;

    // VERTEX ARRAY
    glCheck(glGenVertexArrays(1, &this->VertArrayId));
    glCheck(glBindVertexArray(this->VertArrayId));

    // VERTEX BUFFER
    glCheck(glGenBuffers(1, &this->VertBufferId));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, this->VertBufferId));
    glCheck(glBufferData(GL_ARRAY_BUFFER, totalBufferByteCount, Data.Points.Data(), GL_STATIC_DRAW));

    // POSITION ATTRIBUTE
    glCheck(glEnableVertexAttribArray(0));
    glCheck(glVertexAttribPointer(
        0,                  // attribute index: position
        3,                  // size: 3 floats
        GL_FLOAT,           // data type
        GL_FALSE,           // whether the coords need to be normalized to [-1.0...1.0]
        sizeof(float) * 3,  // stride: bytes to jump to get to the next vertex (all attributes included)
        (void*)(intptr_t)0  // pointer: bytes to jump from the first attribute to get to this attribute
    ));

    // INDEX BUFFER
    glCheck(glGenBuffers(1, &this->IndexBufferId));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferId));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * this->IndexBufferLength, Data.Indices.Data(), GL_STATIC_DRAW));

    glCheck(glBindVertexArray(0));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


HLineStroke::~HLineStroke()
{
    glCheck(glDeleteVertexArrays(1, &this->VertArrayId));
}


void HLineStroke::UpdateData(const DLineStrokeData& Data)
{
    glCheck(glBindVertexArray(this->VertArrayId));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, this->VertBufferId));

    int32 totalBufferByteCount = Data.Points.Length() * 3 * sizeof(float);
    this->LineWidth = Data.LineWidth;
    glCheck(glBufferData(GL_ARRAY_BUFFER, totalBufferByteCount, Data.Points.Data(), GL_STATIC_DRAW));

    this->IndexBufferLength = Data.Indices.Length();
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBufferId));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * this->IndexBufferLength, Data.Indices.Data(), GL_STATIC_DRAW));
    
    glCheck(glBindVertexArray(0));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


void HLineStroke::Draw(HSurfaceMaterial* DrawMaterial)
{
    glCheck(glLineWidth(this->LineWidth));
    glCheck(glBindVertexArray(this->VertArrayId));
    glCheck(glDrawElements(GL_LINES, this->IndexBufferLength, GL_UNSIGNED_INT, nullptr));
}


void HLineStroke::DrawStandalone()
{
}


// ------------------------- HLineWork ------------------------------


void HLineWork::Load()
{
    this->Unload();

	// load from .asset file here
}


bool HLineWork::IsLoaded()
{
    return this->Stroke != nullptr;
}


void HLineWork::Unload()
{
    this->Stroke = nullptr;
}


void HLineWork::Load(const DLineWorkData& Data)
{
    this->Unload();

    this->BoundingBox = Data.BoundingBox;
    this->Stroke = MakeShared<HLineStroke>(Data.Stroke);
}


void HLineWork::UpdateData(const DLineWorkData& Data)
{
    this->BoundingBox = Data.BoundingBox;
    if (this->Stroke)
    {
        this->Stroke->UpdateData(Data.Stroke);
    }
}


const FAxisBox& HLineWork::GetBoundingBox() const
{
    return this->BoundingBox;
}


shared<HSurfaceMaterial> HLineWork::GetMaterial() const
{
    return this->Material;
}
HSurfaceMaterial* HLineWork::GetMaterialPtr() const
{
    return this->Material.Ptr();
}
void HLineWork::SetMaterial(shared<HSurfaceMaterial> NewMaterial)
{
    this->Material = NewMaterial;
}


shared<HLineStroke> HLineWork::GetStroke() const
{
    return this->Stroke;
}
HLineStroke* HLineWork::GetStrokePtr() const
{
    return this->Stroke.Ptr();
}
