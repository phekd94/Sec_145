
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
(*) - NO for the setDebug();


see DSrv class definition


*/

//-------------------------------------------------------------------------------------------------
#include <QObject>       // QObject class (for inheritance); Q_OBJECT macros

#include <cstdint>      // integer types
#include <QSerialPort>  // QSerialPort class; QSerialPort enumerations
#include <list>         // std::list
#include <string>       // std::string
#include <mutex>        // std::mutex, std::lock_guard

#include <system_error>     // std::system_error
#include <exception>        // std::exception
#include <QSerialPortInfo>  // availablePorts()

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage, template <typename T> class Base> class DSrv_USART_QT_test;

//-------------------------------------------------------------------------------------------------
// Class implements work with USART
template <typename Storage, template <typename T> class Base>
class DSrv_USART_QT :
        //public QObject,
        public Base<Storage>
{
	//Q_OBJECT

	friend class DSrv_USART_QT_test<Storage, Base>;

public:

	// Gets available port names
	static const std::list<std::string> getPortNames();

	// Creates a seriral port
	//Q_INVOKABLE
	int32_t start(const QString & name,
	                          const QSerialPort::BaudRate baudRate,
	                          const QSerialPort::Parity parity,
	                          const QSerialPort::DataBits dataBits,
	                          const QSerialPort::FlowControl flowControl) noexcept;

	// Deletes a seriral port	
	//Q_INVOKABLE
	int32_t stop(const bool lock_mutex = true) noexcept;

protected:

	DSrv_USART_QT()
	{
		PRINT_DBG(m_debug, "");
	}

	virtual ~DSrv_USART_QT()
	{
		stop(true);

		PRINT_DBG(m_debug, "");
	}

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
		Base<Storage>::setDebug(d_dsrv, d_storage);
	}

	// Sends data (override method)
	Q_INVOKABLE virtual int32_t sendData(
	        typename Base<Storage>::Data_send data) noexcept override final;

private:

	// Serial port
	QSerialPort * m_serialPort {nullptr};

	// Mutex
	std::mutex m_mutex;

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};

//private slots: // They should not can generate an exeption

	// Handles ready read signal
	void onReadyRead() noexcept;

	// Handles error occured signal
	void onErrorOccured(QSerialPort::SerialPortError err) noexcept
	{
		PRINT_ERR("Error signal from QSerialPort with error: %d", static_cast<int>(err));
	}
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
const std::list<std::string> DSrv_USART_QT<Storage, Base>::getPortNames()
{
	std::list<std::string> res;

	try {

		// Get list with information about serial ports
		QList<QSerialPortInfo> list {QSerialPortInfo::availablePorts()};

		// Fill the result list
		for (const QSerialPortInfo& node : list)
		{
			res.push_back(node.portName().toStdString());
		}

	} // Catch an exception from Qt functions or from push_back()
	catch (std::exception& obj)
	{
		PRINT_ERR("Exception from Qt functions or during push_back() has been occured: %s",
		          obj.what());
		res.clear();
	}

	return res;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
DSrv_USART_QT<Storage, Base>::DSrv_USART_QT(DSrv_USART_QT && obj) : Base<Storage>(std::move(obj))
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::lock_guard<std::mutex> lock_obj(obj.m_mutex);
	}
	catch (std::system_error& obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return;
	}

	// Copy all fields
	m_serialPort = obj.m_serialPort;
	obj.m_serialPort = nullptr;

	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT<Storage, Base>::start(const QString& name,
                                            const QSerialPort::BaudRate baudRate,
                                            const QSerialPort::Parity parity,
                                            const QSerialPort::DataBits dataBits,
                                            const QSerialPort::FlowControl flowControl) noexcept
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error& obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the existing of the QSerialPort class object
	if (m_serialPort != nullptr)
	{
		PRINT_ERR("QSerialPort class object already has created");
		return 0;
	}

	// Create a QSerialPort class object
	m_serialPort = new (std::nothrow) QSerialPort();
	if (nullptr == m_serialPort)
	{
		PRINT_ERR("Can not allocate a memory (m_serialPort)");
		return -1;
	}

	try {

	// Set a port name
	m_serialPort->setPortName(name);

	// Set a baud rate
	if (m_serialPort->setBaudRate(baudRate) == false)
	{
		PRINT_ERR("setBaudRate(%d)", static_cast<int>(baudRate));
		stop(false);
		return -1;
	}

	// Set a parity checking mode
	if (m_serialPort->setParity(parity) == false)
	{
		PRINT_ERR("setParity(%d)", static_cast<int>(parity));
		stop(false);
		return -1;
	}

	// Set a number of bits in the packet
	if (m_serialPort->setDataBits(dataBits) == false)
	{
		PRINT_ERR("setDataBits(%d)", static_cast<int>(dataBits));
		stop(false);
		return -1;
	}

	// Set a flow control
	if (m_serialPort->setFlowControl(flowControl) == false)
	{
		PRINT_ERR("setFlowControl(%d)", static_cast<int>(flowControl));
		stop(false);
		return -1;
	}

	// Open a port for read and write
	if (m_serialPort->open(QIODevice::ReadWrite) == false)
	{
		PRINT_ERR("open(ReadWrite) port %s", name.toStdString().c_str());
		stop(false);
		return -1;
	}

	// Clear serial port buffers
	if (m_serialPort->clear() == false)
	{
		PRINT_ERR("m_serialPort->clear() was not successful");
	}

	// Connect a signal slot for read input message and for handle an error
/*	connect(m_serialPort, &QSerialPort::readyRead,
			this, &DSrv_USART_QT<Storage, Base>::onReadyRead);
	connect(m_serialPort, &QSerialPort::errorOccurred,
			this, &DSrv_USART_QT<Storage, Base>::onErrorOccured);*/

	} // Catch an exception from Qt functions
	catch (std::exception& obj)
	{
		PRINT_ERR("Exception from Qt functions has been occured: %s", obj.what());
		return -1;
	}

	PRINT_DBG(m_debug, "Serial port %s has created and opened", name.toStdString().c_str());

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT<Storage, Base>::stop(const bool lock_mutex) noexcept
{
	// Check the lock_mutex parameter
	if (true == lock_mutex)
	{
		// Lock a mutex
		try {
			std::lock_guard<std::mutex> lock(m_mutex);
		}
		catch (std::system_error& obj)
		{
			PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
			return -1;
		}
	}

	if (m_serialPort != nullptr)
	{
		try {

		// Clear serial port buffers
		if (m_serialPort->isOpen() == true)
		{
			if (m_serialPort->clear() == false)
			{
				PRINT_ERR("m_serialPort->clear() was not successful");
			}
		}

		} // Catch an exception from Qt functions
		catch (std::exception& obj)
		{
			PRINT_ERR("Exception from Qt functions has been occured: %s", obj.what());
		}

		PRINT_DBG(m_debug, "Serial port %s has deleted",
		                   m_serialPort->portName().toStdString().c_str());

		// Delete (with close) a QSerialPort class object
		delete m_serialPort;
		m_serialPort = nullptr;
	}
	else
	{
		PRINT_DBG(m_debug, "QSerialPort class object has not been created");
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT<Storage, Base>::sendData(typename Base<Storage>::Data_send data) noexcept
{
	// Check the incomming parameter
	if (nullptr == data.first)
	{
		PRINT_ERR("nullptr == data.first");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error& obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort)
	{
		PRINT_ERR("nullptr == m_serialPort");
		return -1;
	}

	// Write a data
	qint64 sizeWrite;

	try {
		sizeWrite = m_serialPort->write(reinterpret_cast<const char*>(data.first), data.second);
	}
	catch (std::exception& obj)
	{
		PRINT_ERR("Exception from m_serialPort->write() has been occured: %s", obj.what());
		return -1;
	}

	if (-1 == sizeWrite)
	{
		PRINT_ERR("-1 == sizeWrite");
		return -1;
	}
	else if (data.second != sizeWrite)
	{
		PRINT_ERR("size != sizeWrite");
		return -1;
	}

	// Flush the device at COM port
	m_serialPort->flush();

	PRINT_DBG(m_debug, "Data(0x%p) with size(%lu) has sent",
	                   data.first,
	                   static_cast<unsigned long>(data.second));

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
void DSrv_USART_QT<Storage, Base>::onReadyRead() noexcept
{
	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort)
	{
		PRINT_ERR("nullptr == m_serialPort");
		return;
	}

	try {

	// Lock a mutex
	m_mutex.lock();

	// Read data from the serial port
	QByteArray data_read = m_serialPort->readAll();

	// Unlock a mutex
	m_mutex.unlock();

	// Parse the data
	if (data_read.isEmpty() == true)
	{
		PRINT_ERR("data_read.isEmpty() == true");
		return;
	}

	if (dataParser(Base<Storage>::Data_parser(
	                   reinterpret_cast<uint8_t*>(data_read.data()), data_read.size())) != 0)
	{
		PRINT_ERR("dataParser()");
		if (Storage::clearData() != 0)
		{
			PRINT_ERR("clearData()");
		}
		return;
	}

	} // Catch an exception from mutex.lock()
	catch (std::system_error& obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return;
	}
	// Catch an exception from Qt functions
	catch (std::exception& obj)
	{
		PRINT_ERR("Exception from Qt functions has been occured: %s", obj.what());
		return;
	}

	PRINT_DBG(m_debug, "Data was read and parsed");
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
