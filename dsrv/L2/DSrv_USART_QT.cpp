
#include "DSrv_USART_QT.h"

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* DSrv_USART_QT::PREF      = "[DSrv_USART_QT]: ";
const char* DSrv_USART_QT_test::PREF = "[DSrv_USART_QT_test]: ";

//-------------------------------------------------------------------------------------------------
DSrv_USART_QT::DSrv_USART_QT() : m_serialPort(nullptr)
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_USART_QT::~DSrv_USART_QT()
{
	stop();

	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::start()
{
	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	// Check the existing of the QSerialPort class object
	if (m_serialPort != nullptr) {
		PRINT_ERR(true, PREF, "QSerialPort class object already has created");
		return 0;
	}

	// Create a QSerialPort class object
	m_serialPort = new (std::nothrow) QSerialPort();
	if (nullptr == m_serialPort) {
		PRINT_ERR(true, PREF, "Can not allocate a memory (m_serialPort)");
		return -1;
	}

	// Set a port name
	m_serialPort->setPortName("COM17");
	// "COM10" - Nucleo
	// "COM11" - Discovery
	// "COM13" - FT232

	// Set a baud rate
	if (m_serialPort->setBaudRate(QSerialPort::Baud9600) == false) {
		PRINT_ERR(true, PREF, "setBaudRate(9600)");
		stop(false);
		return -1;
	}
	// m_serialPort->setBaudRate(QSerialPort::Baud9600)
	// m_serialPort->setBaudRate(QSerialPort::Baud19200)

	// Set a number of bits in the packet
	if (m_serialPort->setDataBits(QSerialPort::Data8) == false) {
		PRINT_ERR(true, PREF, "setDataBits(Data8)");
		stop(false);
		return -1;
	}

	// Open a port for read and write
	if (m_serialPort->open(QIODevice::ReadWrite) == false) {
		PRINT_ERR(true, PREF, "open(ReadWrite)");
		stop(false);
		return -1;
	}

	// Clear serial port buffers
	if (m_serialPort->clear() == false) {
		PRINT_ERR(true, PREF, "m_serialPort->clear() was not successful");
	}

	// Connect a signal slot for read input message
	connect(m_serialPort, &QSerialPort::readyRead,
	        this, &DSrv_USART_QT::onReadyRead);

	PRINT_DBG(m_debug, PREF, "Serial port has created and opened");

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::stop(const bool lock_mutex)
{
	// Lock a mutex
	if (true == lock_mutex) {
		std::lock_guard<std::mutex> lock(m_mutex);
	}

	if (m_serialPort != nullptr) {

		// Clear serial port buffers
		if (m_serialPort->isOpen() == true) {
			if (m_serialPort->clear() == false) {
				PRINT_ERR(true, PREF, "m_serialPort->clear() was not successful");
			}
		}

		// Delete a QSerialPort class object
		delete m_serialPort;
		m_serialPort = nullptr;
		PRINT_DBG(m_debug, PREF, "Serial port has deleted");
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT::sendData(const uint8_t* const data,
                                const uint32_t size,
                                const char* const, const uint16_t)
{
	// Check the incomming parameter
	if (nullptr == data) {
		PRINT_ERR(true, PREF, "nullptr == data");
		return -1;
	}

	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort) {
		PRINT_ERR(true, PREF, "nullptr == m_serialPort");
		return -1;
	}

	// Block a readyRead() signal
	m_serialPort->blockSignals(true);

	// Write a data
	qint64 sizeWrite = m_serialPort->write(reinterpret_cast<const char*>(data), size);
	if (-1 == sizeWrite) {
		PRINT_ERR(true, PREF, "-1 == sizeWrite");
		return -1;
	} else if (sizeWrite != size) {
		PRINT_ERR(true, PREF, "sizeWrite != size");
		return -1;
	}

	PRINT_DBG(m_debug, PREF, "Data(0x%p) with size(%lu) has sent",
	          data, static_cast<unsigned long>(size));

	// Unblock a readyRead() signal
	m_serialPort->blockSignals(false);

	return 0;
}

//-------------------------------------------------------------------------------------------------
void DSrv_USART_QT::onReadyRead()
{
	// Check the existing of the QSerialPort class object
	if (nullptr == m_serialPort) {
		PRINT_ERR(true, PREF, "nullptr == m_serialPort");
		return;
	}

	// Lock a mutex
	m_mutex.lock();

	// Read data from the serial port
	QByteArray data_read = m_serialPort->readAll();

	// Unlock a mutex
	m_mutex.unlock();

	// Parse the data
	if (data_read.isEmpty() == true) {
		PRINT_ERR(true, PREF, "data_read.isEmpty() == true");
		return;
	}
	if (dataParser(reinterpret_cast<uint8_t*>(data_read.data()), data_read.size()) != 0) {
		PRINT_ERR(true, PREF, "");
		if (clearData() != 0)
		{
			PRINT_ERR(true, PREF, "clearData()");
			return;
		}
		return;
	}

	PRINT_DBG(m_debug, PREF, "Data was read and parsed");
}

//=================================================================================================
int32_t DSrv_USART_QT_test::pNull(DSrv_USART_QT_for_test& obj)
{
	QSerialPort* serialPort = obj.m_serialPort;
	obj.m_serialPort = nullptr;

	if (obj.sendData(reinterpret_cast<const uint8_t*>("data"), 10, nullptr, 0) == 0) {
		PRINT_ERR(true, PREF, "sendData() when nullptr == m_serialPort");
		return -1;
	}

	obj.onReadyRead();
	PRINT_DBG(true, PREF, "onReadyRead() visual check error");

	QSerialPort tmp;
	obj.m_serialPort = &tmp;
	if (obj.start() != 0) {
		PRINT_ERR(true, PREF, "start() when m_serialPort != nullptr");
		return -1;
	}

	obj.m_serialPort = serialPort;

	if (obj.sendData(nullptr, 10, nullptr, 0) == 0) {
		PRINT_ERR(true, PREF, "sendData(nullptr, ...)");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_USART_QT_test::fullTest()
{
	DSrv_USART_QT_for_test obj;
	obj.setDebug(true, true);

	if (pNull(obj) != 0) {
		PRINT_ERR(true, PREF, "pNull");
		return -1;
	}

	PRINT_DBG(true, PREF, "Test was successful");
	return 0;
}
