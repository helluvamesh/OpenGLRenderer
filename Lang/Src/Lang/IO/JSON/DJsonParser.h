/*
* SJsonParser
* - converts a utf-8 encoded JSON string to C++ data types
* - JSON specification: https://www.json.org/json-en.html
* - uses reflection system, read ReflectionSystem.txt
* - missing fields in the reflection layout are skipped
* - failing to parse anythig as a type declared in the reflection layout causes overall failure
* - if a JSON object represents a DMap, heterogenous entries cause failure
* - heterogenous JSON arrays cause failure
* - enums are represented as string
*/

#pragma once

#include "SJsonUtils.h"
#include "DJsonException.h"
#include "SJsonConvert.h"


class DJsonParser
{
private:
	
	const DString8* JStr = nullptr;


	cint JsonStringToFloatStruct(cint FloatCount, cint Start, float* Dest);

	template<typename OT>
	cint JsonStringToObject(OT* Obj, const DClass& Cls, cint Start);

	template<typename OT>
	cint JsonStringToField(OT* Obj, const DClass& Cls, const DString8& key, cint Start);

	cint Skip(cint Start);

	void ResetState();

public:

	// returns success
	bool ParseObject(shared<HObject> Obj, const DString8& JsonString, cint Start = 0);

	// returns success
	bool ParseStruct(DStruct& Obj, const DClass& Cls, const DString8& JsonString, cint Start = 0);

	// returns success
	bool ParseObjectField(shared<HObject> Obj, const DString8& FieldName, const DString8& JsonString, cint Start = 0);

	// returns success
	bool ParseStructField(DStruct& Obj, const DClass& Cls, const DString8& FieldName, const DString8& JsonString, cint Start = 0);

};