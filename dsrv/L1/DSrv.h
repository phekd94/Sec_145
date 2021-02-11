
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: pure virtual data data server class
TODO:
 * move constructor: is need a mutex?
FIXME:
DANGER:
NOTE:

Sec_145::DSrv class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |   YES(**)  |
+---------------+------------+
(*)  - NO for the setDebug(); see DSrv_Storage class definition
(**) - see DSrv_Storage class definition
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv_Storage.h"           // DSrv_Storage class (for inheritance)
#include <cstdint>                  // integer types
#include <utility>                  // std::pair
#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Pure virtual data data server class
class DSrv : protected DSrv_Storage
{
	friend class DSrv_test;

public:

	// Data type for send data method (pointer + size)
	using Data_send = const std::pair<const uint8_t * const, const uint32_t>;

	// Data type for data parser method (pointer + size)
	using Data_parser = std::pair<const uint8_t *, uint32_t>;

protected:

	DSrv();
	virtual ~DSrv();

	// Move constructor
	DSrv(DSrv && obj);

	// Size of remaining data
	uint32_t m_pktRemSize {0};

	// Enable/disable debug messages
	// (probably the method will be called from another thread)
	void setDebug(const bool d_dsrv, const bool d_storage) noexcept
	{
		m_debug = d_dsrv;
		DSrv_Storage::setDebug(d_storage);
	}

	// Sends data (pure virtual function)
	// (protocol class should realize this function)
	// (probably the method will be called from another thread)
	virtual int32_t sendData(Data_send data) noexcept = 0;

	// Parser of the accepted data (pure virtual function)
	// (concrete class should realize this function)
	virtual int32_t dataParser(Data_parser data) = 0;

private:

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};
};

//=================================================================================================
// Class for test a DSrv class (with override methods)
class DSrv_for_test : public DSrv
{
	virtual int32_t sendData(DSrv::Data_send) noexcept override final
	{
		return 0;
	}

	virtual int32_t dataParser(Data_parser) override final
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv class (with test methods)
class DSrv_test
{
public:

	// Only via public static methods
	DSrv_test() = delete;

	// Tests a work with data
	static int32_t data(DSrv_for_test & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
