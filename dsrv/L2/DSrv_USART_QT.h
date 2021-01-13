
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with USART
TODO:
 * sendData(): blockSignals() (race condition sendData() and slotReadyRead()); also stop()
 * mutex test
 * Q_INVOKABLE instead slots
FIXME:
DANGER:
NOTE:
 * Doesn't suit for large data stream due to slot is used for read data

Sec_145::DSrv_USART_QT class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |    YES(*)  |
+---------------+------------+
(*) - see DSrv class definition
*/

//-------------------------------------------------------------------------------------------------
#include <QObject>       // QObject class (for inheritance); Q_OBJECT macros
#include "../L1/DSrv.h"  // DSrv class (for inheritance)

#include <cstdint>      // integer types
#include <QSerialPort>  // QSerialPort class
#include <list>         // std::list
#include <string>       // std::string
#include <mutex>        // std::mutex, std::lock_guard

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class implements work with USART
class DSrv_USART_QT : public QObject, public DSrv
{
	Q_OBJECT

	friend class DSrv_USART_QT_test;

public:

	// Gets available port names
	static const std::list<std::string> getPortNames();

protected:

	DSrv_USART_QT();
	virtual ~DSrv_USART_QT();

	// Without copy constructor and override an assignment operator
	// (due to class members as pointer are presented)
	DSrv_USART_QT(DSrv_USART_QT&) = delete;
	DSrv_USART_QT& operator=(const DSrv_USART_QT&) = delete;

	// Creates a seriral port
	int32_t start() noexcept;

	// Deletes a seriral port
	int32_t stop(const bool lock_mutex = true) noexcept;

	// Gets a serial port member pointer
	const QSerialPort* getSerialPort() const noexcept
	{
		return m_serialPort;
	}

	// Sends data (override method)
	virtual int32_t sendData(const uint8_t* const data,
	                         const uint32_t size,
	                         const char* const address,
	                         const uint16_t port) noexcept override final;

private:

	// Preface in debug message
	constexpr static const char* PREF {"[DSrv_USART_QT]: "};

	// Serial port
	QSerialPort* m_serialPort {nullptr};

	// Mutex
	std::mutex m_mutex;

private slots: // They should not can generate an exeption

	// Handles ready read signal
	void onReadyRead() noexcept;

	// Handles signal for start an USART server
	void onStart() noexcept;

	// Handles signal for stop an USART server
	void onStop() noexcept;
};

//=================================================================================================
// Class for test a DSrv_USART_QT class (with override method)
class DSrv_USART_QT_for_test : public DSrv_USART_QT
{
	virtual int32_t dataParser(uint8_t*, uint32_t) override final
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_USART_QT class (with test methods)
class DSrv_USART_QT_test
{
public:

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_USART_QT_for_test& obj) noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;

private:

	// Only via public static methods
	DSrv_USART_QT_test()
	{
	}

	// Preface in debug message
	constexpr static const char* PREF {"[DSrv_USART_QT_test]: "};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
