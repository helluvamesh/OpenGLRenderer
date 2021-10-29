#pragma once

#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <filesystem>

#include "../../Src/Lang/Util/CoreTypes.h"
#include "../../Src/Lang/Util/IInterface.h"
#include "../../Src/Lang/Util/Enumerators.h"
#include "../../Src/Lang/Util/SCounterUtil.h"
#include "../../Src/Lang/Util/Allocators.h"
#include "../../Src/Lang/Math/FMath.h"
#include "../../Src/Lang/Math/DMath.h"
#include "../../Src/Lang/Util/Exception/DRuntimeException.h"
#include "../../Src/Lang/Util/Exception/DConcurrentModificationException.h"
#include "../../Src/Lang/Util/Exception/DIndexException.h"
#include "../../Src/Lang/Util/DArrayView.h"
#include "../../Src/Lang/Util/SmartPointers.h"
#include "../../Src/Lang/Util/DFixedArray.h"
#include "../../Src/Lang/Util/DArray.h"
#include "../../Src/Lang/Util/DMap.h"
#include "../../Src/Lang/Util/DSet.h"
#include "../../Src/Lang/Util/DLinkedList.h"
#include "../../Src/Lang/Util/TChar.h"
#include "../../Src/Lang/Util/SAscii.h"
#include "../../Src/Lang/Util/DStringBase.h"
#include "../../Src/Lang/Util/DString8.h"
#include "../../Src/Lang/Util/DString.h"
#include "../../Src/Lang/Util/DNumberFormat.h"
#include "../../Src/Lang/Util/DStrings.h"
#include "../../Src/Lang/Util/DEnumClass.h"
#include "../../Src/Lang/Util/DClass.h"
#include "../../Src/Lang/Util/HObject.h"
#include "../../Src/Lang/Util/DStruct.h"
#include "../../Src/Lang/Util/HRandom.h"
#include "../../Src/Lang/Util/Delegates.h"
#include "../../Src/Lang/Util/SLog.h"
#include "../../Src/Lang/Util/SClock.h"
#include "../../Src/Lang/Util/DDateTime.h"
#include "../../Src/Lang/Util/HTimerManager.h"
#include "../../Src/Lang/Util/SBitSet.h"
#include "../../Src/Lang/IO/SPath.h"
		  		
#include "../../Src/Lang/Math/FVec2.h"
#include "../../Src/Lang/Math/FVec3.h"
#include "../../Src/Lang/Math/FVec4.h"
#include "../../Src/Lang/Math/FQuat.h"
#include "../../Src/Lang/Math/FPlane.h"
#include "../../Src/Lang/Math/FAxisBox.h"
#include "../../Src/Lang/Math/FBox.h"
#include "../../Src/Lang/Math/FSphere.h"
#include "../../Src/Lang/Math/FMat3.h"
#include "../../Src/Lang/Math/FMat4.h"

#include "../../Src/Lang/Math/SColor.h"
		  		
#include "../../Src/Lang/Util/Fields.h"
#include "../../Src/Lang/Util/SFieldSelector.h"

#include "../../Src/Lang/IO/Binary/SPrimitiveCopy.h"
#include "../../Src/Lang/IO/Binary/SBinaryFile.h"
#include "../../Src/Lang/IO/Binary/SStrictSerializer.h"
#include "../../Src/Lang/IO/Binary/SLenientSerializer.h"
#include "../../Src/Lang/IO/Binary/DByteConverter.h"
#include "../../Src/Lang/IO/Binary/DByteReconverter.h"
#include "../../Src/Lang/IO/Binary/SBase64.h"
		  		
#include "../../Src/Lang/IO/Text/STextFile.h"
#include "../../Src/Lang/IO/JSON/DJsonEncoder.h"
#include "../../Src/Lang/IO/JSON/DJsonParser.h"
#include "../../Src/Lang/IO/CSV/DCsvEncoder.h"
#include "../../Src/Lang/IO/CSV/DCsvParser.h"