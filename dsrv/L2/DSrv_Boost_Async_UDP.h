
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with UDP packets by using Boost library
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Boost_Async_UDP class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "boost/asio.hpp"  // Boost.Asio library
#include <cstdint>         // integer types
#include <exception>       // std::exception

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage, template <typename T> class Base> class DSrv_Boost_Async_UDP_test;

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Async_UDP : public Base<Storage>
{

	friend class DSrv_Boost_Async_UDP_test<Storage, Base>;

protected:

	DSrv_Boost_Async_UDP() : m_socket(m_io_context)
	{
		PRINT_DBG(m_debug, "");
	}

	virtual ~DSrv_Boost_Async_UDP()
	{
		stop();
	
		PRINT_DBG(m_debug, "");
	}
	
	// Without copy constructor and override an assignment operator
	DSrv_Boost_Async_UDP(const DSrv_Boost_Async_UDP &) = delete;
	DSrv_Boost_Async_UDP& operator=(const DSrv_Boost_Async_UDP &) = delete;

	// Move constructor	
	DSrv_Boost_Async_UDP(DSrv_Boost_Async_UDP && obj) : Base<Storage>(std::move(obj))
	{
		PRINT_DBG(m_debug, "Move constructor");
	}
	
	// Starts the communication
	void start();
	
	// Stops the communication
	void stop();
	
	// Sends data (override method)
	virtual int32_t sendData(typename Base<Storage>::Data_send data) noexcept override final
	{}
	
	// Receives data () // virtual override final;
	int32_t receiveData(typename Base<Storage>::Data_send data) noexcept
	{}
	
	// Enable/disable debug messages
	// (probably the method will be called from another thread)
	void setDebug(const bool d_usart, const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_usart;
		Base<Storage>::setDebug(d_dsrv, d_storage);
	}

private:

	// IO context
	boost::asio::io_context m_io_context;
	
	// UDP socket
	boost::asio::ip::udp::socket m_socket;
	
	// Maximal lenght of UDP packet
	constexpr static const uint32_t UDP_max_lenght {1500};
	
	// Buffer size
	constexpr static const uint32_t m_bufferSize {UDP_max_lenght};
	
	// Buffer for receive data
	uint8_t m_bufferReceive[m_bufferSize];
	
	// Information about sender
	boost::asio::ip::udp::endpoint m_senderEndpoint;
	
	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};
};

//=================================================================================================
template <typename Storage, template <typename T> Base>
void DSrv_Boost_Async_UDP<Storage, Base>::start(uint16_t port)
{
	try {
		// Open a socket
		m_socket.open(boost::asio::ip::udp::v4());
		
		// Bind a socket
		m_socket.bind(boost::asio::ip::upd::endpoint(boost::asio::ip::udp::v4(), port));
		
		PRINT_DBG(m_debug, "Communication is started (port: %u)", 
		                   static_cast<unsigned int>(port));
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> Base>
void DSrv_Boost_Async_UDP<Storage, Base>::stop()
{
	try {
		// Get a current port
		uint16_t port {m_socket.local_endpoint().port()};
	
		// Open a socket
		m_socket.close();
		
		PRINT_DBG(m_debug, "Communication is stoped (port: %u)", 
		                   static_cast<unsigned int>(port));
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

