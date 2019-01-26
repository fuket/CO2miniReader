#pragma once

class CO2miniReader
{
public:
	enum CO2miniValueType
	{
		VALUE_TYPE_TEMPERATURE,
		VALUE_TYPE_CO2,
	};

	CO2miniReader();
	~CO2miniReader();

	bool openDevice();
	bool closeDevice();
	bool readValue(const CO2miniValueType type, const unsigned int timeout, double &value);

private:
	class Impl;
	Impl *_impl;
};

