
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for work with TTX080 series motor
TODO:
 * test for dataParser()
 * pair or kortej for data and size and success(bool)
 * m_debug member
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
#include <tuple>    // std::tuple
#include <utility>  // std::pair

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class DSrv_Hexapod_v2_motor : public DSrv_USART_QT
{
	Q_OBJECT

	// friend class DSrv_Hexapod_v2_motor_test;

public:

	// Oscillation direction
	enum class OSC_DIR : uint8_t {
		UP = 1,
		DOWN
	};

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

	// Moves to home position
	Q_INVOKABLE int32_t home() noexcept;

	// Moves to the position
	Q_INVOKABLE int32_t moveInPos() noexcept;

	// Start and stop the oscillation
	Q_INVOKABLE int32_t startOsc(OSC_DIR dir) noexcept;
	Q_INVOKABLE int32_t stopOsc() noexcept;

private:

	// Motor id
	uint8_t m_motor_id;

	// Function code, address, data, number of bytes for request
	uint8_t m_funcCode_req {0};
	uint16_t m_address_req {0};
	uint16_t m_val_req {0};
	uint8_t m_numOfBytes_req {0};

	// Message types
	constexpr static const uint8_t read {0x03};
	constexpr static const uint8_t write {0x06};

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
	uint32_t m_watchdog_val {500};

	// Move modes
	enum class MODE {
		NONE,
		HOME,
		MOVE_IN_POS,
		OSC
	} m_mode {MODE::NONE};

	// Oscillation direction
	OSC_DIR m_osc_dir {OSC_DIR::UP};

	// Registers addresses; rigisters(16 bits) counts; register value
	std::tuple<const uint16_t, const uint8_t, int32_t>   Home_Position1   {0x1772u, 2, 0};
	std::tuple<const uint16_t, const uint8_t>            IEG_MOTION       {0x10DDu, 1};
	std::tuple<const uint16_t, const uint8_t, int32_t>   Pfeedback        {0x017Au, 2, 0};
	std::tuple<const uint16_t, const uint8_t, uint16_t>  OEG_MOTION       {0x0069u, 1, 0};
	std::tuple<const uint16_t, const uint8_t>            IEG_MOVE_EDGE    {0x10DFu, 1};
	std::tuple<const uint16_t, const uint8_t, uint16_t>  OEG_MOVE_IN_POS  {0x006Cu, 1, 0};
	std::tuple<const uint16_t, const uint8_t>            IEG_MOVE_LEVEL   {0x10DEu, 1};

	// Parser of the accepted data (override method)
	int32_t dataParser(DSrv::Data_parser data) noexcept override final;

	// Controls of the move
	int32_t moveControl(const uint8_t mode,
	                    const uint8_t * const data,
	                    const uint32_t size) noexcept;

public slots: // They should not generate exeptions

	// Slot for watchdog timer
	void onWatchdog() noexcept;

signals:

	// Signal for watchdog timer
	void toWatchdog(uint32_t motor_id);

	// Signal for complete the home move
	void toHomeComplete(uint32_t motor_id);

	// Signal for complete the move in position
	void toMoveInPosComplete(uint32_t motor_id);

	// Signal for complete the move in position in oscillation mode
	void toMoveInPosCompleteOsc(uint32_t motor_id);
};

//=================================================================================================
// Class for test a DSrv_Hexapod_v2 class (with test methods)
//class DSrv_Hexapod_v2_motor_test : public QObject


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
