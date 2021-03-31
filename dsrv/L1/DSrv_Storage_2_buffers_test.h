
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv_Storage_2_buffers class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv_Storage_2_buffers.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_Storage_2_buffers class
class DSrv_Storage_2_buffers_test
{
public:

	// Only via public static methods
	DSrv_Storage_2_buffers_test() = delete;

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_Storage_2_buffers & obj) noexcept;

	// Tests a work with data
	static int32_t data(DSrv_Storage_2_buffers & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;
	
	// Test a CRC
	static int32_t crc(DSrv_Storage_2_buffers & obj) noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
