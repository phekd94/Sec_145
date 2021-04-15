
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with RAW socket by using Boost library
TODO:
 * Define move constructor
 * mutex for socket
 * template for raw for IPPROTO_ESP
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Boost_Sync_RAW class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
(**) - see Storage and Base classes definition
*/

//-------------------------------------------------------------------------------------------------
#include "boost/asio.hpp"  // Boost.Asio library
#include <cstdint>         // integer types
#include <exception>       // std::exception
#include <netinet/in.h>    // Identifier for the protocols

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
class RAW
{

public:

	// The type of a RAW endpoint
	typedef boost::asio::ip::basic_endpoint<RAW> endpoint;
  	
	// Construct to represent the IPv4 RAW protocol
	static RAW v4() noexcept
	{
		return RAW(AF_INET, IPPROTO_ESP); // TODO: check receiving from the IPPROTO_UDP (echo for all)
	}

	// Construct to represent the IPv6 RAW protocol
	static RAW v6() noexcept
	{
		return RAW(AF_INET6, IPPROTO_ESP);
	}
	
	RAW(int family = AF_INET, int protocol = IPPROTO_ESP) : m_family(family), m_protocol(protocol)
	{
		PRINT_DBG(true, "");
	}
	
	// Gets an identifier for the type of the protocol
	int type() const noexcept
	{
		return SOCK_RAW;
	}
	
	// Gets an identifier for the protocol
	int protocol() const noexcept
	{
		return m_protocol;
	}
	
	// Gets an identifier for the protocol family
	int family() const noexcept
	{
		return m_family;
	}
	
private:
	
	// Protocol family
	int m_family;
	
	// Identifier for the protocol
	int m_protocol;
};

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_RAW : public Base<Storage>
{

public:
	
	// Maximal lenght of IP packet (MTU)
	constexpr static const uint32_t RAW_max_lenght {1500};

public:

	DSrv_Boost_Sync_RAW() : m_socket(m_io_context)
	{
		PRINT_DBG(m_debug, "");
	}

	virtual ~DSrv_Boost_Sync_RAW()
	{
		stop();
	
		PRINT_DBG(m_debug, "");
	}
	
	// Without copy constructor and override an assignment operator
	DSrv_Boost_Sync_RAW(const DSrv_Boost_Sync_RAW &) = delete;
	DSrv_Boost_Sync_RAW& operator=(const DSrv_Boost_Sync_RAW &) = delete;

	// TODO: Move constructor	
	DSrv_Boost_Sync_RAW(DSrv_Boost_Sync_RAW && obj) = delete;
	
	// Starts the communication
	void start();
	
	// Stops the communication
	void stop();
	
	// Sends data to host and port (override method)
	virtual int32_t sendData(typename Base<Storage>::Data_send data,
	                         const std::string host) noexcept;
	   
	// Receives data (override method)
	virtual int32_t receiveData() noexcept override final;
	
	// Enables debug messages
	void setDebug(const bool d_usart, const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_usart;
		Base<Storage>::setDebug(d_dsrv, d_storage);
	}
	
private:

	// IO context
	boost::asio::io_context m_io_context;
	
	// RAW socket
	boost::asio::basic_raw_socket<RAW> m_socket;
	
	// Information about sender
	RAW::endpoint m_senderEndpoint;
	
	// Endpoint for send
	RAW::endpoint m_endpointSend;

	// Buffer size
	constexpr static const uint32_t m_bufferSize {RAW_max_lenght};
	
	// Buffer for receive data
	uint8_t m_bufferReceive[m_bufferSize];	
	
	// Enable debug messages
	bool m_debug {true};
	
	// Sends data (override method)
	virtual int32_t sendData(
	   const typename Base<Storage>::Data_send data) noexcept override final;
};

//=================================================================================================
template <typename Storage, template <typename T> class Base>
void DSrv_Boost_Sync_RAW<Storage, Base>::
start()
{
	try {
		// Open a socket
		m_socket.open();
		
		PRINT_DBG(m_debug, "Communication is started");
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
void DSrv_Boost_Sync_RAW<Storage, Base>::
stop()
{
	try {
		// Close a socket
		m_socket.close();
		
		PRINT_DBG(m_debug, "Communication is stoped");
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_RAW<Storage, Base>::
sendData(const typename Base<Storage>::Data_send data) noexcept
{
	try {
		// Send data
		m_socket.send_to(boost::asio::buffer(data.first, data.second), m_endpointSend);
		
		PRINT_DBG(m_debug, "Data is send to %s (size = %u)",
		                   m_endpointSend.address().to_string().c_str(),
		                   static_cast<unsigned int>(data.second));
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_RAW<Storage, Base>::
sendData(typename Base<Storage>::Data_send data, 
         const std::string host) noexcept
{
	try {
		// Set endpoint for send (without port)
		m_endpointSend = RAW::endpoint(boost::asio::ip::address::from_string(host), 0);
		
		// Send data
		if (sendData(data) != 0)
		{
			PRINT_ERR("sendData(data)");
			return -1;
		}
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_RAW<Storage, Base>::
receiveData() noexcept
{
	try {
		// Check of data availability
		if (m_socket.available() == 0)
			return 1;
	
		// Read all available packets
		while (m_socket.available() != 0) 
		{			
			// Receive data
			uint32_t receiveBytes = m_socket.receive_from(
			        boost::asio::buffer(m_bufferReceive, m_bufferSize), m_senderEndpoint);
			
			PRINT_DBG(m_debug, "Data is received from %s (size = %u)", 
			                   m_senderEndpoint.address().to_string().c_str(), 
			                   static_cast<unsigned int>(receiveBytes));
			
			// Parse data
			if (this->dataParser(
			         typename Base<Storage>::Data_parser(m_bufferReceive, receiveBytes)) != 0)
			{
				PRINT_ERR("dataParser()");
				
				// Clear data
				if (Storage::clearData() != 0)
				{
					PRINT_ERR("clearData()");
				}
				return -1;
			}
		}
		
		return 0;
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

