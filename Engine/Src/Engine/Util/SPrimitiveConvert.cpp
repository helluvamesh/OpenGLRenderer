#include "Engine\pch.h"
#include "SPrimitiveConvert.h"


/*
* https://stackoverflow.com/questions/3026441/float32-to-float16
*/
int16 SPrimitiveConvert::FloatToHalfFloat(float F)
{
	int32 fltInt32 = *((int32*)&F);
	int16 fltInt16 = (fltInt32 >> 31) << 5;
	uint16 tmp = (fltInt32 >> 23) & 0xff;
	tmp = (tmp - 0x70) & ((uint32)((int32)(0x70 - tmp) >> 4) >> 27);
	fltInt16 = (fltInt16 | tmp) << 10;
	fltInt16 |= (fltInt32 >> 13) & 0x3ff;
	return fltInt16;
}


int16 SPrimitiveConvert::UnitVectorToUInt16(float F)
{
	if (F >= .0f) return (int16)(F * INT16_MAX);
	else return (int16)(F * (INT16_MAX + 1));
}


//int32 PackInt16sToInt32(int16 I1, int16 I2)
//{
//	return (I1 & 0xFFFF) | ((I2 & 0xFFFF) << 16);
//}
