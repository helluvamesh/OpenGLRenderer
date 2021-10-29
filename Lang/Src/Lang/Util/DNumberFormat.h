#pragma once


class DNumberFormat
{
public:
	static const DNumberFormat DEFAULT;
	static const DString NAN_STRING;
	static const DString INF_STRING;
	static const DString NEG_INF_STRING;

protected:
	tchar DecimalSeparator;
	tchar GroupSeparator;
	bool UseGroupSeparator;
	cint MinFloatFractionDigits;
	cint MaxFloatFractionDigits;
	cint MinDoubleFractionDigits;
	cint MaxDoubleFractionDigits;
	cint MinIntegralDigits;


public:
	DNumberFormat();

	tchar GetDecimalSeparator() const;
	void SetDecimalSeparator(tchar V);

	tchar GetGroupSeparator() const;
	tchar GetUseGroupSeparator() const;
	void SetGroupSeparator(tchar V);
	void ClearGroupSeparator();

	cint GetMinFloatFractionDigits() const;
	// [1...18]
	void SetMinFloatFractionDigits(cint V);

	cint GetMinDoubleFractionDigits() const;
	// [1...18]
	void SetMinDoubleFractionDigits(cint V);

	cint GetMaxFloatFractionDigits() const;
	// [1...18]
	void SetMaxFloatFractionDigits(cint V);

	cint GetMaxDoubleFractionDigits() const;
	// [1...18]
	void SetMaxDoubleFractionDigits(cint V);

	cint GetMinIntegralDigits() const;
	// min 1
	void SetMinIntegralDigits(cint V);
	
	void ToString(int32 Value, DString& Result) const;
	DString ToString(int32 Value) const;
	void AppendTo(int32 Value, DString& Result) const;
	
	void ToString(int64 Value, DString& Result) const;
	DString ToString(int64 Value) const;
	void AppendTo(int64 Value, DString& Result) const;
	
	void ToString(uint32 Value, DString& Result) const;
	DString ToString(uint32 Value) const;
	void AppendTo(uint32 Value, DString& Result) const;

	void ToString(uint64 Value, DString& Result) const;
	DString ToString(uint64 Value) const;
	void AppendTo(uint64 Value, DString& Result) const;


	void ToString(int8 Value, DString& Result) const;
	DString ToString(int8 Value) const;
	void AppendTo(int8 Value, DString& Result) const;

	void ToString(int16 Value, DString& Result) const;
	DString ToString(int16 Value) const;
	void AppendTo(int16 Value, DString& Result) const;

	void ToString(uint8 Value, DString& Result) const;
	DString ToString(uint8 Value) const;
	void AppendTo(uint8 Value, DString& Result) const;

	void ToString(uint16 Value, DString& Result) const;
	DString ToString(uint16 Value) const;
	void AppendTo(uint16 Value, DString& Result) const;


	void ToString(float Value, DString& Result) const;
	DString ToString(float Value) const;
	void AppendTo(float Value, DString& Result) const;
	
	void ToString(double Value, DString& Result) const;
	DString ToString(double Value) const;
	void AppendTo(double Value, DString& Result) const;

};


class DNumberFormat8
{
public:
	static const DNumberFormat8 DEFAULT;
	static const DString8 NAN_STRING;
	static const DString8 INF_STRING;
	static const DString8 NEG_INF_STRING;

protected:
	char8 DecimalSeparator;
	char8 GroupSeparator;
	bool UseGroupSeparator;
	cint MinFloatFractionDigits;
	cint MaxFloatFractionDigits;
	cint MinDoubleFractionDigits;
	cint MaxDoubleFractionDigits;
	cint MinIntegralDigits;


public:
	DNumberFormat8();

	char8 GetDecimalSeparator() const;
	void SetDecimalSeparator(char8 V);

	char8 GetGroupSeparator() const;
	char8 GetUseGroupSeparator() const;
	void SetGroupSeparator(char8 V);
	void ClearGroupSeparator();

	cint GetMinFloatFractionDigits() const;
	// [1...18]
	void SetMinFloatFractionDigits(cint V);

	cint GetMinDoubleFractionDigits() const;
	// [1...18]
	void SetMinDoubleFractionDigits(cint V);

	cint GetMaxFloatFractionDigits() const;
	// [1...18]
	void SetMaxFloatFractionDigits(cint V);

	cint GetMaxDoubleFractionDigits() const;
	// [1...18]
	void SetMaxDoubleFractionDigits(cint V);

	cint GetMinIntegralDigits() const;
	// min 1
	void SetMinIntegralDigits(cint V);

	void ToString(int32 Value, DString8& Result) const;
	DString8 ToString(int32 Value) const;
	void AppendTo(int32 Value, DString8& Result) const;
	
	void ToString(int64 Value, DString8& Result) const;
	DString8 ToString(int64 Value) const;
	void AppendTo(int64 Value, DString8& Result) const;

	void ToString(uint32 Value, DString8& Result) const;
	DString8 ToString(uint32 Value) const;
	void AppendTo(uint32 Value, DString8& Result) const;

	void ToString(uint64 Value, DString8& Result) const;
	DString8 ToString(uint64 Value) const;
	void AppendTo(uint64 Value, DString8& Result) const;


	void ToString(int8 Value, DString8& Result) const;
	DString8 ToString(int8 Value) const;
	void AppendTo(int8 Value, DString8& Result) const;

	void ToString(int16 Value, DString8& Result) const;
	DString8 ToString(int16 Value) const;
	void AppendTo(int16 Value, DString8& Result) const;

	void ToString(uint8 Value, DString8& Result) const;
	DString8 ToString(uint8 Value) const;
	void AppendTo(uint8 Value, DString8& Result) const;

	void ToString(uint16 Value, DString8& Result) const;
	DString8 ToString(uint16 Value) const;
	void AppendTo(uint16 Value, DString8& Result) const;


	void ToString(float Value, DString8& Result) const;
	DString8 ToString(float Value) const;
	void AppendTo(float Value, DString8& Result) const;

	void ToString(double Value, DString8& Result) const;
	DString8 ToString(double Value) const;
	void AppendTo(double Value, DString8& Result) const;

};