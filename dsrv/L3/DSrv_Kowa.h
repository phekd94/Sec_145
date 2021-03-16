
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with Kowa lens
TODO:
 * test methods
FIXME:
DANGER:
NOTE:

Sec_145::Dsrv_Kowa class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|       (*)     |            |
+---------------+------------+
(*) - NO for the setDebug(); see DSrv_USART_QT class definition
*/

//-------------------------------------------------------------------------------------------------
#include "../L2/DSrv_USART_QT.h"  // DSrv_USART_QT class (for inheritance)

#include <QObject>  // Q_OBJECT macros
#include <cstdint>  // integer types

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class DSrv_Kowa : public DSrv_USART_QT
{
	Q_OBJECT

	// friend class DSrv_Kowa_test;

public:

	// Data type for calculate CRC method (pointer + size)
	using Data_crc = std::pair<const uint8_t *, uint8_t>;

	DSrv_Kowa();
	virtual ~DSrv_Kowa();

	// Move constructor
	DSrv_Kowa(DSrv_Kowa && obj);

	// Calculates a CRC
	uint8_t calcCRC(Data_crc data) const noexcept;

	// Focus plus
	Q_INVOKABLE int32_t focus_p() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x80;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		return write();
	}

	// Focus minus
	Q_INVOKABLE int32_t focus_m() noexcept
	{
		m_data[2] = 0x01;
		m_data[3] = 0x00;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		return write();
	}

	// Zoom plus
	Q_INVOKABLE int32_t zoom_p() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x40;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		return write();
	}

	// Zoom minus
	Q_INVOKABLE int32_t zoom_m() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x20;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		return write();
	}

	// Stop
	Q_INVOKABLE int32_t stop() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x00;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		return write();
	}

	// Enable/disable debug messages
	// (probably the method will be called from another thread)
	void setDebug(const bool d_hexapod, const bool d_usart,
	              const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_hexapod;
		DSrv_USART_QT::setDebug(d_usart, d_dsrv, d_storage);
	}

private:

	// Data type for write data (pointer + size)
	constexpr static const uint8_t m_size {7};
	uint8_t m_data[m_size] = {0xFF, 0x01};

	bool m_debug {true};

	// Parser of the accepted data (override method)
	int32_t dataParser(DSrv::Data_parser data) noexcept override final;

	// Writes data to lens
	int32_t write()
	{
		uint8_t crc = calcCRC(Data_crc(&m_data[1], m_size - 2));
		m_data[m_size - 1] = crc;

		// Send data
		if (sendData(DSrv::Data_send(m_data, m_size)) != 0)
		{
			PRINT_ERR("sendData()");
			return -1;
		}

		return 0;
	}
};
//=================================================================================================
// Class for test a DSrv_Kowa class (with test methods)
//class DSrv_Kowa_test : public QObject


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
