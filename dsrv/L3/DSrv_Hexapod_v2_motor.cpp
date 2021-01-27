
#include "DSrv_Hexapod_v2_motor.h"

#include <exception>  // std::exception class
#include <cstring>    // std::memcpy
#include <memory>     // std::unique_ptr

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Hexapod_v2_motor::DSrv_Hexapod_v2_motor(uint8_t motor_id) : m_motor_id(motor_id)
{
	// Set timer in single shot mode
	m_watchdog.setSingleShot(true);

	// Connect slot to timer
	connect(&m_watchdog, SIGNAL(timeout()), this, SLOT(onWatchdog()));

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

	m_address_req = obj.m_address_req;
	m_val_req = obj.m_val_req;
	m_numOfBytes_req = obj.m_numOfBytes_req;

	m_motor_id_pars = obj.m_motor_id_pars;
	m_funcCode_pars = obj.m_funcCode_pars;
	m_crc_l_pars = obj.m_crc_l_pars;
	m_crc_h_pars = obj.m_crc_h_pars;
	m_count_pars = obj.m_count_pars;
	m_address_l_pars = obj.m_address_l_pars;
	m_address_h_pars = obj.m_address_h_pars;
	m_val_l_pars = obj.m_val_l_pars;
	m_val_h_pars = obj.m_val_h_pars;

	// Stop old watchdog timer
	obj.m_watchdog.stop();

	// Set new timer in single shot mode
	m_watchdog.setSingleShot(true);

	// Connect slot to the new timer
	connect(&m_watchdog, SIGNAL(timeout()), this, SLOT(onWatchdog()));

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
	// Data for request
	uint8_t data[] {
		m_motor_id,
		read, // function code
		static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
		0x00, count, // number of 16 bit registers
		0x00, 0x00  // for CRC
	};

	// Calculate a CRC (size - 2 due to CRC field)
	uint16_t crc = calcCRC(data, sizeof(data) - 2);
	data[sizeof(data) - 2] = crc & 0xFF;
	data[sizeof(data) - 1] = crc >> 8;

	// Set the request parameters
	m_funcCode_req = read;
	m_address_req = address;

	m_numOfBytes_req = 2 * count + 5;
	// 5 bits = id(1 bit) + function code(1 bit) + byte count(1 bit) + CRC(2 bits)

	// Send data
	if (sendData(data, sizeof(data), "", 0) != 0)
	{
		PRINT_ERR(true, "sendData()");
		return -1;
	}
	else
	{
		// Run a watchdog timer
		m_watchdog.start(500);
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::writeSingleReg(const uint16_t address, const uint16_t val) noexcept
{
	// Data for request
	uint8_t data[] {
		m_motor_id,
		write, // function code
		static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
		static_cast<uint8_t>(val >> 8), static_cast<uint8_t>(val & 0xFF),
		0x00, 0x00  // for CRC
	};

	// Calculate a CRC (size - 2 due to CRC field)
	uint16_t crc = calcCRC(data, sizeof(data) - 2);
	data[sizeof(data) - 2] = crc & 0xFF;
	data[sizeof(data) - 1] = crc >> 8;

	// Set the request parameters
	m_funcCode_req = write;
	m_address_req = address;
	m_val_req = val;

	m_numOfBytes_req = 8;
	// 8 bits = id(1 bit) + function code(1 bit) + address(2 bits) + value(2 bits) + CRC(2 bits)

	// Send data
	if (sendData(data, sizeof(data), "", 0) != 0)
	{
		PRINT_ERR(true, "sendData()");
		return -1;
	}
	else
	{
		// Run a watchdog timer
		m_watchdog.start(500);
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::home() noexcept
{
	// Set a HOME mode
	m_mode = MODE::HOME;

	// Read a Home_Position1 register
	return readHoldingRegs(std::get<0>(Home_Position1), std::get<1>(Home_Position1));
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::dataParser(uint8_t* data, uint32_t size) noexcept
{
	// Identificator for add data to last record in storage class
	bool add {true};

	// Lambda function for update paremeters
	auto updateParams = [&data, &size](){++data; --size;};

	// Stop the watchdog timer
	m_watchdog.stop();

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
			// === First byte in message ===

			// Reset all pairs
			m_motor_id_pars.first = false;
			m_funcCode_pars.first = false;
			m_count_pars.first = false;
			m_address_l_pars.first = m_address_h_pars.first = false;
			m_val_l_pars.first = m_val_h_pars.first = false;
			m_crc_l_pars.first = m_crc_h_pars.first = false;

			// Set a remaining size
			m_pktRemSize = m_numOfBytes_req;

			// Get a motor id
			m_motor_id_pars.first = true;
			m_motor_id_pars.second = *data;

			PRINT_DBG(m_debug, "[0]: 0x%x", *data);

			// Check a motor id
			if (m_motor_id_pars.second != m_motor_id)
			{
				PRINT_ERR(true, "Motor id is not correct");
				m_pktRemSize = 0;
				updateParams();
				continue;
			}
		}
		else
		{
			if (false == m_funcCode_pars.first)
			{
				// === Second byte in message ===

				// Get a function code
				m_funcCode_pars.first = true;
				m_funcCode_pars.second = *data;

				PRINT_DBG(m_debug, "[1]: 0x%x", *data);

				// Check a function code
				if (m_funcCode_pars.second != m_funcCode_req)
				{
					PRINT_ERR(true, "Function code is not correct");
					m_pktRemSize = 0;
					updateParams();
					continue;
				}
			}
			else if (read == m_funcCode_pars.second && false == m_count_pars.first)
			{
				// Get a count of bytes
				m_count_pars.first = true;
				m_count_pars.second = *data;

				PRINT_DBG(m_debug, "[2]: 0x%x", *data);

				// Check the count of bytes
				if (m_count_pars.second > 20)
				{
					PRINT_ERR(true, "Count of bytes more than 20");
					m_pktRemSize = 0;
					updateParams();
					continue;
				}
			}
			else if (write == m_funcCode_pars.second && false == m_address_h_pars.first)
			{
				// Get an high address
				m_address_h_pars.first = true;
				m_address_h_pars.second = *data;

				PRINT_DBG(m_debug, "[3]: 0x%x", *data);
			}
			else if (write == m_funcCode_pars.second && false == m_address_l_pars.first)
			{
				// Get an low address
				m_address_l_pars.first = true;
				m_address_l_pars.second = *data;

				PRINT_DBG(m_debug, "[4]: 0x%x", *data);

				// Check the address
				if (   (m_address_req & 0xFF) != m_address_l_pars.second
				    || (m_address_req >> 8) != m_address_h_pars.second)
				{
					PRINT_ERR(true, "Address is not correct");
					m_pktRemSize = 0;
					updateParams();
					continue;
				}
			}
			else if (write == m_funcCode_pars.second && false == m_val_h_pars.first)
			{
				m_val_h_pars.first = true;
				m_val_h_pars.second = *data;

				PRINT_DBG(m_debug, "[5]: 0x%x", *data);
			}
			else if (write == m_funcCode_pars.second && false == m_val_l_pars.first)
			{
				m_val_l_pars.first = true;
				m_val_l_pars.second = *data;

				PRINT_DBG(m_debug, "[6]: 0x%x", *data);

				// Check the value
				if (   (m_val_req & 0xFF) != m_val_l_pars.second
				    || (m_val_req >> 8) != m_val_h_pars.second)
				{
					PRINT_ERR(true, "Register value is not correct");
					m_pktRemSize = 0;
					updateParams();
					continue;
				}
			}
			else if (write == m_funcCode_pars.second && m_pktRemSize > 2)
			{
				PRINT_ERR(true, "Write response is not correct");
				m_pktRemSize = 0;
				updateParams();
				continue;
			}
			else if (read == m_funcCode_pars.second && m_pktRemSize > 2)
			{
				PRINT_DBG(m_debug, "[+]: 0x%x", *data);

				// Set data for read type message
				if (DSrv_Storage::setData(data, 1, add) != 0)
				{
					PRINT_ERR(true, "DSrv_Storage::setData()");
					m_pktRemSize = 0;
					return -1;
				}
			}
			else if (false == m_crc_l_pars.first)
			{
				m_crc_l_pars.first = true;
				m_crc_l_pars.second = *data;

				PRINT_DBG(m_debug, "[*]: 0x%x", *data);
			}
			else if (false == m_crc_h_pars.first)
			{
				m_crc_h_pars.first = true;
				m_crc_h_pars.second = *data;

				PRINT_DBG(m_debug, "[*]: 0x%x", *data);
			}
			else
			{
				PRINT_ERR(true, "Message format error; [-]: 0x%x", *data);
				m_pktRemSize = 0;
				updateParams();
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

			// Parse data
			switch (m_funcCode_pars.second)
			{
			    case write:
			    {
				    // Check the CRC
				    uint16_t crc;
					std::unique_ptr<uint8_t[]> data_for_crc(new (std::nothrow) uint8_t[6]);
					  // 6 bits = id(1 bit) + func code(1 bit) + address(2 bits) + val(2 bits)
					if (nullptr == data_for_crc)
					{
						PRINT_ERR(true, "Can not allocate a memory for check a CRC");
						return -1;
					}
					data_for_crc[0] = m_motor_id_pars.second;
					data_for_crc[1] = m_funcCode_pars.second;
					data_for_crc[2] = m_address_h_pars.second;
					data_for_crc[3] = m_address_l_pars.second;
					data_for_crc[4] = m_val_h_pars.second;
					data_for_crc[5] = m_val_l_pars.second;
					crc = calcCRC(data_for_crc.get(), 6);
					if ((crc & 0xFF) != m_crc_l_pars.second || (crc >> 8) != m_crc_h_pars.second)
					{
						PRINT_ERR(true, "CRC is not correct");
						updateParams();
						continue;
					}

					// Call a move control function
					if (moveControl(write, data_res, size_res) != 0)
					{
						m_mode = MODE::NONE;
						updateParams();
						continue;
					}

				    break;
			    }
			    case read:
			    {
				    // Check the data size of result
				    if (size_res != m_count_pars.second)
					{
						PRINT_ERR(true, "Data size is not match");
						updateParams();
						continue;
					}

					// Check the CRC
					uint16_t crc;
					std::unique_ptr<uint8_t[]> data_for_crc(
					            new (std::nothrow) uint8_t[size_res + 3]);
					  // 3 bits = id(1 bit) + func code(1 bit) + count(1 bit)
					if (nullptr == data_for_crc)
					{
						PRINT_ERR(true, "Can not allocate a memory for check a CRC");
						return -1;
					}
					data_for_crc[0] = m_motor_id_pars.second;
					data_for_crc[1] = m_funcCode_pars.second;
					data_for_crc[2] = m_count_pars.second;
					memcpy(data_for_crc.get() + 3, data_res, size_res);
					crc = calcCRC(data_for_crc.get(), size_res + 3);
					if ((crc & 0xFF) != m_crc_l_pars.second || (crc >> 8) != m_crc_h_pars.second)
					{
						PRINT_ERR(true, "CRC is not correct");
						updateParams();
						continue;
					}

					// Swap the high and low bits in 16 bits data
					for (uint32_t i = 0; i < size_res; i += 2)
					{
						std::swap(data_res[i], data_res[i + 1]);
					}

					// Call a move control function
					if (moveControl(read, data_res, size_res) != 0)
					{
						m_mode = MODE::NONE;
						updateParams();
						continue;
					}

				    break;
			    }
			    default:
				    PRINT_ERR(true, "Unknown function code (%u)",
					          static_cast<unsigned int>(m_funcCode_pars.second));
					updateParams();
				    continue;
			}
		}

		// Update a data pointer and a size
		updateParams();
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Hexapod_v2_motor::moveControl(const uint8_t mode,
                                           const uint8_t * const data,
                                           const uint32_t) noexcept
{
	switch (mode)
	{
	    case write:
	    {
		    if (MODE::HOME == m_mode)
			{
				if (std::get<0>(IEG_MOTION) == m_address_req && (1u << 8) == m_val_req)
				{
					PRINT_DBG(m_debug, "Write(IEG_MOTION, HOME) is got");

					if (writeSingleReg(std::get<0>(IEG_MOTION), 0) != 0)
					{
						PRINT_ERR(true, "writeSingleReg(IEG_MOTION, 0)");
						return -1;
					}
				}
				else if (std::get<0>(IEG_MOTION) == m_address_req && 0 == m_val_req)
				{
					PRINT_DBG(m_debug, "Write(IEG_MOTION, 0) is got");

					if (readHoldingRegs(std::get<0>(OEG_MOTION), std::get<1>(OEG_MOTION)) != 0)
					{
						PRINT_ERR(true, "readHoldingRegs(OEG_MOTION)");
						return -1;
					}
				}
				else
				{
					PRINT_ERR(true, "Unknown write answer for MODE::HOME mode");
					return -1;
				}
			}
			else if (MODE::NONE == m_mode)
			{
				PRINT_DBG(m_debug, "Write is complete");
			}
			else
			{
				PRINT_ERR(true, "Unknown write MODE:: mode");
				return -1;
			}

			break;
	    }
	    case read:
	    {
		    if (MODE::HOME == m_mode)
			{
				if (std::get<0>(Home_Position1) == m_address_req)
				{
					std::get<2>(Home_Position1) = *reinterpret_cast<const int32_t*>(data);
					PRINT_DBG(m_debug, "Read(Home_Position1) is got (= %ld)",
					          static_cast<long>(std::get<2>(Home_Position1)));

					if (writeSingleReg(std::get<0>(IEG_MOTION), 1u << 8) != 0)
					{
						PRINT_ERR(true, "writeSingleReg(IEG_MOTION, HOME)");
						return -1;
					}
				}
				else if (std::get<0>(OEG_MOTION) == m_address_req)
				{
					std::get<2>(OEG_MOTION) = *data;
					PRINT_DBG(m_debug, "Read(OEG_MOTION) is got (= 0x%x)",
					          static_cast<unsigned int>(std::get<2>(OEG_MOTION)));

					// Check the HMV (Homing Active) flag
					if (!(std::get<2>(OEG_MOTION) & (1u << 5)))
					{
						m_mode = MODE::NONE;
						PRINT_DBG(m_debug, "HOME is completed");
						emit toHomeComplete(m_motor_id);
					}
					else
					{
						if (readHoldingRegs(std::get<0>(OEG_MOTION), std::get<1>(OEG_MOTION)) != 0)
						{
							PRINT_ERR(true, "readHoldingRegs(OEG_MOTION)");
							return -1;
						}
					}
				}
				else
				{
					PRINT_ERR(true, "Unknown read answer for MODE::HOME mode");
					return -1;
				}
			}
			else if (MODE::NONE == m_mode)
			{
				PRINT_DBG(m_debug, "Read is complete");
			}
			else
			{
				PRINT_ERR(true, "Unknown read MODE:: mode");
				return -1;
			}

			break;
	    }
	    default:
		    PRINT_ERR(true, "Unknown mode(%u) function parameter",
			          static_cast<unsigned int>(mode));
		    return -1;
		    break;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
void DSrv_Hexapod_v2_motor::onWatchdog() noexcept
{
	PRINT_ERR(true, "Watchdog timer is triggered");

	emit toWatchdog(m_motor_id);
}
