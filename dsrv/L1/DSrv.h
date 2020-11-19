
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: pure virtual data data server class
TODO:
 * sendData(..., address, port, and other parameters) for the all protocols
FIXME:
DANGER:
NOTE:

Sec_145::DSrv class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |   YES(**)  |
+---------------+------------+
(*)  - NO for the m_debug, DSrv_Storage::setDebug()
(**) - see DSrv_Storage class definition
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv_Storage.h"           // DSrv_Storage class (for inheritance)
#include <cstdint>                  // integer types
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

	// Enable/disable a debug output via printDebug.cpp/.h
	// (probably the method will be called from another thread)
	void setDebug(const bool d, const bool d_storage) noexcept
	{
		m_debug = d;
		DSrv_Storage::setDebug(d_storage);
	}

protected:

	DSrv();
	virtual ~DSrv();

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};

	// Size of remaining data
	uint32_t m_pktRemSize {0};

	// Sends data (pure virtual function)
	// (protocol class should realize this function)
	// (probably the method will be called from another thread)
	virtual int32_t sendData(const uint8_t* const data,
	                         const uint32_t size,
	                         const char* const address,
	                         const uint16_t port) noexcept = 0;

	// Parser of the accepted data (pure virtual function)
	// (concrete class should realize this function)
	virtual int32_t dataParser(uint8_t* data, uint32_t size) = 0;

private:

	// Preface in debug message
	constexpr static const char* PREF {"[DSrv]: "};
};

//=================================================================================================
// Class for test a DSrv class (with override methods)
class DSrv_for_test : public DSrv
{
	virtual int32_t sendData(const uint8_t* const,
	                         const uint32_t,
	                         const char* const,
	                         const uint16_t) noexcept override final
	{
		return 0;
	}

	virtual int32_t dataParser(uint8_t*, uint32_t) override final
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv class (with test methods)
class DSrv_test
{
public:

	// Tests a work with data
	static int32_t data(DSrv_for_test& obj) noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;

private:

	// Only via public static methods
	DSrv_test()
	{
	}

	// Preface in debug message
	constexpr static const char* PREF {"[DSrv_test]: "};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
