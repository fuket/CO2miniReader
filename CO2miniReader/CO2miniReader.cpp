#include "stdafx.h"
#include "CO2miniReader.h"

class CO2miniReader::Impl
{
private:
	typedef unsigned char CO2miniData[8];

	hid_device *_device;
	bool _isApiInitialized;
	bool _isDeviceInitialized;
	static CO2miniData _magicTable;

	void swap(unsigned char *a, unsigned char *b)
	{
		unsigned char tmp = *a;
		*a = *b;
		*b = tmp;
	}

	void decode(const CO2miniData data, CO2miniData result)
	{
		CO2miniData buf;
		memcpy_s(buf, sizeof(CO2miniData), data, sizeof(CO2miniData));

		swap(&buf[0], &buf[2]);
		swap(&buf[1], &buf[4]);
		swap(&buf[3], &buf[7]);
		swap(&buf[5], &buf[6]);

		for (int i = 0; i < sizeof(_magicTable); i++)
		{
			buf[i] ^= _magicTable[i];
		}

		CO2miniData temp;
		for (int i = 0; i < sizeof(CO2miniData); i++)
		{
			temp[i] = ((buf[i] >> 3) | (buf[(i - 1 + 8) % 8] << 5)) & 0xff;
		}

		CO2miniData ctmp;
		const unsigned char magicWord[] = "Htemp99e";
		for (int i = 0; i < sizeof(CO2miniData); i++)
		{
			ctmp[i] = ((magicWord[i] >> 4) | (magicWord[i] << 4)) & 0xff;
		}

		for (int i = 0; i < sizeof(CO2miniData); i++)
		{
			result[i] = (0x100 + temp[i] - ctmp[i]) & 0xff;
		}
	}

public:
	Impl()
		: _isApiInitialized(hid_init() >= 0)
		, _isDeviceInitialized(false)
		, _device(nullptr)
	{}

	~Impl()
	{
		closeDevice();
		if (_isApiInitialized)
		{
			hid_exit();
		}
	}

	bool openDevice()
	{
		if (_isDeviceInitialized)
		{
			return true;
		}

		if (!_isApiInitialized)
		{
			return false;
		}

		_device = hid_open(0x04d9, 0xa052, nullptr);
		if (_device == nullptr)
		{
			return false;
		}

		unsigned char initTable[] =
		{
						0x00, _magicTable[0], _magicTable[1]
			, _magicTable[2], _magicTable[3], _magicTable[4]
			, _magicTable[5], _magicTable[6], _magicTable[7]
		};
		const int result = hid_send_feature_report(_device, initTable, sizeof(initTable));
		_isDeviceInitialized = (result == sizeof(initTable));

		return (_isDeviceInitialized);
	}

	bool closeDevice()
	{

		if (_device != nullptr)
		{
			hid_close(_device);
			_device = nullptr;
		}
		_isDeviceInitialized = false;
		return true;
	}

	bool readValue(const CO2miniValueType type, const unsigned int timeout, double &value)
	{
		if (_device == nullptr || !_isDeviceInitialized)
		{
			return false;
		}

		const DWORD startTime = GetTickCount();
		while (GetTickCount() - startTime < timeout)
		{
			CO2miniData data = { 0 };
			const int length = hid_read_timeout(_device, data, sizeof(data), timeout);
			if (length != sizeof(data))
			{
				return false;
			}

			CO2miniData result;
			decode(data, result);
			if (result[4] != 0x0d)
			{
				continue;
			}

			const unsigned char checksum = (result[0] + result[1] + result[2]) & 0xff;
			if (checksum != result[3])
			{
				continue;
			}

			const unsigned int w = (result[1] << 8) + result[2];
			switch (result[0])
			{
			case 0x42://温度
				if (type == VALUE_TYPE_TEMPERATURE)
				{
					value = static_cast<double>(w) * 0.0625 - 273.15;
					return true;
				}
				break;
			case 0x50://CO2濃度
				if (type == VALUE_TYPE_CO2)
				{
					if (static_cast<unsigned int>(w) > 3000)
					{
						break;
					}
					value = w;
					return true;
				}
				break;
			default:
				break;
			}
		};

		return false;
	}
};

CO2miniReader::Impl::CO2miniData CO2miniReader::Impl::_magicTable = { 0x90, 0xb1, 0x1e, 0x6d, 0x9f, 0xea, 0x5b, 0x59 };

/**
	コンストラクタ
*/
CO2miniReader::CO2miniReader()
	: _impl(new Impl())
{}

/**
	デストラクタ
*/
CO2miniReader::~CO2miniReader()
{
	delete _impl;
}

/**
	デバイスを開く
*/
bool CO2miniReader::openDevice()
{
	return _impl->openDevice();
}

/**
	デバイスを閉じる
*/
bool CO2miniReader::closeDevice()
{
	return _impl->closeDevice();
}

/**
	センサーから値を読み込む
	@param[in]	type	読み込む値のタイプ
	@param[in]	timeout	タイムアウト時間(ms単位)
	@param[out]	value	読み取った値が入る
	@return	タイムアウト時間内に値が読み込めた場合はtrue、それ以外はfalse
*/
bool CO2miniReader::readValue(const CO2miniValueType type, const unsigned int timeout, double &value)
{
	return _impl->readValue(type, timeout, value);
}
