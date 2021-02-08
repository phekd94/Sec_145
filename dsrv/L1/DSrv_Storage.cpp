
#include "DSrv_Storage.h"
#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE

#include <cstring>       // std::memcpy
#include <utility>       // std::swap
#include <system_error>  // std::system_error
#include <utility>       // std::move

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Storage::DSrv_Storage()
{
	// Allocate a memory for the data
	m_completeData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_completeData)
	{
		PRINT_ERR(true, "Can not allocate a memory (m_completeData)");
		return;
	}
	m_fillingData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_fillingData)
	{
		PRINT_ERR(true, "Can not allocate a memory (m_fillingData)");
		return;
	}

	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Storage::~DSrv_Storage()
{
	// Delete a memory for the data
	if (m_completeData != nullptr)
	{
		delete [] m_completeData;
		m_completeData = nullptr;
	}
	if (m_fillingData != nullptr)
	{
		delete [] m_fillingData;
		m_fillingData = nullptr;
	}

	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Storage::DSrv_Storage(DSrv_Storage && obj)
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::lock_guard<std::mutex> lock_obj(obj.m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return;
	}

	// Copy all fields
	m_completeData = obj.m_completeData;
	obj.m_completeData = nullptr;

	m_fillingData = obj.m_fillingData;
	obj.m_fillingData = nullptr;

	m_fillingIndex = obj.m_fillingIndex;
	m_completeSize = obj.m_completeSize;
	m_debug = obj.m_debug;

	PRINT_DBG(m_debug, "Move constructor");
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::setData(Data_set data, const bool add) noexcept
{
	// Check the incoming parameter
	if (nullptr == data.first)
	{
		PRINT_ERR(true, "nullptr == data.first");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR(true, "Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the size of the input data
	if (   (true == add && m_fillingIndex + data.second > MAX_DATA_SIZE)
	    || (false == add && data.second > MAX_DATA_SIZE))
	{
		PRINT_ERR(true, "Size of the data is too much "
		                "(size = %lu, fillingIndex = %lu, add = %s)",
		                static_cast<unsigned long>(data.second),
		                static_cast<unsigned long>(m_fillingIndex),
		                true == add ? "true" : "false");
		return -1;
	}

	PRINT_DBG(m_debug, "Add the data(0x%p) with size(%5lu) to the 0x%p with size(%5lu)",
	                   data.first,
	                   static_cast<unsigned long>(data.second),
	                   m_fillingData,
	                   static_cast<unsigned long>(m_fillingIndex));

	// Add the data and add the size to the fillingIndex
	std::memcpy(m_fillingData + m_fillingIndex, data.first, data.second);
	m_fillingIndex += data.second;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::getData(Data_get data) noexcept
{
	// Check the incoming parameters
	if (nullptr == data.first)
	{
		PRINT_ERR(true, "nullptr == data.first");
		return -1;
	}
	if (nullptr == data.second)
	{
		PRINT_ERR(true, "nullptr == data.second");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR(true, "Memory for data have not been allocated");
		*data.first = nullptr;
		*data.second = 0;
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	PRINT_DBG(m_debug, "Data for geter: 0x%p with size(%5lu)",
	                    m_completeData,
	                    static_cast<unsigned long>(m_completeSize));

	// Set the data and the size
	*data.first = m_completeData;
	*data.second = m_completeSize;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::clearData() noexcept
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Set the fillingIndex and the completeSize to 0
	m_fillingIndex = 0;
	m_completeSize = 0;

	PRINT_DBG(m_debug, "Data have cleared");

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::completeData() noexcept
{
	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR(true, "Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR(true, "Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Exchange pointers
	std::swap(m_fillingData, m_completeData);

	// Set the completeSize
	m_completeSize = m_fillingIndex;

	// Set the fillingIndex to 0
	m_fillingIndex = 0;

	PRINT_DBG(m_debug, "Data(0x%p) with size(%5lu) is complete",
	                   m_completeData,
	                   static_cast<unsigned long>(m_completeSize));

	return 0;
}

//=================================================================================================
int32_t DSrv_Storage_test::pNull(DSrv_Storage & obj) noexcept
{
	uint8_t* data;
	uint32_t size = 1;

	// Test all methods which utilize input pointers
	if (obj.setData(DSrv_Storage::Data_set(nullptr, size), true) == 0)
	{
		PRINT_ERR(true, "setData(nullptr, size, true)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(nullptr, &size)) == 0)
	{
		PRINT_ERR(true, "getData(nullptr, &size)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data, nullptr)) == 0)
	{
		PRINT_ERR(true, "getData(&data, nullptr)");
		return -1;
	}

	// Test all methods which utilize data pointers
	uint8_t* m_completeData = obj.m_completeData;
	uint8_t* m_fillingData = obj.m_fillingData;
	obj.m_completeData = obj.m_fillingData = nullptr;

	if (obj.setData(DSrv_Storage::Data_set(data, size), true) == 0)
	{
		PRINT_ERR(true, "setData(data, size, true) with nullptr == m_...Data");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data, &size)) == 0)
	{
		PRINT_ERR(true, "getData(&data, &size) with nullptr == m_...Data");
		return -1;
	}
	if (obj.completeData() == 0)
	{
		PRINT_ERR(true, "completeData() with nullptr == m_...Data");
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
		PRINT_ERR(true, "setData(data_1, ...)");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR(true, "getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR(true, "size_r after first setData()");
			return -1;
		}
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR(true, "completeData() after first setData()");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR(true, "getData() after first completeData()");
		return -1;
	}
	else
	{
		if (size_r != 1 || data_r[0] != 1)
		{
			PRINT_ERR(true, "size_r or data_r after first completeData()");
			return -1;
		}
	}

	// Two setData
	if (obj.setData(DSrv_Storage::Data_set(data_2, size), false) != 0)
	{
		PRINT_ERR(true, "setData(data_2, ...)");
		return -1;
	}
	if (obj.setData(DSrv_Storage::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR(true, "setData(data_1, ...)");
		return -1;
	}
	if (obj.completeData() != 0)
	{
		PRINT_ERR(true, "completeData() for two setData()");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR(true, "getData() for two setData()");
		return -1;
	}
	else
	{
		if (size_r != 2 || data_r[0] != 2 || data_r[1] != 1)
		{
			PRINT_ERR(true, "size_r or data_r for two setData()");
			return -1;
		}
	}

	// MAX_DATA_SIZE + 1 (new data)
	if (obj.setData(DSrv_Storage::Data_set(data_1, MAX_DATA_SIZE + 1), false) == 0)
	{
		PRINT_ERR(true, "setData(..., MAX_DATA_SIZE + 1, false)");
		return -1;
	}

	// MAX_DATA_SIZE (add data)
	if (obj.setData(DSrv_Storage::Data_set(data_1, size), true) != 0)
	{
		PRINT_ERR(true, "setData(data_1, ...)");
		return -1;
	}
	if (obj.setData(DSrv_Storage::Data_set(data_1, MAX_DATA_SIZE), true) == 0)
	{
		PRINT_ERR(true, "setData(..., MAX_DATA_SIZE, true)");
		return -1;
	}

	// Clear method
	if (obj.clearData() != 0)
	{
		PRINT_ERR(true, "clearData()");
		return -1;
	}
	if (obj.getData(DSrv_Storage::Data_get(&data_r, &size_r)) != 0)
	{
		PRINT_ERR(true, "getData()");
		return -1;
	}
	else
	{
		if (size_r != 0)
		{
			PRINT_ERR(true, "size_r after clearData()");
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
		PRINT_ERR(true, "m_completeData or m_fillingData of obj_1 is not equal nullptr");
		return -1;
	}

	// Check obj_2 pointers
	if (obj_2.m_completeData != m_completeData || obj_2.m_fillingData != m_fillingData)
	{
		PRINT_ERR(true, "m_completeData or m_fillingData of obj_1 is not equal saved pointers");
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
		PRINT_ERR(true, "pNull");
		return -1;
	}

	if (data(obj) != 0)
	{
		PRINT_ERR(true, "data");
		return -1;
	}

	if (move() != 0)
	{
		PRINT_ERR(true, "move");
		return -1;
	}

	PRINT_DBG(true, "Test was successful");
	return 0;
}
