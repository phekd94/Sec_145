
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with hexapod
TODO:
FIXME:
DANGER:
 *	for (uint32_t i = m_data_size[m_cmd_id]; i < sizeof(uint32_t); ++i)
	{
		data_res[i] = 0; // => minimal size for storage data is sizeof(uint32_t)
	}
NOTE:

Sec_145::Dsrv_Hexapod_v2 class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |    YES(*)  |
+---------------+------------+
(*) - see DSrv_USART_QT class definition
*/

//-------------------------------------------------------------------------------------------------
#include "../L2/DSrv_USART_QT.h"  // DSrv_USART_QT class (for inheritance)

#include <QObject>  // Q_OBJECT macros
#include <cstdint>  // integer types
#include <vector>   // std::vector

//-------------------------------------------------------------------------------------------------
// Constants
[[maybe_unused]] const uint32_t HEXAPOD_POS       = 0;
[[maybe_unused]] const uint32_t HEXAPOD_SPEED     = 1;
[[maybe_unused]] const uint32_t HEXAPOD_ERR       = 2;
[[maybe_unused]] const uint32_t HEXAPOD_START     = 3;
[[maybe_unused]] const uint32_t HEXAPOD_STOP      = 4;
[[maybe_unused]] const uint32_t HEXAPOD_FREQUENCY = 5;
[[maybe_unused]] const uint32_t HEXAPOD_SPEED_TO  = 6;
[[maybe_unused]] const uint32_t HEXAPOD_START_POS = 7;
[[maybe_unused]] const uint32_t HEXAPOD_MODE      = 8;
[[maybe_unused]] const uint32_t HEXAPOD_SIZE      = 9;
[[maybe_unused]] const uint32_t HEXAPOD_VALUE     = 10;
[[maybe_unused]] const uint32_t HEXAPOD_REINIT    = 11;
[[maybe_unused]] const uint32_t HEXAPOD_ANSWER    = 12;

[[maybe_unused]] const uint32_t HEXAPOD_NUM_OF_CMD = 13;

[[maybe_unused]] const uint32_t HEXAPOD_ERR_USART_OPEN  = 100;
[[maybe_unused]] const uint32_t HEXAPOD_ERR_USART_IO    = 101;
[[maybe_unused]] const uint32_t HEXAPOD_DEV_OPENED      = 102;
[[maybe_unused]] const uint32_t HEXAPOD_DEV_CLOSED      = 103;
[[maybe_unused]] const uint32_t HEXAPOD_ERR_USART_CLOSE = 104;

[[maybe_unused]] const uint32_t HEXAPOD_CMD_CON    = 200;
[[maybe_unused]] const uint32_t HEXAPOD_CMD_DISCON = 201;
[[maybe_unused]] const uint32_t HEXAPOD_CMD_SEND   = 202;

[[maybe_unused]] const uint32_t HEXAPOD_NUM_OF_MOTORS = 6;

[[maybe_unused]] const uint32_t PKT_HEADER_SIZE = 2;

[[maybe_unused]] const uint32_t PKT_HEADER_MOTOR_INDEX = 0;
[[maybe_unused]] const uint32_t PKT_HEADER_CMD_INDEX   = 1;

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class DSrv_Hexapod_v2 : public DSrv_USART_QT
{
	Q_OBJECT

	friend class DSrv_Hexapod_v2_test;

public:

	DSrv_Hexapod_v2();
	virtual ~DSrv_Hexapod_v2();
	
	// Flag of complete
	volatile bool m_stop_ok {false};

	// Gets a vector with data sizes
	const std::vector<uint32_t>& getDataSize() const noexcept
	{
		return m_data_size;
	}

private:

	// Packet parameters
	int32_t m_motor_id {-1};
	uint32_t m_cmd_id {0};
	std::vector<uint32_t> m_data_size;

	// Calls start() method from DSrv_USART_QT class and connects signals
	int32_t start() noexcept;

	// Calls stop() method from DSrv_USART_QT class
	int32_t stop() noexcept;

	// Parser of the accepted data (override method)
	int32_t dataParser(uint8_t* data, uint32_t size) noexcept override final;

public slots: // They should not generate exeptions

	// Handles error occured signal
	void onErrorOccured(QSerialPort::SerialPortError err) noexcept;

	// Handles a command request
	void onSendCommand(const uint32_t cmd, const uint32_t* data, const uint32_t size) noexcept;

signals:

	// Signal emits when state has changed
	void stateChanged(const uint32_t motor, const uint32_t state, const uint32_t value);
};

//=================================================================================================
// Class for test a DSrv_Hexapod_v2 class (with test methods)
class DSrv_Hexapod_v2_test : public QObject
{
	Q_OBJECT

public:

	// Tests a logic of functions
	static int32_t logic(DSrv_Hexapod_v2& obj, DSrv_Hexapod_v2_test& obj_test) noexcept;

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_Hexapod_v2& obj) noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;

private:

	// Only via public static methods
	DSrv_Hexapod_v2_test()
	{
	}

	// Flag when values in slot are successful
	volatile bool m_slot_ok {false};

public slots:

	// Handles state changed signal from DSrv_Hexapod_v2 class
	void onStateChanged(const uint32_t motor,
	                    const uint32_t state,
	                    const uint32_t value) noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
