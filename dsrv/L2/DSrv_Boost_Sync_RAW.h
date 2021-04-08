
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class implements work with RAW socket by using Boost library
TODO:
 * Define move constructor
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

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
//namespace asio {
//namespace ip {
class mraw
{

public:

	// The type of a RAW endpoint
	typedef boost::asio::ip::basic_endpoint<mraw> endpoint;
  
	// Default construction, compiler complains otherwise
	//explicit raw();
	
	// Construct to represent the IPv4 RAW protocol
	static mraw v4()
	{
		return mraw();
	}

	// Construct to represent the IPv6 RAW protocol
	static mraw v6()
	{
		return mraw();
	}
	
	// Obtain an identifier for the type of the protocol
	int type() const
	{
		return SOCK_RAW;
	}
	
	// Obtain an identifier for the protocol
	int protocol() const
	{
		return 50; // IPSEC-ESP;
	}
	
	// Obtain an identifier for the protocol family
	int family() const
	{
		return AF_INET;
	}
  
};
//} // namespace ip
//} // namespace asio

//-------------------------------------------------------------------------------------------------
namespace Sec_145 
{

class DSrv_Boost_Sync_RAW
{

public:

	void test()
	{
		boost::asio::io_service io_service;
		boost::asio::basic_raw_socket<mraw> socket_(io_service);
		
		try {
			socket_.open();
			socket_.send_to(
				boost::asio::buffer("ABCD", 4), 
				mraw::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 50001)
			);
		} catch (std::exception& e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
		
		std::cout << "OK" << std::endl;
	}

};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

