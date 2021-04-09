
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with UDP packets by using Boost library
TODO:
 * Define move constructor
 * Add mutex for m_socket
FIXME:
DANGER:
 * If CRC is needed buffer should be more than size on sizeof(uint32_t) 
NOTE:

Sec_145::DSrv_Boost_Sync_UDP class
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
#include "boost/crc.hpp"   // Boost.CRC library

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage, template <typename T> class Base> class DSrv_Boost_Sync_UDP_test;

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_UDP : public Base<Storage>
{

	friend class DSrv_Boost_Sync_UDP_test<Storage, Base>;
	
public:
	
	// Maximal lenght of UDP packet
	constexpr static const uint32_t UDP_max_lenght {1500};

protected:

	DSrv_Boost_Sync_UDP() : m_socket(m_io_context)
	{
		PRINT_DBG(m_debug, "");
	}

	virtual ~DSrv_Boost_Sync_UDP()
	{
		stop();
	
		PRINT_DBG(m_debug, "");
	}
	
	// Without copy constructor and override an assignment operator
	DSrv_Boost_Sync_UDP(const DSrv_Boost_Sync_UDP &) = delete;
	DSrv_Boost_Sync_UDP& operator=(const DSrv_Boost_Sync_UDP &) = delete;

	// Move constructor	
	DSrv_Boost_Sync_UDP(DSrv_Boost_Sync_UDP && obj) : Base<Storage>(std::move(obj))
	{
		// TODO: define move constructor
		
		PRINT_DBG(m_debug, "Move constructor");
	}
	
	// Starts the communication
	void start(const uint16_t port);
	
	// Stops the communication
	void stop();
	
	// Sends data to host and port (override method)
	virtual int32_t sendData(typename Base<Storage>::Data_send data,
	                         const std::string host, const uint16_t port, 
	                         const bool crc = false) noexcept;
	
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
	
	// UDP socket
	boost::asio::ip::udp::socket m_socket;
	
	// Information about sender
	boost::asio::ip::udp::endpoint m_senderEndpoint;
	
	// Endpoint for send
	boost::asio::ip::udp::endpoint m_endpointSend;
	
	// CRC
	boost::crc_32_type m_crc;
	
	// Buffer size
	constexpr static const uint32_t m_bufferSize {UDP_max_lenght};
	
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
void DSrv_Boost_Sync_UDP<Storage, Base>::
start(const uint16_t port)
{
	try {
		// Open a socket
		m_socket.open(boost::asio::ip::udp::v4()); // can be without argument
		
		// Bind a socket
		m_socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
		
		PRINT_DBG(m_debug, "Communication is started (port: %u)", 
		                   static_cast<unsigned int>(port));
	}
	catch (std::exception & ex)
	{
		PRINT_ERR("%s", ex.what());
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
void DSrv_Boost_Sync_UDP<Storage, Base>::
stop()
{
	try {
		// Get a current port
		uint16_t port {m_socket.local_endpoint().port()};
	
		// Close a socket
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
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_UDP<Storage, Base>::
sendData(const typename Base<Storage>::Data_send data) noexcept
{
	try {
		// Send data
		m_socket.send_to(boost::asio::buffer(data.first, data.second), m_endpointSend);
		
		PRINT_DBG(m_debug, "Data is send to %s (port = %u) (size = %u)",
		                   m_endpointSend.address().to_string().c_str(),
		                   static_cast<unsigned int>(m_endpointSend.port()),
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
int32_t DSrv_Boost_Sync_UDP<Storage, Base>::
sendData(typename Base<Storage>::Data_send data, 
         const std::string host, const uint16_t port, const bool crc) noexcept
{
	try {
		// Set endpoint for send
		m_endpointSend = boost::asio::ip::udp::endpoint(
		                      boost::asio::ip::address::from_string(host), port);
		
		// Add CRC
		if (crc == true)
		{
			// Calculate a CRC
			m_crc.process_bytes(data.first, data.second);
			
			// Add CRC to data
			*reinterpret_cast<uint32_t *>(
			   const_cast<uint8_t *>(data.first + data.second)) = m_crc.checksum();
			data.second += sizeof(uint32_t);
			
			PRINT_DBG(m_debug, "CRC for send data: 0x%x", m_crc.checksum());
		}
		
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
int32_t DSrv_Boost_Sync_UDP<Storage, Base>::
receiveData() noexcept
{
	try {
		// Check of data availability
		if (m_socket.available() != 0)
		{
			// Receive data
			uint32_t receiveBytes = m_socket.receive_from(
			        boost::asio::buffer(m_bufferReceive, m_bufferSize), m_senderEndpoint);
			        
			PRINT_DBG(m_debug, "Data is received from %s (port = %u) (size = %u)", 
			                   m_senderEndpoint.address().to_string().c_str(),
			                   static_cast<unsigned int>(m_senderEndpoint.port()), 
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
		else
		{
			return 1;
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

