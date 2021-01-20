
#include "DSrv.h"

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv::DSrv()
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
DSrv::~DSrv()
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
DSrv::DSrv(DSrv && obj) : DSrv_Storage(std::move(obj))
{
	// Copy all fields
	m_pktRemSize = obj.m_pktRemSize;
	obj.m_pktRemSize = 0;

	m_debug = obj.m_debug;

	PRINT_DBG(m_debug, PREF, "Move constructor");
}

//=================================================================================================
int32_t DSrv_test::data(DSrv_for_test& obj) noexcept
{
	uint8_t data_1[1] = {1};
	uint8_t data_2[1] = {2};
	uint8_t* data_r;
	uint32_t size = 1;
	uint32_t size_r;

	// Get if data is not complete
	if (obj.setData(data_1, size, false) != 0)
	{
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0)
	{
		PRINT_ERR(true, PREF, "getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR(true, PREF, "size_r after first setData()");
			return -1;
		}
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR(true, PREF, "completeData() after first setData()");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0)
	{
		PRINT_ERR(true, PREF, "getData() after first completeData()");
		return -1;
	}
	else
	{
		if (size_r != 1 || data_r[0] != 1)
		{
			PRINT_ERR(true, PREF, "size_r or data_r after first completeData()");
			return -1;
		}
	}

	// Two setData
	if (obj.setData(data_2, size, false) != 0)
	{
		PRINT_ERR(true, PREF, "setData(data_2, ...)");
		return -1;
	}
	if (obj.setData(data_1, size, true) != 0)
	{
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR(true, PREF, "completeData() for two setData()");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0)
	{
		PRINT_ERR(true, PREF, "getData() for two setData()");
		return -1;
	}
	else
	{
		if (size_r != 2 || data_r[0] != 2 || data_r[1] != 1)
		{
			PRINT_ERR(true, PREF, "size_r or data_r for two setData()");
			return -1;
		}
	}

	// MAX_DATA_SIZE + 1 (new data)
	if (obj.setData(data_1, MAX_DATA_SIZE + 1, false) == 0)
	{
		PRINT_ERR(true, PREF, "setData(..., MAX_DATA_SIZE + 1, false)");
		return -1;
	}

	// MAX_DATA_SIZE (add data)
	if (obj.setData(data_1, size, true) != 0)
	{
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.setData(data_1, MAX_DATA_SIZE, true) == 0)
	{
		PRINT_ERR(true, PREF, "setData(..., MAX_DATA_SIZE, true)");
		return -1;
	}

	// Clear method
	if (obj.clearData() != 0)
	{
		PRINT_ERR(true, PREF, "clearData()");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0)
	{
		PRINT_ERR(true, PREF, "getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR(true, PREF, "size_r after clearData()");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_test::move() noexcept
{
	DSrv_for_test obj_1;
	obj_1.m_pktRemSize = 1;

	// Save value of pointer
	const auto m_pktRemSize {obj_1.m_pktRemSize};

	// Apply move constructor
	DSrv_for_test obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_pktRemSize != 0)
	{
		PRINT_ERR(true, PREF, "obj_1.m_pktRemSize != 0");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_pktRemSize != m_pktRemSize)
	{
		PRINT_ERR(true, PREF, "obj_2.m_pktRemSize != m_pktRemSize");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_test::fullTest() noexcept
{
	DSrv_for_test obj;
	obj.setDebug(true, true);

	if (data(obj) != 0)
	{
		PRINT_ERR(true, PREF, "data");
		return -1;
	}

	if (move() != 0)
	{
		PRINT_ERR(true, PREF, "move");
		return -1;
	}

	PRINT_DBG(true, PREF, "Test was successful");
	return 0;
}
