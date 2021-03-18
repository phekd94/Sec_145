
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

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Focus minus
	Q_INVOKABLE int32_t focus_m() noexcept
	{
		m_data[2] = 0x01;
		m_data[3] = 0x00;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Zoom plus
	Q_INVOKABLE int32_t zoom_p() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x40;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Zoom minus
	Q_INVOKABLE int32_t zoom_m() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x20;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Stops
	Q_INVOKABLE int32_t stop() noexcept
	{
		m_data[2] = 0x00;
		m_data[3] = 0x00;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Moves to focus position
	Q_INVOKABLE int32_t focusPos(uint16_t val)
	{
		m_data[2] = 0x00;
		m_data[3] = 0x87;
		m_data[4] = val >> 8;
		m_data[5] = val & 0xFF;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Moves to zoom position
	Q_INVOKABLE int32_t zoomPos(uint16_t val)
	{
		m_data[2] = 0x00;
		m_data[3] = 0x4F;
		m_data[4] = val >> 8;
		m_data[5] = val & 0xFF;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Gets a focus position
	Q_INVOKABLE int32_t focusGet()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x8B;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 7;
		m_dataNeed = true;

		return write();
	}

	// Gets a zoom position
	Q_INVOKABLE int32_t zoomGet()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x55;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 7;
		m_dataNeed = true;

		return write();
	}

	// Sets slow speed for focus
	Q_INVOKABLE int32_t focusSS()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x27;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Sets slow speed for zoom
	Q_INVOKABLE int32_t zoomSS()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x25;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// AF
	Q_INVOKABLE int32_t AF()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x2B;
		m_data[4] = 0x00;
		m_data[5] = 0x00;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Sets slow speed for focus continuously
	Q_INVOKABLE int32_t focusSS01()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x83;
		m_data[4] = 0x00;
		m_data[5] = 15u;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

		return write();
	}

	// Sets slow speed for zoom continuously
	Q_INVOKABLE int32_t zoomSS01()
	{
		m_data[2] = 0x00;
		m_data[3] = 0x81;
		m_data[4] = 0x00;
		m_data[5] = 15u;

		m_numOfBytes_req = 4;
		m_dataNeed = false;

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

	// Number of bytes for request and need data flag
	uint8_t m_numOfBytes_req {0};
	bool m_dataNeed {false};

	// Command
	enum class eCmd {
		NONE = 0, FOCUS, ZOOM
	} m_cmd {eCmd::NONE};

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

signals:

	void toFocusVal(uint16_t val);
	void toZoomVal(uint16_t val);
};
//=================================================================================================
// Class for test a DSrv_Kowa class (with test methods)
//class DSrv_Kowa_test : public QObject


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
