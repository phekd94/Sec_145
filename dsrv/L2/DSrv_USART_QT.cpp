
#include "DSrv_USART_QT.h"

#include <system_error>     // std::system_error
#include <exception>        // std::exception
#include <QSerialPortInfo>  // availablePorts()

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const std::list<std::string> DSrv_USART_QT::getPortNames()
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
		PRINT_ERR(true,
		          "Exception from Qt functions or during push_back() has been occured: %s",
		          obj.what());
		res.clear();
	}

	return res;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::start(const QString& name,
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
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the existing of the QSerialPort class object
	if (m_serialPort != nullptr)
	{
		PRINT_ERR(true, "QSerialPort class object already has created");
		return 0;
	}

	// Create a QSerialPort class object
	m_serialPort = new (std::nothrow) QSerialPort();
	if (nullptr == m_serialPort)
	{
		PRINT_ERR(true, "Can not allocate a memory (m_serialPort)");
		return -1;
	}

	try {

	// Set a port name
	m_serialPort->setPortName(name);

	// Set a baud rate
	if (m_serialPort->setBaudRate(baudRate) == false)
	{
		PRINT_ERR(true, "setBaudRate(%d)", static_cast<int>(baudRate));
		stop(false);
		return -1;
	}

	// Set a parity checking mode
	if (m_serialPort->setParity(parity) == false)
	{
		PRINT_ERR(true, "setParity(%d)", static_cast<int>(parity));
		stop(false);
		return -1;
	}

	// Set a number of bits in the packet
	if (m_serialPort->setDataBits(dataBits) == false)
	{
		PRINT_ERR(true, "setDataBits(%d)", static_cast<int>(dataBits));
		stop(false);
		return -1;
	}

	// Set a flow control
	if (m_serialPort->setFlowControl(flowControl) == false)
	{
		PRINT_ERR(true, "setFlowControl(%d)", static_cast<int>(flowControl));
		stop(false);
		return -1;
	}

	// Open a port for read and write
	if (m_serialPort->open(QIODevice::ReadWrite) == false)
	{
		PRINT_ERR(true, "open(ReadWrite) port %s", name.toStdString().c_str());
		stop(false);
		return -1;
	}

	// Clear serial port buffers
	if (m_serialPort->clear() == false)
	{
		PRINT_ERR(true, "m_serialPort->clear() was not successful");
	}

	// Connect a signal slot for read input message and for handle an error
	connect(m_serialPort, &QSerialPort::readyRead,
	        this, &DSrv_USART_QT::onReadyRead);
	connect(m_serialPort, &QSerialPort::errorOccurred,
	        this, &DSrv_USART_QT::onErrorOccured);

	} // Catch an exception from Qt functions
	catch (std::exception& obj)
	{
		PRINT_ERR(true, "Exception from Qt functions has been occured: %s", obj.what());
		return -1;
	}

	PRINT_DBG(m_debug, "Serial port %s has created and opened", name.toStdString().c_str());

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::stop(const bool lock_mutex) noexcept
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
			PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
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
				PRINT_ERR(true, "m_serialPort->clear() was not successful");
			}
		}

		} // Catch an exception from Qt functions
		catch (std::exception& obj)
		{
			PRINT_ERR(true, "Exception from Qt functions has been occured: %s", obj.what());
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
DSrv_USART_QT::DSrv_USART_QT()
{
	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_USART_QT::~DSrv_USART_QT()
{
	stop(true);

	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_USART_QT::DSrv_USART_QT(DSrv_USART_QT && obj) : DSrv(std::move(obj))
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::lock_guard<std::mutex> lock_obj(obj.m_mutex);
	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return;
	}

	// Copy all fields
	m_serialPort = obj.m_serialPort;
	obj.m_serialPort = nullptr;

	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::sendData(DSrv::Data_send data) noexcept
{
	// Check the incomming parameter
	if (nullptr == data.first)
	{
		PRINT_ERR(true, "nullptr == data.first");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort)
	{
		PRINT_ERR(true, "nullptr == m_serialPort");
		return -1;
	}

	// Write a data
	qint64 sizeWrite;

	try {
		sizeWrite = m_serialPort->write(reinterpret_cast<const char*>(data.first), data.second);
	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, "Exception from m_serialPort->write() has been occured: %s", obj.what());
		return -1;
	}

	if (-1 == sizeWrite)
	{
		PRINT_ERR(true, "-1 == sizeWrite");
		return -1;
	}
	else if (data.second != sizeWrite)
	{
		PRINT_ERR(true, "size != sizeWrite");
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
void DSrv_USART_QT::onReadyRead() noexcept
{
	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort)
	{
		PRINT_ERR(true, "nullptr == m_serialPort");
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
		PRINT_ERR(true, "data_read.isEmpty() == true");
		return;
	}

	if (dataParser(DSrv::Data_parser(
	                   reinterpret_cast<uint8_t*>(data_read.data()), data_read.size())) != 0)
	{
		PRINT_ERR(true, "dataParser()");
		if (DSrv_Storage::clearData() != 0)
		{
			PRINT_ERR(true, "clearData()");
		}
		return;
	}

	} // Catch an exception from mutex.lock()
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return;
	}
	// Catch an exception from Qt functions
	catch (std::exception& obj)
	{
		PRINT_ERR(true, "Exception from Qt functions has been occured: %s", obj.what());
		return;
	}

	PRINT_DBG(m_debug, "Data was read and parsed");
}

//-------------------------------------------------------------------------------------------------
void DSrv_USART_QT::onErrorOccured(QSerialPort::SerialPortError err) noexcept
{
	PRINT_ERR(true, "Error signal from QSerialPort with error: %d", static_cast<int>(err));
}

//=================================================================================================
int32_t DSrv_USART_QT_test::pNull(DSrv_USART_QT_for_test& obj) noexcept
{
	QSerialPort * serialPort = obj.m_serialPort;
	obj.m_serialPort = nullptr;

	if (obj.sendData(DSrv::Data_send(reinterpret_cast<const uint8_t *>("data"), 10)) == 0)
	{
		PRINT_ERR(true, "sendData() when nullptr == m_serialPort");
		return -1;
	}

	obj.onReadyRead();
	PRINT_DBG(true, "onReadyRead() visual check error");

	QSerialPort tmp;
	obj.m_serialPort = &tmp;
	if (obj.start(QString(),
	              QSerialPort::Baud19200, QSerialPort::NoParity,
	              QSerialPort::Data8, QSerialPort::NoFlowControl) != 0)
	{
		PRINT_ERR(true, "start() when m_serialPort != nullptr");
		return -1;
	}

	obj.m_serialPort = serialPort;

	if (obj.sendData(DSrv::Data_send(nullptr, 10)) == 0)
	{
		PRINT_ERR(true, "sendData(nullptr, ...)");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT_test::move() noexcept
{
	DSrv_USART_QT_for_test obj_1;

	// Save value of pointer
	const auto m_serialPort {obj_1.m_serialPort};

	// Apply move constructor
	DSrv_USART_QT_for_test obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_serialPort != nullptr)
	{
		PRINT_ERR(true, "obj_1.m_serialPort != nullptr");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_serialPort != m_serialPort)
	{
		PRINT_ERR(true, "obj_2.m_serialPort != m_serialPort");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT_test::fullTest() noexcept
{
	DSrv_USART_QT_for_test obj;
	obj.setDebug(true, true);

	if (pNull(obj) != 0)
	{
		PRINT_ERR(true, "pNull");
		return -1;
	}

	if (move() != 0)
	{
		PRINT_ERR(true, "move");
		return -1;
	}

	PRINT_DBG(true, "Test was successful");
	return 0;
}
