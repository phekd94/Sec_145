
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv_Boost class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv_Boost.h"  // DSrv_Boost class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv_Boost class (with override methods)
template <typename Storage>
class DSrv_Boost_for_test : public DSrv_Boost<Storage>
{
	virtual int32_t sendData(typename DSrv_Boost<Storage>::Data_send) noexcept override final
	{
		return 0;
	}
	
	virtual int32_t receiveData() noexcept override final
	{
		return 0;
	}

	virtual int32_t dataParser(typename DSrv_Boost<Storage>::Data_parser) noexcept override final
	{
		return 0;
	}
};

//=================================================================================================
// Class for test a DSrv_Boost class (with test methods)
template <typename Storage>
class DSrv_Boost_test
{

public:

	// Only via public static methods
	DSrv_Boost_test() = delete;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
template <typename Storage>
int32_t DSrv_Boost_test<Storage>::move() noexcept
{
	DSrv_Boost_for_test<Storage> obj_1;
	obj_1.m_pktRemSize = 1;

	// Save value of pointer
	const auto m_pktRemSize {obj_1.m_pktRemSize};

	// Apply move constructor
	DSrv_Boost_for_test<Storage>  obj_2 {std::move(obj_1)};

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

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename Storage>
int32_t DSrv_Boost_test<Storage>::fullTest() noexcept
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
