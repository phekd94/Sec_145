
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with TTX080 series motor
TODO:
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
#include <QTimer>   // QTimer class

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

	// Motor id
	uint8_t m_motor_id;

	// Function code, address, data, number of bytes for request
	uint8_t m_funcCode_req {0};
	uint16_t m_address_req {0};
	uint16_t m_val_req {0};
	uint8_t m_numOfBytes_req {0};

	// For data parser
	  // For all type message
	std::pair<bool, uint8_t> m_motor_id_pars {false, 0};
	std::pair<bool, uint8_t> m_funcCode_pars {false, 0};
	std::pair<bool, uint8_t> m_crc_l_pars {false, 0};
	std::pair<bool, uint8_t> m_crc_h_pars {false, 0};
	  // For read
	std::pair<bool, uint8_t> m_count_pars {false, 0};
	  // For write
	std::pair<bool, uint8_t> m_address_l_pars {false, 0};
	std::pair<bool, uint8_t> m_address_h_pars {false, 0};
	std::pair<bool, uint8_t> m_val_l_pars {false, 0};
	std::pair<bool, uint8_t> m_val_h_pars {false, 0};

	// Watchdog timer
	QTimer m_watchdog;

	// Parser of the accepted data (override method)
	int32_t dataParser(uint8_t* data, uint32_t size) noexcept override final;

public slots: // They should not generate exeptions

	// Slot for watchdog timer
	void onWatchdog() noexcept;

signals:

	// Signal for watchdog timer
	void toWatchdog(uint32_t motor_id);
};

//=================================================================================================
// Class for test a DSrv_Hexapod_v2 class (with test methods)
//class DSrv_Hexapod_v2_motor_test : public QObject


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
