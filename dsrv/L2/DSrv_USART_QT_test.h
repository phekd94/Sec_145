
#pragma once

//-------------------------------------------------------------------------------------------------
#include "DSrv_USART_QT.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_USART_QT class (with override method)
template <typename Storage, template <typename T> class Base>
class DSrv_USART_QT_for_test : public DSrv_USART_QT<Storage, Base>
{
	virtual int32_t dataParser(typename Base<Storage>::Data_parser) noexcept override final
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_USART_QT class (with test methods)
template <typename Storage, template <typename T> class Base>
class DSrv_USART_QT_test
{
public:

	// Only via public static methods
	DSrv_USART_QT_test() = delete;

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_USART_QT_for_test<Storage, Base> & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT_test<Storage, Base>::pNull(
        DSrv_USART_QT_for_test<Storage, Base> & obj) noexcept
{
	QSerialPort * serialPort = obj.m_serialPort;
	obj.m_serialPort = nullptr;

	if (obj.sendData(Base<Storage>::Data_send(
	                     reinterpret_cast<const uint8_t *>("data"), 10)) == 0)
	{
		PRINT_ERR("sendData() when nullptr == m_serialPort");
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
		PRINT_ERR("start() when m_serialPort != nullptr");
		return -1;
	}

	obj.m_serialPort = serialPort;

	if (obj.sendData(Base<Storage>::Data_send(nullptr, 10)) == 0)
	{
		PRINT_ERR("sendData(nullptr, ...)");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT_test<Storage, Base>::move() noexcept
{
	DSrv_USART_QT_for_test<Storage, Base> obj_1;

	// Save value of pointer
	const auto m_serialPort {obj_1.m_serialPort};

	// Apply move constructor
	DSrv_USART_QT_for_test<Storage, Base> obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_serialPort != nullptr)
	{
		PRINT_ERR("obj_1.m_serialPort != nullptr");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_serialPort != m_serialPort)
	{
		PRINT_ERR("obj_2.m_serialPort != m_serialPort");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_USART_QT_test<Storage, Base>::fullTest() noexcept
{
	DSrv_USART_QT_for_test<Storage, Base> obj;

	if (pNull(obj) != 0)
	{
		PRINT_ERR("pNull");
		return -1;
	}

	if (move() != 0)
	{
		PRINT_ERR("move");
		return -1;
	}

	PRINT_DBG(true, "Test was successful");
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
