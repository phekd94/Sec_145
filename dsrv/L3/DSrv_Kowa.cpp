
#include "DSrv_Kowa.h"

#include <exception>  // std::exception class

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::DSrv_Kowa()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::~DSrv_Kowa()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Kowa::DSrv_Kowa(DSrv_Kowa && obj) : DSrv_USART_QT(std::move(obj))
{
	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
uint8_t DSrv_Kowa::calcCRC(Data_crc data) const noexcept
{
	uint16_t crc = 0;

	while (data.second--)
	{
		crc += *data.first++;
	}

	return crc % 256;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Kowa::dataParser(DSrv::Data_parser data) noexcept
{
	// Identificator for add data to last record in storage class
	bool add {true};

	// Lambda function for update paremeters
	auto updateParams = [&data](){ ++data.first; --data.second; };

	// Stop the watchdog timer
	// m_watchdog.stop();

	// Check the incomming parameters
	if (nullptr == data.first)
	{
		PRINT_ERR("nullptr == data.first");
		return -2;
	}

	while (data.second > 0)
	{
		if (0 == m_pktRemSize)
		{
			// === First byte in message ===

			// Set a remaining size
			m_pktRemSize = m_numOfBytes_req;

			PRINT_DBG(m_debug, "[S]: 0x%x", *data.first);

			if (*data.first != 0xFF)
			{
				PRINT_ERR("Error synchronization");
				m_pktRemSize = 0;
				updateParams();
				continue;
			}
		}
		else if (   (3 == m_pktRemSize && false == m_dataNeed)
		         || (6 == m_pktRemSize && true == m_dataNeed))
		{
			// === ID ===

			PRINT_DBG(m_debug, "[I]: 0x%x", *data.first);

			if (*data.first != 0x01)
			{
				PRINT_ERR("Error ID");
				m_pktRemSize = 0;
				updateParams();
				continue;
			}
		}
		else if (1 == m_pktRemSize)
		{
			// === CRC ===

			PRINT_DBG(m_debug, "[R]: 0x%x", *data.first);
		}
		else if (false == m_dataNeed)
		{
			PRINT_DBG(m_debug, "[D]: 0x%x", *data.first);
		}
		else if (true == m_dataNeed)
		{
			if (2 == m_pktRemSize || 3 == m_pktRemSize)
			{
				PRINT_DBG(m_debug, "[D]: 0x%x", *data.first);

				// Set data for read type message
				if (DSrv_Storage::setData(DSrv_Storage::Data_set(data.first, 1), add) != 0)
				{
					PRINT_ERR("DSrv_Storage::setData()");
					m_pktRemSize = 0;
					return -1;
				}
			}
			else
			{
				PRINT_DBG(m_debug, "[C]: 0x%x", *data.first);

				if (4 == m_pktRemSize)
				{
					if (0x8D == *data.first)
					{
						m_cmd = eCmd::FOCUS;
					}
					else if (0x5D == *data.first)
					{
						m_cmd = eCmd::ZOOM;
					}
					else
					{
						PRINT_ERR("Unknown command");
						m_pktRemSize = 0;
						updateParams();
						continue;
					}
				}
			}
		}

		// Update a m_pktRemSize
		--m_pktRemSize;
		if (0 == m_pktRemSize && true == m_dataNeed)
		{
			// Data is complete
			if (DSrv_Storage::completeData() != 0)
			{
				PRINT_ERR("DSrv_Storage::completeData() != 0");
				m_pktRemSize = 0;
				return -1;
			}

			// Get complete data
			uint8_t* data_res;
			uint32_t size_res;
			if (DSrv_Storage::getData(DSrv_Storage::Data_get(&data_res, &size_res)) != 0)
			{
				PRINT_ERR("DSrv_Storage::getData() != 0");
				m_pktRemSize = 0;
				return -1;
			}

			if (size_res != 2)
			{
				PRINT_ERR("Result size != 2");
				m_pktRemSize = 0;
				return -1;
			}

			std::swap(data_res[0], data_res[1]);
			if (eCmd::FOCUS == m_cmd)
			{
				emit toFocusVal(*reinterpret_cast<uint16_t *>(data_res));
			}
			else if (eCmd::ZOOM == m_cmd)
			{
				emit toZoomVal(*reinterpret_cast<uint16_t *>(data_res));
			}
			else
			{
				PRINT_ERR("Unknown command");
				updateParams();
				continue;
			}
			m_cmd = eCmd::NONE;
		}

		// Update a data pointer and a size
		updateParams();
	}

	return 0;
}
