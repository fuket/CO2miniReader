// Sample.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <iostream>
#include "../CO2miniReader/CO2miniReader.h"

int main()
{
	CO2miniReader reader;

	reader.openDevice();

	{
		double temperature = 0;
		if (!reader.readValue(CO2miniReader::VALUE_TYPE_TEMPERATURE, 5000, temperature))
		{
			wprintf(L"Failed to get temperature value.\n");
			return 0;
		}
		wprintf(L"temp: %.1lf\n", temperature);
	}

	{
		double co2 = 0;
		if (!reader.readValue(CO2miniReader::VALUE_TYPE_CO2, 5000, co2))
		{
			wprintf(L"Failed to get co2 value.\n");
			return 0;
		}
		wprintf(L"co2 : %.1lf\n", co2);
	}

	reader.closeDevice();

	return 0;
}

