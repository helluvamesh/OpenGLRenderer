#include "Engine\pch.h"
#include "DWavefrontEncoder.h"


void DWavefrontEncoder::Encode(const DStaticMeshData& Mesh, DStringView ObjectName, DString8& Result)
{
	Result.Clear();

	Result.Append("o ");
	Result.Append(DStrings::UTF16ToUTF8(ObjectName));
	Result.Append("\n");

	for (const DStaticSubMeshData& submesh : Mesh.SubMeshes)
	{
		for (const FVec3& pos : submesh.Positions)
		{
			Result.Append("v ");
			DNumberFormat8::DEFAULT.AppendTo(pos.X, Result);
			Result.Append(" ");
			DNumberFormat8::DEFAULT.AppendTo(pos.Y, Result);
			Result.Append(" ");
			DNumberFormat8::DEFAULT.AppendTo(pos.Z, Result);
			Result.Append("\n");
		}

		for (const FVec3& nor : submesh.Normals)
		{
			Result.Append("vn ");
			DNumberFormat8::DEFAULT.AppendTo(nor.X, Result);
			Result.Append(" ");
			DNumberFormat8::DEFAULT.AppendTo(nor.Y, Result);
			Result.Append(" ");
			DNumberFormat8::DEFAULT.AppendTo(nor.Z, Result);
			Result.Append("\n");
		}

		if (submesh.UVLayers.Length() >= 1)
		{
			for (const FVec2& uv : submesh.UVLayers[0].UVs)
			{
				Result.Append("vt ");
				DNumberFormat8::DEFAULT.AppendTo(uv.X, Result);
				Result.Append(" ");
				DNumberFormat8::DEFAULT.AppendTo(uv.Y, Result);
				Result.Append("\n");
			}
		}

		Result.Append("g ");
		Result.Append(DStrings::UTF16ToAscii(submesh.Name));
		Result.Append("\n");

		// wavefront, against all conventions, starts indexing at 1!
		for (const DStaticMeshTriangle& tri : submesh.Tris)
		{
			Result.Append("f ");
			
			DString8 str0 = DNumberFormat8::DEFAULT.ToString(tri.VertIndex0 + 1);
			Result.Append(str0);
			Result.Append("/");
			Result.Append(str0);
			Result.Append("/");
			Result.Append(str0);
			Result.Append(" ");

			DString8 str1 = DNumberFormat8::DEFAULT.ToString(tri.VertIndex1 + 1);
			Result.Append(str1);
			Result.Append("/");
			Result.Append(str1);
			Result.Append("/");
			Result.Append(str1);
			Result.Append(" ");

			DString8 str2 = DNumberFormat8::DEFAULT.ToString(tri.VertIndex2 + 1);
			Result.Append(str2);
			Result.Append("/");
			Result.Append(str2);
			Result.Append("/");
			Result.Append(str2);

			Result.Append("\n");
		}
	}
	
}
