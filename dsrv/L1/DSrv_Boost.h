
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: pure virtual data server template class for Boost protocol class
TODO:
 * move constructor: is need a mutex?
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Boost class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|   YES(*)(**)  |   YES(**)  |
+---------------+------------+
(*)  - NO for the setDebug()
(**) - see Storage class definition
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <utility>  // std::pair; std::move
#include <string>   // std::string

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Test class definition
template <typename Storage> class DSrv_Boost_test;

//-------------------------------------------------------------------------------------------------
// Pure virtual data server template class
template <typename Storage>
class DSrv_Boost : protected Storage
{
	friend class DSrv_Boost_test<Storage>;

public:

	// Data type for send data method (pointer + size)
	typedef  std::pair<const uint8_t *, uint32_t>  Data_send;

	// Data type for data parser method (pointer + size)
	typedef  Data_send  Data_parser;

protected:

	// Constructor
	DSrv_Boost()
	{
		PRINT_DBG(m_debug, "");
	}

	// Destructor
	virtual ~DSrv_Boost()
	{
		PRINT_DBG(m_debug, "");
	}

	// Move constructor
	DSrv_Boost(DSrv_Boost && obj) : Storage(std::move(obj))
	{
		// Copy all fields
		m_pktRemSize = obj.m_pktRemSize;
		obj.m_pktRemSize = 0;

		m_debug = obj.m_debug;

		PRINT_DBG(m_debug, "Move constructor");
	}

	// Enables debug messages
	void setDebug(const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_dsrv;
		Storage::setDebug(d_storage);
	}

	// Size of remaining data
	uint32_t m_pktRemSize {0};

	// Sends data (pure virtual function)
	// (protocol class should realize this function)
	virtual int32_t sendData(const Data_send data) noexcept = 0;
	
	// Receives data (pure virtual function)
	// (protocol class should realize this function)
	virtual int32_t receiveData() noexcept = 0;

	// Parser of the accepted data (pure virtual function)
	// (concrete class should realize this function)
	virtual int32_t dataParser(Data_parser data) noexcept = 0;

private:

	// Enable debug messages
	bool m_debug {true};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
