
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with USART
TODO:
 * mutex test
 * QObject in move constructor
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
(*) - NO for the setDebug(); see DSrv class definition
*/

//-------------------------------------------------------------------------------------------------
#include <QObject>       // QObject class (for inheritance); Q_OBJECT macros
#include "../L1/DSrv.h"  // DSrv class (for inheritance)

#include <cstdint>      // integer types
#include <QSerialPort>  // QSerialPort class; QSerialPort enumerations
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

	// Creates a seriral port
	Q_INVOKABLE int32_t start(const QString & name,
	                          const QSerialPort::BaudRate baudRate,
	                          const QSerialPort::Parity parity,
	                          const QSerialPort::DataBits dataBits,
	                          const QSerialPort::FlowControl flowControl) noexcept;

	// Deletes a seriral port
	Q_INVOKABLE int32_t stop(const bool lock_mutex = true) noexcept;

protected:

	DSrv_USART_QT();
	virtual ~DSrv_USART_QT();

	// Without copy constructor and override an assignment operator
	// (due to class members as pointer are presented)
	DSrv_USART_QT(DSrv_USART_QT &) = delete;
	DSrv_USART_QT& operator=(const DSrv_USART_QT &) = delete;

	// Move constructor
	DSrv_USART_QT(DSrv_USART_QT && obj);

	// Gets a serial port member pointer
	const QSerialPort * getSerialPort() const noexcept
	{
		return m_serialPort;
	}

	// Enable/disable debug messages
	// (probably the method will be called from another thread)
	void setDebug(const bool d_usart, const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_usart;
		DSrv::setDebug(d_dsrv, d_storage);
	}

	// Sends data (override method)
	Q_INVOKABLE virtual int32_t sendData(DSrv::Data_send data) noexcept override final;

private:

	// Serial port
	QSerialPort * m_serialPort {nullptr};

	// Mutex
	std::mutex m_mutex;

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};

private slots: // They should not can generate an exeption

	// Handles ready read signal
	void onReadyRead() noexcept;

	// Handles error occured signal
	void onErrorOccured(QSerialPort::SerialPortError err) noexcept;
};

//=================================================================================================
// Class for test a DSrv_USART_QT class (with override method)
class DSrv_USART_QT_for_test : public DSrv_USART_QT
{
	virtual int32_t dataParser(DSrv::Data_parser) override final
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_USART_QT class (with test methods)
class DSrv_USART_QT_test
{
public:

	// Only via public static methods
	DSrv_USART_QT_test() = delete;

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_USART_QT_for_test & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
