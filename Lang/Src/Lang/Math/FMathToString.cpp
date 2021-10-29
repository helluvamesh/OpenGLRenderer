#include "pch.h"


DString FAxisBox::ToString() const
{
	return DString::Format(
		TX("FAxisBox(Min({0}, {1}, {2})\n")
		TX("         Max({3}, {4}, {5}))")
		,
		{ 
			this->Min.X, this->Min.Y, this->Min.Z,
			this->Max.X, this->Max.Y, this->Max.Z
		}
	);
}


DString FBox::ToString() const
{
	auto& p = this->Points;
	return DString::Format(
		TX("FBox(({0}, {1}, {2})\n")
		TX("     ({3}, {4}, {5})\n")
		TX("     ({6}, {7}, {8})\n")
		TX("     ({9}, {10}, {11})\n")
		TX("     ({12}, {13}, {14})\n")
		TX("     ({15}, {16}, {17})\n")
		TX("     ({18}, {19}, {20})\n")
		TX("     ({21}, {22}, {23}))\n")
		,
		{ 
			p[0].X, p[0].Y, p[0].Z,
			p[1].X, p[1].Y, p[1].Z,
			p[2].X, p[2].Y, p[2].Z,
			p[3].X, p[3].Y, p[3].Z,
			p[4].X, p[4].Y, p[4].Z,
			p[5].X, p[5].Y, p[5].Z,
			p[6].X, p[6].Y, p[6].Z,
			p[7].X, p[7].Y, p[7].Z
		}
	);
}


DString FVec4::ToString() const
{
	return DString::Format(
		TX("FVec4({0}, {1}, {2}, {3})"),
		{ this->X, this->Y, this->Z, this->W }
	);
}


DString FMat3::ToString() const
{
	return DString::Format(
		TX("FMat3({0}, {1}, {2}\n")
		TX("      {3}, {4}, {5}\n")
		TX("      {6}, {7}, {8}\n")
		,
		{ 
			this->Data[0][0], this->Data[0][1], this->Data[0][2],
			this->Data[1][0], this->Data[1][1], this->Data[1][2],
			this->Data[2][0], this->Data[2][1], this->Data[2][2]
		}
	);
}


DString FMat4::ToString() const
{
	return DString::Format(
		TX("FMat4({0}, {1}, {2}, {3}\n")
		TX("      {4}, {5}, {6}, {7}\n")
		TX("      {8}, {9}, {10}, {11}\n")
		TX("      {12}, {13}, {14}, {15})")
		,
		{ 
			this->Data[0][0], this->Data[0][1], this->Data[0][2], this->Data[0][3],
			this->Data[1][0], this->Data[1][1], this->Data[1][2], this->Data[1][3],
			this->Data[2][0], this->Data[2][1], this->Data[2][2], this->Data[2][3],
			this->Data[3][0], this->Data[3][1], this->Data[3][2], this->Data[3][3]
		}
	);
}


DString FPlane::ToString() const
{
	return DString::Format(
		TX("FPlane({0}, {1}, {2}, {3})"),
		{ this->A, this->B, this->C, this->D }
	);
}


DString FQuat::ToString() const
{
    return DString::Format(
        TX("FQuat({0}, {1}, {2}, w: {3})"),
        { this->X, this->Y, this->Z, this->W }
    );
}


DString FSphere::ToString() const
{
	return DString::Format(
		TX("FSphere(center: ({0}, {1}, {2}), radius: {3})"),
		{ this->Center.X, this->Center.Y, this->Center.Z, this->Radius }
	);
}


DString FVec2::ToString() const
{
	return DString::Format(
		TX("FVec2({0}, {1})"), 
		{ this->X, this->Y }
	);
}


DString FVec3::ToString() const
{
	return DString::Format(
		TX("FVec3({0}, {1}, {2})"),
		{ this->X, this->Y, this->Z }
	);
}
