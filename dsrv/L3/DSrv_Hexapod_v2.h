
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with hexapod
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::Dsrv_Hexapod_v2 class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |    YES     |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "../L2/DSrv_USART_QT.h"  // DSrv_USART_QT class (for inheritance)

#include <QObject>                // Q_OBJECT macros
#include <cstdint>                // integer types
#include <vector>                 // std::vector

//-------------------------------------------------------------------------------------------------
// Constants
const uint32_t HEXAPOD_POS       = 0;
const uint32_t HEXAPOD_SPEED     = 1;
const uint32_t HEXAPOD_ERR       = 2;
const uint32_t HEXAPOD_START     = 3;
const uint32_t HEXAPOD_STOP      = 4;
const uint32_t HEXAPOD_FREQUENCY = 5;
const uint32_t HEXAPOD_SPEED_TO  = 6;
const uint32_t HEXAPOD_START_POS = 7;
const uint32_t HEXAPOD_MODE      = 8;
const uint32_t HEXAPOD_SIZE      = 9;
const uint32_t HEXAPOD_VALUE     = 10;
const uint32_t HEXAPOD_REINIT    = 11;
const uint32_t HEXAPOD_ANSWER    = 12;

const uint32_t HEXAPOD_NUM_OF_CMD = 13;

const uint32_t HEXAPOD_ERR_USART_OPEN  = 100;
const uint32_t HEXAPOD_ERR_USART_IO    = 101;
const uint32_t HEXAPOD_DEV_OPENED      = 102;
const uint32_t HEXAPOD_DEV_CLOSED      = 103;
const uint32_t HEXAPOD_ERR_USART_CLOSE = 104;

const uint32_t HEXAPOD_CMD_CON    = 200;
const uint32_t HEXAPOD_CMD_DISCON = 201;
const uint32_t HEXAPOD_CMD_SEND   = 202;

const uint32_t HEXAPOD_NUM_OF_MOTORS = 6;

const uint32_t PKT_HEADER_SIZE = 2;

const uint32_t PKT_HEADER_MOTOR_INDEX = 0;
const uint32_t PKT_HEADER_CMD_INDEX   = 1;

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
class DSrv_Hexapod_v2 : public DSrv_USART_QT
{
	Q_OBJECT

	friend class DSrv_Hexapod_v2_test;

public:

	DSrv_Hexapod_v2();
	virtual ~DSrv_Hexapod_v2();
	
	// Flag of complete
	volatile bool m_stop_ok;

	// Gets a vector with data sizes
	const std::vector<uint32_t>& getDataSize() const;

private:

	// Preface in debug message
	static const char* PREF;

	// Packet parameters
	int32_t m_motor_id;
	uint32_t m_cmd_id;
	std::vector<uint32_t> m_data_size;

	// Calls start() method from DSrv_USART_QT class and connects signals
	int32_t start();

	// Calls stop() method from DSrv_USART_QT class
	int32_t stop();

	// Parser of the accepted data (override method)
	int32_t dataParser(uint8_t* data, uint32_t size) override final;

public slots:

	// Handles error occured signal
	void onErrorOccured(QSerialPort::SerialPortError err);

	// Handles a command request
	void onSendCommand(const uint32_t cmd, const uint32_t* data, const uint32_t size);

signals:

	// Signal emits when state has changed
	void stateChanged(const uint32_t motor, const uint32_t state, const uint32_t value);
};

//-------------------------------------------------------------------------------------------------
class DSrv_Hexapod_v2_test : public QObject
{
	Q_OBJECT

public:

	// Tests a logic of functions
	static int32_t logic(DSrv_Hexapod_v2& obj, DSrv_Hexapod_v2_test& obj_test);

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_Hexapod_v2& obj);

	// Runs all tests
	static int32_t fullTest();

private:

	DSrv_Hexapod_v2_test() : m_slot_ok(false)
	{
	}

	// Preface in debug message
	static const char* PREF;

	// Flag when values in slot are successful
	volatile bool m_slot_ok;

public slots:

	// Handles state changed signal from DSrv_Hexapod_v2 class
	void onStateChanged(const uint32_t motor, const uint32_t state, const uint32_t value);
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
