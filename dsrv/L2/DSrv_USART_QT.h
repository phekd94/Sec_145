
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with USART
TODO: * sendData(): blockSignals() (race condition sendData() and slotReadyRead())
	  * mutex test
	  * include QSerialPort replace by public inheritance
FIXME:
DANGER:
NOTE: * Doesn't suit for large data stream due to slot for read data

Sec_145::DSrv_USART_QT class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES(*)   |     YES    |
+---------------+------------+
(*) - NO for the start(), stop() methods
*/

//-------------------------------------------------------------------------------------------------
#include <QObject>           // QObject class (for inheritance); Q_OBJECT macros
#include "dsrv/L1/DSrv.h"    // DSrv class (for inheritance)

#include <cstdint>           // integer types
#include <QSerialPort>       // QSerialPort class
#include <mutex>             // std::mutex, std::lock_guard

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
class DSrv_USART_QT : public QObject, public DSrv
{
	Q_OBJECT

	friend class DSrv_USART_QT_test;

protected:

	DSrv_USART_QT();
	virtual ~DSrv_USART_QT();

	// Sends data (override method)
	virtual int32_t sendData(const uint8_t* const data,
	                         const uint32_t size,
	                         const char* const address,
	                         const uint16_t port) override final;

	// Creates a seriral port
	int32_t start();

	// Deletes a seriral port
	int32_t stop(const bool lock_mutex = true);

	// Gets a serial port member
	const QSerialPort* getSerialPort() const
	{ return m_serialPort; }

private:

	// Preface in debug message
	static const char* PREF;

	// Serial port
	QSerialPort* m_serialPort;

	// Mutex
	std::mutex m_mutex;

private slots:

	// Handles ready read signal
	void onReadyRead();
};

//-------------------------------------------------------------------------------------------------
class DSrv_USART_QT_for_test : public DSrv_USART_QT
{
	virtual int32_t dataParser(uint8_t*, uint32_t) override final
	{ return 0; }
};

class DSrv_USART_QT_test
{
public:

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_USART_QT_for_test& obj);

	// Runs all tests
	static int32_t fullTest();

private:

	// Preface in debug message
	static const char* PREF;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
