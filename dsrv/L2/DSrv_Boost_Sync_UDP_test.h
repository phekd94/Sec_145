
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv_Boost_Sync_UDP class
TODO:
 * Move test
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------

#include "DSrv_Boost_Sync_UDP.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_UDP_for_test : public DSrv_Boost_Sync_UDP<Storage, Base>
{
	virtual int32_t dataParser(typename Base<Storage>::Data_parser data) noexcept
	{
		return 0;
	}
};

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
class DSrv_Boost_Sync_UDP_test
{
public:

	DSrv_Boost_Sync_UDP_test() = delete;
	
	// Tests a move constructor
	static int32_t move() noexcept;
	
	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_UDP_test<Storage, Base>::move() noexcept
{
	/*
	DSrv_Boost_Sync_UDP_for_test<Storage, Base> obj_1;
	obj_1.m_pktRemSize = 1;

	// Save value of pointer
	const auto m_pktRemSize {obj_1.m_pktRemSize};

	// Apply move constructor
	DSrv_Boost_Sync_UDP_for_test<Storage, Base> obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_pktRemSize != 0)
	{
		PRINT_ERR("obj_1.m_pktRemSize != 0");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_pktRemSize != m_pktRemSize)
	{
		PRINT_ERR("obj_2.m_pktRemSize != m_pktRemSize");
		return -1;
	}
	*/

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage, template <typename T> class Base>
int32_t DSrv_Boost_Sync_UDP_test<Storage, Base>::fullTest() noexcept
{
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

