
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: test class for DSrv class
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include "DSrv.h"  // DSrv class

#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for test a DSrv class (with override methods)
template<typename Storage>
class DSrv_for_test : public DSrv<Storage>
{
	virtual int32_t sendData(typename DSrv<Storage>::Data_send) noexcept override final
	{
		return 0;
	}

	virtual int32_t dataParser(typename DSrv<Storage>::Data_parser) noexcept override final
	{
		return 0;
	}
};

//=================================================================================================
// Class for test a DSrv class (with test methods)
template<typename Storage>
class DSrv_test
{
public:

	// Only via public static methods
	DSrv_test() = delete;

	// Tests a work with data
	static int32_t data(DSrv_for_test<Storage> & obj) noexcept;

	// Tests a move constructor
	static int32_t move() noexcept;

	// Runs all tests
	static int32_t fullTest() noexcept;
};

//-------------------------------------------------------------------------------------------------
template<typename Storage>
int32_t DSrv_test<Storage>::data(DSrv_for_test<Storage> & obj) noexcept
{
	uint8_t data_1[1] = {1};
	uint8_t data_2[1] = {2};
	uint8_t * data_r;
	uint32_t size {1};
	uint32_t size_r;

	// Get if data is not complete
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_1, size), false) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.getData(DSrv_for_test<Storage>::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR("getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR("size_r after first setData()");
			return -1;
		}
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() after first setData()");
		return -1;
	}
	if (obj.getData(DSrv_for_test<Storage>::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR("getData() after first completeData()");
		return -1;
	}
	else
	{
		if (size_r != 1 || data_r[0] != 1)
		{
			PRINT_ERR("size_r or data_r after first completeData()");
			return -1;
		}
	}

	// Two setData
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_2, size), false) != 0)
	{
		PRINT_ERR("setData(data_2, ...)");
		return -1;
	}
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() for two setData()");
		return -1;
	}
	if (obj.getData(DSrv_for_test<Storage>::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR("getData() for two setData()");
		return -1;
	}
	else
	{
		if (size_r != 2 || data_r[0] != 2 || data_r[1] != 1)
		{
			PRINT_ERR("size_r or data_r for two setData()");
			return -1;
		}
	}

	// MAX_DATA_SIZE + 1 (new data)
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_1, MAX_DATA_SIZE + 1), false) == 0)
	{
		PRINT_ERR("setData(..., MAX_DATA_SIZE + 1, false)");
		return -1;
	}

	// MAX_DATA_SIZE (add data)
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.setData(DSrv_for_test<Storage>::Data_set(data_1, MAX_DATA_SIZE), true) == 0)
	{
		PRINT_ERR("setData(..., MAX_DATA_SIZE, true)");
		return -1;
	}

	// Clear method
	if (obj.clearData() != 0)
	{
		PRINT_ERR("clearData()");
		return -1;
	}
	if (obj.getData(DSrv_for_test<Storage>::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR("getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR("size_r after clearData()");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
template<typename Storage>
int32_t DSrv_test<Storage>::move() noexcept
{
	DSrv_for_test<Storage> obj_1;
	obj_1.m_pktRemSize = 1;

	// Save value of pointer
	const auto m_pktRemSize {obj_1.m_pktRemSize};

	// Apply move constructor
	DSrv_for_test obj_2 {std::move(obj_1)};

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
template<typename Storage>
int32_t DSrv_test<Storage>::fullTest() noexcept
{
	DSrv_for_test<Storage> obj;
	obj.setDebug(true, true);

	if (data(obj) != 0)
	{
		PRINT_ERR("data");
		return -1;
	}

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
