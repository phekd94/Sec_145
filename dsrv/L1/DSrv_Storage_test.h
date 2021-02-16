
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv_Storage class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv_Storage.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_Storage class
class DSrv_Storage_test
{
public:

	// Only via public static methods
	DSrv_Storage_test() = delete;

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_Storage & obj) noexcept;

	// Tests a work with data
	static int32_t data(DSrv_Storage & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
