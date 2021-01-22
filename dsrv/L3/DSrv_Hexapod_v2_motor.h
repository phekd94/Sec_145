
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with TTX080 series motor
TODO:
 * wait the answer via watchdog timer
 * add CRC check in the dataParser() method
 * test for dataParser()
FIXME:
DANGER:
NOTE:

Sec_145::Dsrv_Hexapod_v2 class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
(*) - see DSrv_USART_QT class definition
*/

//-------------------------------------------------------------------------------------------------
#include "../L2/DSrv_USART_QT.h"  // DSrv_USART_QT class (for inheritance)

#include <QObject>  // Q_OBJECT macros
#include <cstdint>  // integer types

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class DSrv_Hexapod_v2_motor : public DSrv_USART_QT
{
	Q_OBJECT

	// friend class DSrv_Hexapod_v2_motor_test;

public:

	DSrv_Hexapod_v2_motor(uint8_t motor_id);
	virtual ~DSrv_Hexapod_v2_motor();

	// Move constructor
	DSrv_Hexapod_v2_motor(DSrv_Hexapod_v2_motor && obj);

	// Calculates a CRC
	uint16_t calcCRC(const uint8_t *data, uint8_t length) const noexcept;

	// Gets a motor id
	uint8_t getMotorId()
	{
		return m_motor_id;
	}

	// Reads holding 16 bits registers
	Q_INVOKABLE int32_t readHoldingRegs(const uint16_t address, const uint8_t count) noexcept;

	// Wtires single 16 bits register
	Q_INVOKABLE int32_t writeSingleReg(const uint16_t address, const uint16_t val) noexcept;

private:

	// Preface in debug message
	constexpr static const char* PREF {"[DSrv_Hexapod_v2]: "};

	// Motor id
	uint8_t m_motor_id;

	// Function code, address, data, number of bytes for request
	uint8_t m_funcCode {0};
	uint16_t m_addressReq {0};
	uint16_t m_val {0};
	uint8_t m_numOfBytesReq {0};

	// Parser of the accepted data (override method)
	int32_t dataParser(uint8_t* data, uint32_t size) noexcept override final;

public slots: // They should not generate exeptions

};

//=================================================================================================
// Class for test a DSrv_Hexapod_v2 class (with test methods)
//class DSrv_Hexapod_v2_motor_test : public QObject


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
