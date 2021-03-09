
#include "DSrv_Storage_test.h"

#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE
#include <utility>                  // std::move

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_test::pNull(DSrv_Storage & obj) noexcept
{
	uint8_t* data;
	uint32_t size = 1;

	// Test all methods which utilize input pointers
	if (obj.setData(DSrv_Storage::Data_set(nullptr, size), true) == 0)
	{
		PRINT_ERR("setData(nullptr, size, true)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(nullptr, &size)) == 0)
	{
		PRINT_ERR("getData(nullptr, &size)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data, nullptr)) == 0)
	{
		PRINT_ERR("getData(&data, nullptr)");
		return -1;
	}

	// Test all methods which utilize data pointers
	uint8_t* m_completeData = obj.m_completeData;
	uint8_t* m_fillingData = obj.m_fillingData;
	obj.m_completeData = obj.m_fillingData = nullptr;

	if (obj.setData(DSrv_Storage::Data_set(data, size), true) == 0)
	{
		PRINT_ERR("setData(data, size, true) with nullptr == m_...Data");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data, &size)) == 0)
	{
		PRINT_ERR("getData(&data, &size) with nullptr == m_...Data");
		return -1;
	}
	if (obj.completeData() == 0)
	{
		PRINT_ERR("completeData() with nullptr == m_...Data");
		return -1;
	}

	obj.m_completeData = m_completeData;
	obj.m_fillingData = m_fillingData;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_test::data(DSrv_Storage & obj) noexcept
{
	uint8_t data_1[1] = {1};
	uint8_t data_2[1] = {2};
	uint8_t* data_r;
	uint32_t size = 1;
	uint32_t size_r;

	// Get if data is not complete
	if (obj.setData(DSrv_Storage::Data_set(data_1, size), false) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
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
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
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
	if (obj.setData(DSrv_Storage::Data_set(data_2, size), false) != 0)
	{
		PRINT_ERR("setData(data_2, ...)");
		return -1;
	}
	if (obj.setData(DSrv_Storage::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR("completeData() for two setData()");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
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
	if (obj.setData(DSrv_Storage::Data_set(data_1, MAX_DATA_SIZE + 1), false) == 0)
	{
		PRINT_ERR("setData(..., MAX_DATA_SIZE + 1, false)");
		return -1;
	}

	// MAX_DATA_SIZE (add data)
	if (obj.setData(DSrv_Storage::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR("setData(data_1, ...)");
		return -1;
	}
	if (obj.setData(DSrv_Storage::Data_set(data_1, MAX_DATA_SIZE), true) == 0)
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
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
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
int32_t DSrv_Storage_test::move() noexcept
{
	DSrv_Storage obj_1;

	// Save values of pointers
	const auto m_completeData {obj_1.m_completeData};
	const auto m_fillingData {obj_1.m_fillingData};

	// Apply move constructor
	DSrv_Storage obj_2 {std::move(obj_1)};

	// Check obj_1 pointers
	if (obj_1.m_completeData != nullptr || obj_1.m_fillingData != nullptr)
	{
		PRINT_ERR("m_completeData or m_fillingData of obj_1 is not equal nullptr");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_completeData != m_completeData || obj_2.m_fillingData != m_fillingData)
	{
		PRINT_ERR("m_completeData or m_fillingData of obj_1 is not equal saved pointers");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_test::fullTest() noexcept
{
	DSrv_Storage obj;
	obj.setDebug(true);

	if (pNull(obj) != 0)
	{
		PRINT_ERR("pNull");
		return -1;
	}

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
