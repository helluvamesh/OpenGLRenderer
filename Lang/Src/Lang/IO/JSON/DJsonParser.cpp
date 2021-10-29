#include "pch.h"
#include "DJsonParser.h"
#include "SJsonUtils.h"
#include "DJsonException.h"
#include "SJsonConvert.h"


void DJsonParser::ResetState()
{
	this->JStr = nullptr;
}


cint DJsonParser::JsonStringToFloatStruct(cint FloatCount, cint Start, float* Dest)
{
	const DString8& jstr = *this->JStr;

	cint i;
	cint progress = Start;
	float* buffer = (float*)_malloca(sizeof(float) * FloatCount);

	i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
	if (i >= 0) progress = i + 1; else { _freea(buffer); throw DJsonException("Expected [", jstr, progress); }
	cint float_i = 0;
	bool arrayClosed = false;
	for (; float_i < FloatCount; ++float_i)
	{
		i = SJsonConvert<float>::JsonStringToItem(jstr, progress, buffer[float_i]);
		if (i >= 0) progress = i; else { _freea(buffer); throw DJsonException("Expected number", jstr, progress); }
		i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
		if (i >= 0) progress = i + 1;
		else 
		{
			i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
			if (i >= 0) 
			{ 
				progress = i + 1; 
				arrayClosed = true;
				break;
			}
			else { _freea(buffer); throw DJsonException("Expected ]", jstr, progress); }
		}
	}

	if (float_i == FloatCount - 1 && arrayClosed)
	{
		for (cint iii = 0; iii < FloatCount; ++iii)
		{
			Dest[iii] = buffer[iii];
		}
	}
	else
	{
		progress = -1;
	}

	_freea(buffer);
	return progress;
}


template<typename OT>
cint DJsonParser::JsonStringToObject(OT* Obj, const DClass& Cls, cint Start)
{
	const DString8& jstr = *this->JStr;

	cint originalStart = Start;
	cint len = jstr.Length();

	cint i;
	cint progress;

	i = SJsonUtils::FindCharThruWhitespace(jstr, Start, '{');
	if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, Start);
	i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
	if (i >= 0) { return i + 1; }

	while (true)
	{
		if (progress < len)
		{
			DString8 key;
			i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, key);
			if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

			i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
			if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

			progress = JsonStringToField(Obj, Cls, key, progress);

			i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
			if (i >= 0)
			{
				progress = i + 1;  // keep looking for fields
			}
			else
			{
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
				if (i >= 0)
				{
					return i + 1;  // found end of the object
				}
				else
				{
					throw DJsonException("Expected }", jstr, progress);
				}
			}
		}
	}
}


template<typename OT>
cint DJsonParser::JsonStringToField(OT* Obj, const DClass& Cls, const DString8& key, cint Start)
{
	cint i;
	cint progress = Start;
	const DString8& jstr = *this->JStr;

	const DField* f = Cls.FindField(key);
	if (f)
	{
		switch (f->ArchType)
		{
		case EArchType::BOOL:
		{
			const DBoolField<OT>* field = static_cast<const DBoolField<OT>*>(f);
			i = SJsonConvert<bool>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected boolean", jstr, progress);
			break;
		}
		case EArchType::ENUM:
		{
			const DEnumField<OT, uint8>* field = static_cast<const DEnumField<OT, uint8>*>(f);
			DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
			i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(enumClass, jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
			break;
		}
		case EArchType::INT:
		{
			const DIntField<OT>* field = static_cast<const DIntField<OT>*>(f);
			i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
			break;
		}
		case EArchType::INT64:
		{
			const DInt64Field<OT>* field = static_cast<const DInt64Field<OT>*>(f);
			i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
			break;
		}
		case EArchType::FLOAT:
		{
			const DFloatField<OT>* field = static_cast<const DFloatField<OT>*>(f);
			i = SJsonConvert<float>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
			break;
		}
		case EArchType::DOUBLE:
		{
			const DDoubleField<OT>* field = static_cast<const DDoubleField<OT>*>(f);
			i = SJsonConvert<double>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
			break;
		}
		case EArchType::STRING:
		{
			const DStringField<OT>* field = static_cast<const DStringField<OT>*>(f);
			DString8 u8str;
			i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8str);
			if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
			*field->GetPtr(Obj) = DStrings::UTF8ToUTF16(u8str);
			break;
		}
		case EArchType::STRING8:
		{
			const DString8Field<OT>* field = static_cast<const DString8Field<OT>*>(f);
			i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, *field->GetPtr(Obj));
			if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
			break;
		}
		case EArchType::FLOAT_STRUCT:
		{
			const DFloatStructField<OT, float>* field = static_cast<const DFloatStructField<OT, float>*>(f);
			cint floatCount = field->GetByteCount() / CINT_SIZEOF(float);
			float* floats = field->GetPtr(Obj);
			i = JsonStringToFloatStruct(floatCount, progress, floats);
			if (i >= 0) progress = i; else throw DJsonException("Expected float array", jstr, progress);
			break;
		}
		case EArchType::STRUCT:
		{
			const DStructField<OT, DStruct>* field = static_cast<const DStructField<OT, DStruct>*>(f);
			i = JsonStringToObject<DStruct>(field->GetPtr(Obj), field->GetStructClass(), progress);
			if (i >= 0) progress = i; else throw DJsonException("Expected object", jstr, progress);
			break;
		}
		case EArchType::ARRAY:
		{
			switch (f->ItemType)
			{
			case EItemType::BOOL:
			{
				auto* field = static_cast<const DArrayField<OT, bool>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					bool* value = field->AddDefault(Obj);
					i = SJsonConvert<bool>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected boolean", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::ENUM:
			{
				auto* field = static_cast<const DArrayField<OT, uint8>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
				field->Resize(Obj, 0);
				while (true)
				{
					uint8* value = field->AddDefault(Obj);
					i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(enumClass, jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::INT:
			{
				auto* field = static_cast<const DArrayField<OT, int32>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					int32* value = field->AddDefault(Obj);
					i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::INT64:
			{
				auto* field = static_cast<const DArrayField<OT, int64>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					int64* value = field->AddDefault(Obj);
					i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::FLOAT:
			{
				auto* field = static_cast<const DArrayField<OT, float>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					float* value = field->AddDefault(Obj);
					i = SJsonConvert<float>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::DOUBLE:
			{
				auto* field = static_cast<const DArrayField<OT, double>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					double* value = field->AddDefault(Obj);
					i = SJsonConvert<double>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::STRING:
			{
				auto* field = static_cast<const DArrayField<OT, DString>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					DString8 u8str;
					i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8str);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					DString* value = field->AddDefault(Obj);
					*value = DStrings::UTF8ToUTF16(u8str);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::STRING8:
			{
				auto* field = static_cast<const DArrayField<OT, DString8>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Resize(Obj, 0);
				while (true)
				{
					DString8* value = field->AddDefault(Obj);
					i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, *value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::FLOAT_STRUCT:
			{
				auto* field = static_cast<const DArrayField<OT, float>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
				field->Resize(Obj, 0);
				while (true)
				{
					i = JsonStringToFloatStruct(floatCount, progress, field->AddDefault(Obj));
					if (i >= 0) progress = i; else throw DJsonException("Expected float array", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EItemType::STRUCT:
			{
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }

				auto* field = static_cast<const DStructArrayField<OT, DStruct>*>(f);
				cint structByteCount = field->GetItemByteCount();
				const DClass& structClass = field->GetStructClass();
				field->Resize(Obj, 0);
				while (true)
				{
					i = JsonStringToObject<DStruct>(field->AddDefault(Obj), structClass, progress);
					if (i >= 0) progress = i; else throw DJsonException("Expected object", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			}
			break;
		}
		case EArchType::SET:
		{
			switch (f->KeyType)
			{
			case EKeyType::ENUM:
			{
				auto* field = static_cast<const DSetField<OT, uint8>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				const char* id = field->GetTypeName();
				field->Clear(Obj);
				DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetTypeName());
				while (true)
				{
					uint8 value;
					i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(enumClass, jstr, progress, value);
					field->Add(Obj, value);
					if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EKeyType::INT:
			{
				auto* field = static_cast<const DSetField<OT, int32>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Clear(Obj);
				while (true)
				{
					int32 value;
					i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, value);
					field->Add(Obj, value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EKeyType::INT64:
			{
				auto* field = static_cast<const DSetField<OT, int64>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Clear(Obj);
				while (true)
				{
					int64 value;
					i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, value);
					field->Add(Obj, value);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EKeyType::STRING:
			{
				auto* field = static_cast<const DSetField<OT, DString>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Clear(Obj);
				while (true)
				{
					DString8 u8str;
					i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8str);
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					field->Add(Obj, DStrings::UTF8ToUTF16(u8str));
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			case EKeyType::STRING8:
			{
				auto* field = static_cast<const DSetField<OT, DString8>*>(f);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '[');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected [", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
				if (i >= 0) { progress = i + 1; break; }
				field->Clear(Obj);
				while (true)
				{
					DString8 value;
					i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, value);
					field->Add(Obj, std::move(value));
					if (i >= 0) progress = i; else throw DJsonException("Expected number", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
					if (i >= 0) progress = i + 1; 
					else 
					{
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
						if (i >= 0) { progress = i + 1; break; }
						else throw DJsonException("Expected ]", jstr, progress);
					}
				}
				break;
			}
			}
			break;
		}
		case EArchType::MAP:
		{
			switch (f->KeyType)
			{
			case EKeyType::ENUM:
			{
				switch (f->ItemType)
				{
				case EItemType::BOOL:
				{
					const DMapField<OT, uint8, bool>* field = static_cast<const DMapField<OT, uint8, bool>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						bool* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<bool>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::ENUM:
				{
					const DMapField<OT, uint8, uint8>* field = static_cast<const DMapField<OT, uint8, uint8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					DEnumClass* itemEnumClass = DEnumClass::FindEnumClass(field->GetItemTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						uint8* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(itemEnumClass, jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT:
				{
					const DMapField<OT, uint8, int32>* field = static_cast<const DMapField<OT, uint8, int32>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int32* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT64:
				{
					const DMapField<OT, uint8, int64>* field = static_cast<const DMapField<OT, uint8, int64>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int64* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT:
				{
					const DMapField<OT, uint8, float>* field = static_cast<const DMapField<OT, uint8, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<float>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::DOUBLE:
				{
					const DMapField<OT, uint8, double>* field = static_cast<const DMapField<OT, uint8, double>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						double* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<double>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING:
				{
					const DMapField<OT, uint8, DString>* field = static_cast<const DMapField<OT, uint8, DString>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8 u8value;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8value);
						DString* value = field->AddDefault(Obj, mapKey);
						*value = DStrings::UTF8ToUTF16(u8value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING8:
				{
					const DMapField<OT, uint8, DString8>* field = static_cast<const DMapField<OT, uint8, DString8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8* value = field->AddDefault(Obj, mapKey);
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT_STRUCT:
				{
					const DMapField<OT, uint8, float>* field = static_cast<const DMapField<OT, uint8, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToFloatStruct(floatCount, progress, value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRUCT:
				{
					const DStructMapField<OT, uint8, DStruct>* field = static_cast<const DStructMapField<OT, uint8, DStruct>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* keyEnumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						uint8 mapKey;
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(keyEnumClass, jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DStruct* struct_ = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToObject<DStruct>(struct_, field->GetItemStructClass(), progress);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				}
				break;
			}
			case EKeyType::STRING:
			{
				switch (f->ItemType)
				{
				case EItemType::BOOL:
				{
					const DMapField<OT, DString, bool>* field = static_cast<const DMapField<OT, DString, bool>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						bool* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<bool>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::ENUM:
				{
					const DMapField<OT, DString, uint8>* field = static_cast<const DMapField<OT, DString, uint8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						uint8* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(enumClass, jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT:
				{
					const DMapField<OT, DString, int32>* field = static_cast<const DMapField<OT, DString, int32>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int32* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT64:
				{
					const DMapField<OT, DString, int64>* field = static_cast<const DMapField<OT, DString, int64>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int64* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT:
				{
					const DMapField<OT, DString, float>* field = static_cast<const DMapField<OT, DString, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<float>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::DOUBLE:
				{
					const DMapField<OT, DString, double>* field = static_cast<const DMapField<OT, DString, double>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						double* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<double>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING:
				{
					const DMapField<OT, DString, DString>* field = static_cast<const DMapField<OT, DString, DString>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8 u8value;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8value);
						DString* value = field->AddDefault(Obj, std::move(mapKey));
						*value = DStrings::UTF8ToUTF16(u8value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING8:
				{
					const DMapField<OT, DString, DString8>* field = static_cast<const DMapField<OT, DString, DString8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT_STRUCT:
				{
					const DMapField<OT, DString, float>* field = static_cast<const DMapField<OT, DString, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToFloatStruct(floatCount, progress, value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRUCT:
				{
					const DStructMapField<OT, DString, DStruct>* field = static_cast<const DStructMapField<OT, DString, DStruct>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 u8mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8mapKey);
						DString mapKey = DStrings::UTF8ToUTF16(u8mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DStruct* struct_ = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToObject<DStruct>(struct_, field->GetItemStructClass(), progress);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				}
				break;
			}
			case EKeyType::STRING8:
			{
				switch (f->ItemType)
				{
				case EItemType::BOOL:
				{
					const DMapField<OT, DString8, bool>* field = static_cast<const DMapField<OT, DString8, bool>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						bool* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<bool>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::ENUM:
				{
					const DMapField<OT, DString8, uint8>* field = static_cast<const DMapField<OT, DString8, uint8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					DEnumClass* enumClass = DEnumClass::FindEnumClass(field->GetKeyTypeName());
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						uint8* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<UINT8_ENUM>::JsonStringToItem(enumClass, jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT:
				{
					const DMapField<OT, DString8, int32>* field = static_cast<const DMapField<OT, DString8, int32>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int32* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<int32>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::INT64:
				{
					const DMapField<OT, DString8, int64>* field = static_cast<const DMapField<OT, DString8, int64>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						int64* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<int64>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT:
				{
					const DMapField<OT, DString8, float>* field = static_cast<const DMapField<OT, DString8, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<float>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::DOUBLE:
				{
					const DMapField<OT, DString8, double>* field = static_cast<const DMapField<OT, DString8, double>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						double* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<double>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING:
				{
					const DMapField<OT, DString8, DString>* field = static_cast<const DMapField<OT, DString8, DString>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8 u8value;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, u8value);
						DString* value = field->AddDefault(Obj, std::move(mapKey));
						*value = DStrings::UTF8ToUTF16(u8value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRING8:
				{
					const DMapField<OT, DString8, DString8>* field = static_cast<const DMapField<OT, DString8, DString8>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DString8* value = field->AddDefault(Obj, std::move(mapKey));
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, *value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::FLOAT_STRUCT:
				{
					const DMapField<OT, DString8, float>* field = static_cast<const DMapField<OT, DString8, float>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					cint floatCount = field->GetItemByteCount() / CINT_SIZEOF(float);
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						float* value = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToFloatStruct(floatCount, progress, value);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				case EItemType::STRUCT:
				{
					const DStructMapField<OT, DString8, DStruct>* field = static_cast<const DStructMapField<OT, DString8, DStruct>*>(f);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '{');
					if (i >= 0) progress = i + 1; else throw DJsonException("Expected {", jstr, progress);
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
					if (i >= 0) { progress = i + 1; break; }
					field->Clear(Obj);
					while (true)
					{
						DString8 mapKey;
						i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
						if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

						DStruct* struct_ = field->AddDefault(Obj, std::move(mapKey));
						i = JsonStringToObject<DStruct>(struct_, field->GetItemStructClass(), progress);
						if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);

						i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
						if (i >= 0) progress = i + 1; 
						else 
						{
							i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
							if (i >= 0) { progress = i + 1; break; }
							else throw DJsonException("Expected }", jstr, progress);
						}
					}
					break;
				}
				}
				break;
			}
			}
			break;
		}
		default:
		{
			i = Skip(progress);
			if (i >= 0) progress = i; else throw DJsonException("Failed to skip json", jstr, i);
		}
		}
	}
	else
	{
		i = Skip(progress);
		if (i >= 0) progress = i; else throw DJsonException("Failed to skip json", jstr, i);
	}

	return progress;	
}


cint DJsonParser::Skip(cint Start)
{
	const DString8& jstr = *this->JStr;

	cint len = jstr.Length();
	for (cint progress = Start; progress < len; ++progress)
	{
		char8 c = jstr[progress];
		if (SAscii::IsWhitespace(c)) continue;

		else if (c == '{') 
		{
			cint i;
			progress++;

			i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');  // empty map
			if (i >= 0) { return i + 1; }

			while (true)
			{
				DString8 mapKey;
				i = SJsonConvert<DString8>::JsonStringToItem(jstr, progress, mapKey);
				if (i >= 0) progress = i; else throw DJsonException("Expected string", jstr, progress);
				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ':');
				if (i >= 0) progress = i + 1; else throw DJsonException("Expected :", jstr, progress);

				i = Skip(progress);
				if (i >= 0) progress = i; else throw DJsonException("Failed to skip json", jstr, i);

				i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ',');
				if (i >= 0) progress = i + 1; 
				else 
				{
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, '}');
					if (i >= 0) { progress = i + 1; break; }
					else throw DJsonException("Expected }", jstr, progress);
				}
			}
			return progress;
		}
		else if (c == '[') 
		{
			cint i;
			progress++;

			i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');  // empty array
			if (i >= 0) { return i + 1; }

			while (true)
			{
				i = Skip(progress);
				if (i >= 0) progress = i; else throw DJsonException("Failed to skip json", jstr, i);
				i = SJsonUtils::FindCharThruWhitespace(jstr, i, ',');
				if (i >= 0) progress = i + 1; 
				else 
				{
					i = SJsonUtils::FindCharThruWhitespace(jstr, progress, ']');
					if (i >= 0) { progress = i + 1; break; }
					else throw DJsonException("Expected ]", jstr, progress);
				}
			}
			return progress;
		}
		else if (c == '"')
		{
			DString8 value;
			return SJsonConvert<DString8>::JsonStringToItem(jstr, Start, value);
		}
		else if (SAscii::IsDigit(c) || c == '.' || c == '-') 
		{
			float value;
			return SJsonConvert<float>::JsonStringToItem(jstr, Start, value);
		}
		else if (c == 'f')
		{
			return SJsonUtils::ParseKeyword(jstr, Start, SJsonUtils::FALSE_STR);
		}
		else if (c == 't')
		{
			return SJsonUtils::ParseKeyword(jstr, Start, SJsonUtils::TRUE_STR);
		}
		else if (c == 'n')
		{
			return SJsonUtils::ParseKeyword(jstr, Start, SJsonUtils::NULL_STR);
		}
		else
		{
			throw new DJsonException("Failed to find json type", jstr, progress);
		}
	}
	return -1;
}


bool DJsonParser::ParseObject(shared<HObject> Obj, const DString8& JsonString, cint Start)
{
	this->ResetState();
	this->JStr = &JsonString;
	try
	{
		this->JsonStringToObject(Obj.Ptr(), Obj->GetClass(), Start);
		this->ResetState();
		return true;
	}
	catch (DJsonException e)
	{
		SLog::Error(e.what());
		SLog::Error(e.ErrorPartString);
		this->ResetState();
		return false;
	}
}


bool DJsonParser::ParseStruct(DStruct& Obj, const DClass& Cls, const DString8& JsonString, cint Start)
{
	this->ResetState();
	this->JStr = &JsonString;
	try
	{
		this->JsonStringToObject(&Obj, Cls, Start);
		this->ResetState();
		return true;
	}
	catch (DJsonException e)
	{
		SLog::Error(e.what());
		SLog::Error(e.ErrorPartString);
		this->ResetState();
		return false;
	}
}


bool DJsonParser::ParseObjectField(shared<HObject> Obj, const DString8& FieldName, const DString8& JsonString, cint Start)
{
	this->ResetState();
	this->JStr = &JsonString;
	try
	{
		this->JsonStringToField(Obj.Ptr(), Obj->GetClass(), FieldName, Start);
		this->ResetState();
		return true;
	}
	catch (DJsonException e)
	{
		SLog::Error(e.what());
		SLog::Error(e.ErrorPartString);
		this->ResetState();
		return false;
	}
}


bool DJsonParser::ParseStructField(DStruct& Obj, const DClass& Cls, const DString8& FieldName, const DString8& JsonString, cint Start)
{
	this->ResetState();
	this->JStr = &JsonString;
	try
	{
		this->JsonStringToField(&Obj, Cls, FieldName, Start);
		this->ResetState();
		return true;
	}
	catch (DJsonException e)
	{
		SLog::Error(e.what());
		SLog::Error(e.ErrorPartString);
		this->ResetState();
		return false;
	}
}
