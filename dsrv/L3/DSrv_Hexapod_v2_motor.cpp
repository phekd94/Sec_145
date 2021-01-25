
#include "DSrv_Hexapod_v2_motor.h"

#include <exception>  // std::exception class
#include <utility>    // std::pair
#include <cstring>    // std::memcpy

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2_motor::DSrv_Hexapod_v2_motor(uint8_t motor_id) : m_motor_id(motor_id)
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2_motor::~DSrv_Hexapod_v2_motor()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2_motor::DSrv_Hexapod_v2_motor(DSrv_Hexapod_v2_motor && obj) :
    DSrv_USART_QT(std::move(obj))
{
	// Copy all fields
	m_motor_id = obj.m_motor_id;
	m_addressReq = obj.m_addressReq;
	m_val = obj.m_val;
	m_numOfBytesReq = obj.m_numOfBytesReq;

	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
uint16_t DSrv_Hexapod_v2_motor::calcCRC(const uint8_t *data, uint8_t length) const noexcept
{
	uint8_t j;

	uint16_t crc = 0xFFFF;

	while (length--)
	{
		crc ^= *data++;
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x01)
			{
				crc = (crc >> 1) ^ 0xA001;
			}
			else
			{
				crc = crc >> 1;
			}
		}
	}

	return crc;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::readHoldingRegs(const uint16_t address,
                                               const uint8_t count) noexcept
{
	const uint8_t funcCode {0x03};

	// Data for request
	uint8_t data[] {
		m_motor_id,
		funcCode, // function code
		static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
		0x00, count, // number of 16 bit registers
		0x00, 0x00  // for CRC
	};

	// Calculate a CRC (size - 2 due to CRC field)
	uint16_t crc = calcCRC(data, sizeof(data) - 2);
	data[sizeof(data) - 2] = crc & 0xFF;
	data[sizeof(data) - 1] = crc >> 8;

	// Set the request parameters
	m_funcCode = funcCode;

	m_numOfBytesReq = 2 * count + 5;
	// 5 bits = id(1 bit) + function code(1 bit) + byte count(1 bit) + CRC(2 bits)

	return sendData(data, sizeof(data), "", 0);
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::writeSingleReg(const uint16_t address, const uint16_t val) noexcept
{
	const uint8_t funcCode {0x06};

	// Data for request
	uint8_t data[] {
		m_motor_id,
		funcCode, // function code
		static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
		static_cast<uint8_t>(val >> 8), static_cast<uint8_t>(val & 0xFF),
		0x00, 0x00  // for CRC
	};

	// Calculate a CRC (size - 2 due to CRC field)
	uint16_t crc = calcCRC(data, sizeof(data) - 2);
	data[sizeof(data) - 2] = crc & 0xFF;
	data[sizeof(data) - 1] = crc >> 8;

	// Set the request parameters
	m_funcCode = funcCode;
	m_addressReq = address;
	m_val = val;

	m_numOfBytesReq = 8;
	// 8 bits = id(1 bit) + function code(1 bit) + address(2 bits) + value(2 bits) + CRC(2 bits)

	return sendData(data, sizeof(data), "", 0);
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::dataParser(uint8_t* data, uint32_t size) noexcept
{
	// Identificator for add data to last record in storage class
	bool add {true};

	// Lambda function for update paremeters
	auto updataParams = [&data, &size](){++data; --size;};

	// Message type
	const uint8_t read {0x03};
	const uint8_t write {0x06};

	// For all type message
	static std::pair<bool, uint8_t> id {false, 0};
	static std::pair<bool, uint8_t> funcCode {false, 0};
	static std::pair<bool, uint8_t> crc_l {false, 0};
	static std::pair<bool, uint8_t> crc_h {false, 0};

	// For read
	static std::pair<bool, uint8_t> count {false, 0};

	// For write
	static std::pair<bool, uint8_t> address_l {false, 0};
	static std::pair<bool, uint8_t> address_h {false, 0};
	static std::pair<bool, uint8_t> regVal_l {false, 0};
	static std::pair<bool, uint8_t> regVal_h {false, 0};

	// Check the incomming parameters
	if (nullptr == data)
	{
		PRINT_ERR(m_debug, "nullptr == data");
		return -2;
	}

	while (size > 0)
	{
		if (0 == m_pktRemSize)
		{
			// First byte in message

			// Reset all pairs
			id.first = false;
			funcCode.first = false;
			count.first = false;
			address_l.first = address_h.first = false;
			regVal_l.first = regVal_h.first = false;
			crc_l.first = crc_h.first = false;

			// Set a remaining size
			m_pktRemSize = m_numOfBytesReq;

			// Get a motor id
			id.first = true;
			id.second = *data;

			// Check a motor id
			if (id.second != m_motor_id)
			{
				PRINT_ERR(true, "Motor id is not correct");
				m_pktRemSize = 0;
				updataParams();
				continue;
			}

			PRINT_DBG(m_debug, "[0]: 0x%x", *data);
		}
		else
		{
			if (false == funcCode.first)
			{
				// Second byte in message

				// Check a function code
				if (*data != m_funcCode)
				{
					PRINT_ERR(true, "Function code is not correct");
					m_pktRemSize = 0;
					updataParams();
					continue;
				}

				// Get a function code
				funcCode.first = true;
				funcCode.second = *data;

				PRINT_DBG(m_debug, "[1]: 0x%x", *data);
			}
			else if (read == funcCode.second && false == count.first)
			{
				// Check the count of bytes
				if (*data > 20)
				{
					PRINT_ERR(true, "Count of bytes more than 20");
					m_pktRemSize = 0;
					updataParams();
					continue;
				}

				// Get a count of bytes
				count.first = true;
				count.second = *data;

				PRINT_DBG(m_debug, "[2]: 0x%x", *data);
			}
			else if (write == funcCode.second && false == address_h.first)
			{
				// Get an high address
				address_h.first = true;
				address_h.second = *data;

				PRINT_DBG(m_debug, "[3]: 0x%x", *data);
			}
			else if (write == funcCode.second && false == address_l.first)
			{
				// Check the address
				if ((m_addressReq & 0xFF) != *data || (m_addressReq >> 8) != address_h.second)
				{
					PRINT_ERR(true, "Address is not correct");
					m_pktRemSize = 0;
					updataParams();
					continue;
				}

				// Get an low address
				address_l.first = true;
				address_l.second = *data;

				PRINT_DBG(m_debug, "[4]: 0x%x", *data);
			}
			else if (write == funcCode.second && false == regVal_h.first)
			{
				regVal_h.first = true;
				regVal_h.second = *data;

				PRINT_DBG(m_debug, "[5]: 0x%x", *data);
			}
			else if (write == funcCode.second && false == regVal_l.first)
			{
				// Check the value
				if ((m_val & 0xFF) != *data || (m_val >> 8) != regVal_h.second)
				{
					PRINT_ERR(true, "Register value is not correct");
					m_pktRemSize = 0;
					updataParams();
					continue;
				}

				regVal_l.first = true;
				regVal_l.second = *data;

				PRINT_DBG(m_debug, "[6]: 0x%x", *data);
			}
			else if (write == funcCode.second && m_pktRemSize > 2)
			{
				PRINT_ERR(true, "Write response is not correct");
				m_pktRemSize = 0;
				updataParams();
				continue;
			}
			else if (read == funcCode.second && m_pktRemSize > 2)
			{
				// Set data for read type message
				if (DSrv_Storage::setData(data, 1, add) != 0)
				{
					PRINT_ERR(true, "DSrv_Storage::setData()");
					return -1;
				}

				PRINT_DBG(m_debug, "[+]: 0x%x", *data);
			}
			else if (false == crc_l.first)
			{
				crc_l.first = true;
				crc_l.second = *data;

				PRINT_DBG(m_debug, "[*]: 0x%x", *data);
			}
			else if (false == crc_h.first)
			{
				crc_h.first = true;
				crc_h.second = *data;

				PRINT_DBG(m_debug, "[*]: 0x%x", *data);
			}
			else
			{
				PRINT_ERR(true, "message format error; data = 0x%x", *data);
				m_pktRemSize = 0;
				updataParams();
				continue;
			}
		}


		// Update a m_pktRemSize
		--m_pktRemSize;
		if (0 == m_pktRemSize)
		{
			// Data is complete
			if (DSrv_Storage::completeData() != 0)
			{
				PRINT_ERR(true, "DSrv_Storage::completeData() != 0");
				return -1;
			}

			// Get complete data
			uint8_t* data_res;
			uint32_t size_res;
			if (DSrv_Storage::getData(&data_res, &size_res) != 0)
			{
				PRINT_ERR(true, "DSrv_Storage::getData() != 0");
				return -1;
			}

			// For CRC check
			uint8_t *data_for_crc {nullptr};
			uint16_t crc;

			// Parse data
			switch (funcCode.second)
			{
			    case write:
				    // Check the CRC
				    data_for_crc = new (std::nothrow) uint8_t[6];
					  // 6 bits = id(1 bit) + func code(1 bit) + address(2 bits) + val(2 bits)
					data_for_crc[0] = id.second;
					data_for_crc[1] = funcCode.second;
					data_for_crc[2] = address_h.second;
					data_for_crc[3] = address_l.second;
					data_for_crc[4] = regVal_h.second;
					data_for_crc[5] = regVal_l.second;
					crc = calcCRC(data_for_crc, 6);
					if ((crc & 0xFF) != crc_l.second || (crc >> 8) != crc_h.second)
					{
						PRINT_ERR(true, "CRC is not correct");
						return -1;
					}

					PRINT_DBG(m_debug, "Write is complete");
				    break;

			    case read:
				    // Check the data size of result
				    if (size_res != count.second)
					{
						PRINT_ERR(true, "Data size is not match");
						return -1;
					}

					// Check the CRC
					data_for_crc = new (std::nothrow) uint8_t[size_res + 3];
					  // 3 bits = id(1 bit) + func code(1 bit) + count(1 bit)
					data_for_crc[0] = id.second;
					data_for_crc[1] = funcCode.second;
					data_for_crc[2] = count.second;
					memcpy(data_for_crc + 3, data_res, size_res);
					crc = calcCRC(data_for_crc, size_res + 3);
					if ((crc & 0xFF) != crc_l.second || (crc >> 8) != crc_h.second)
					{
						PRINT_ERR(true, "CRC is not correct");
						return -1;
					}

					// Swap the high and low bits in 16 bits data
					for (uint32_t i = 0; i < size_res; i += 2)
					{
						std::swap(data_res[i], data_res[i + 1]);
					}

					PRINT_DBG(m_debug, "Read is complete");

					qDebug() << "res" << *reinterpret_cast<int32_t*>(data_res)
					         << *reinterpret_cast<int32_t*>(data_res) * 0.0000152587890625;
				    break;
			}

			delete [] data_for_crc;
		}

		// Update a data pointer and a size
		updataParams();
	}

	return 0;
}
