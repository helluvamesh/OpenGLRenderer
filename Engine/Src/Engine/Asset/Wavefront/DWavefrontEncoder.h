#pragma once

#include "Engine\Render\Mesh\DStaticMeshData.h"


class DWavefrontEncoder
{
public:

	void Encode(const DStaticMeshData& Mesh, DStringView ObjectName, DString8& Result);

};