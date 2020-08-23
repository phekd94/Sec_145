
#include "DSrv_Storage.h"
#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE

#include <cstring>                  // std::memcpy
#include <utility>                  // std::swap

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* DSrv_Storage::PREF      = "[DSrv_Storage]: ";
const char* DSrv_Storage_test::PREF = "[DSrv_Storage_test]: ";

//-------------------------------------------------------------------------------------------------
DSrv_Storage::DSrv_Storage() : m_completeData(nullptr),
                               m_fillingData(nullptr),
                               m_fillingIndex(0),
                               m_completeSize(0),
                               m_debug(true)
{
	// Allocate a memory for the data
	m_completeData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_completeData) {
		PRINT_ERR(true, PREF, "Can not allocate a memory (m_completeData)");
		return;
	}
	m_fillingData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_fillingData) {
		PRINT_ERR(true, PREF, "Can not allocate a memory (m_fillingData)");
		return;
	}

	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Storage::~DSrv_Storage()
{
	// Delete a memory for the data
	if (m_completeData != nullptr) {
		delete [] m_completeData;
	}
	if (m_fillingData != nullptr) {
		delete [] m_fillingData;
	}

	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::setData(const uint8_t* const data, const uint32_t size, const bool add)
{
	// Check the incoming parameter
	if (nullptr == data) {
		PRINT_ERR(true, PREF, "nullptr == data");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData) {
		PRINT_ERR(true, PREF, "Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	// Check the size of the input data
	if (   (true == add && m_fillingIndex + size > MAX_DATA_SIZE)
	    || (false == add && size > MAX_DATA_SIZE)) {
		PRINT_ERR(true, PREF, "Size of the data is too much "
		          "(size = %lu, fillingIndex = %lu, add = %s)",
		          static_cast<unsigned long>(size),
		          static_cast<unsigned long>(m_fillingIndex),
		          true == add ? "true" : "false");
		return -1;
	}

	PRINT_DBG(m_debug, PREF, "Add the data(0x%p) with size(%5lu) to the 0x%p with size(%5lu)",
	                         data,
	                         static_cast<unsigned long>(size),
	                         m_fillingData,
	                         static_cast<unsigned long>(m_fillingIndex));

	// Add the data and add the size to the fillingIndex
	std::memcpy(m_fillingData + m_fillingIndex, data, size);
	m_fillingIndex += size;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::getData(uint8_t** const data, uint32_t* const size)
{
	// Check the incoming parameters
	if (nullptr == data) {
		PRINT_ERR(true, PREF, "nullptr == data");
		return -1;
	}
	if (nullptr == size) {
		PRINT_ERR(true, PREF, "nullptr == size");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData) {
		PRINT_ERR(true, PREF, "Memory for data have not been allocated");
		*data = nullptr;
		*size = 0;
		return -1;
	}

	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	PRINT_DBG(m_debug, PREF, "Data for geter: 0x%p with size(%5lu)",
	                         m_completeData,
	                         static_cast<unsigned long>(m_completeSize));

	// Set the data and the size
	*data = m_completeData;
	*size = m_completeSize;

	return 0;
}

//-------------------------------------------------------------------------------------------------
void DSrv_Storage::clearData()
{
	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	// Set the fillingIndex and the completeSize to 0
	m_fillingIndex = 0;
	m_completeSize = 0;

	PRINT_DBG(m_debug, PREF, "Data have cleared");
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage::completeData()
{
	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData) {
		PRINT_ERR(true, PREF, "Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	std::lock_guard<std::mutex> lock(m_mutex);

	// Exchange pointers
	std::swap(m_fillingData, m_completeData);

	// Set the completeSize
	m_completeSize = m_fillingIndex;

	// Set the fillingIndex to 0
	m_fillingIndex = 0;

	PRINT_DBG(m_debug, PREF, "Data(0x%p) with size(%5lu) is complete",
	                         m_completeData,
	                         static_cast<unsigned long>(m_completeSize));

	return 0;
}

//=================================================================================================
int32_t DSrv_Storage_test::pNull(DSrv_Storage& obj)
{
	uint8_t* data;
	uint32_t size = 1;

	// Test all methods which utilize input pointers
	if (obj.setData(nullptr, size, true) == 0) {
		PRINT_ERR(true, PREF, "setData(nullptr, size, true)");
		return -1;
	}
	if (obj.getData(nullptr, &size) == 0) {
		PRINT_ERR(true, PREF, "getData(nullptr, &size)");
		return -1;
	}
	if (obj.getData(&data, nullptr) == 0) {
		PRINT_ERR(true, PREF, "getData(&data, nullptr)");
		return -1;
	}

	// Test all methods which utilize data pointers
	uint8_t* m_completeData = obj.m_completeData;
	uint8_t* m_fillingData = obj.m_fillingData;
	obj.m_completeData = obj.m_fillingData = nullptr;

	if (obj.setData(data, size, true) == 0) {
		PRINT_ERR(true, PREF, "setData(data, size, true) with nullptr == m_...Data");
		return -1;
	}
	if (obj.getData(&data, &size) == 0) {
		PRINT_ERR(true, PREF, "getData(&data, &size) with nullptr == m_...Data");
		return -1;
	}
	if (obj.completeData() == 0) {
		PRINT_ERR(true, PREF, "completeData() with nullptr == m_...Data");
		return -1;
	}

	obj.m_completeData = m_completeData;
	obj.m_fillingData = m_fillingData;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_test::data(DSrv_Storage& obj)
{
	uint8_t data_1[1] = {1};
	uint8_t data_2[1] = {2};
	uint8_t* data_r;
	uint32_t size = 1;
	uint32_t size_r;

	// Get if data is not complete
	if (obj.setData(data_1, size, false) != 0) {
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0) {
		PRINT_ERR(true, PREF, "getData()");
		return -1;
	} else {
		if (size_r != 0) {
			PRINT_ERR(true, PREF, "size_r after first setData()");
			return -1;
		}
	}
	if (obj.completeData() != 0) {
		PRINT_ERR(true, PREF, "completeData() after first setData()");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0) {
		PRINT_ERR(true, PREF, "getData() after first completeData()");
		return -1;
	} else {
		if (size_r != 1 || data_r[0] != 1) {
			PRINT_ERR(true, PREF, "size_r or data_r after first completeData()");
			return -1;
		}
	}

	// Two setData
	if (obj.setData(data_2, size, false) != 0) {
		PRINT_ERR(true, PREF, "setData(data_2, ...)");
		return -1;
	}
	if (obj.setData(data_1, size, true) != 0) {
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.completeData() != 0) {
		PRINT_ERR(true, PREF, "completeData() for two setData()");
		return -1;
	}
	if (obj.getData(&data_r, &size_r) != 0) {
		PRINT_ERR(true, PREF, "getData() for two setData()");
		return -1;
	} else {
		if (size_r != 2 || data_r[0] != 2 || data_r[1] != 1) {
			PRINT_ERR(true, PREF, "size_r or data_r for two setData()");
			return -1;
		}
	}

	// MAX_DATA_SIZE + 1 (new data)
	if (obj.setData(data_1, MAX_DATA_SIZE + 1, false) == 0) {
		PRINT_ERR(true, PREF, "setData(..., MAX_DATA_SIZE + 1, false)");
		return -1;
	}

	// MAX_DATA_SIZE (add data)
	if (obj.setData(data_1, size, true) != 0) {
		PRINT_ERR(true, PREF, "setData(data_1, ...)");
		return -1;
	}
	if (obj.setData(data_1, MAX_DATA_SIZE, true) == 0) {
		PRINT_ERR(true, PREF, "setData(..., MAX_DATA_SIZE, true)");
		return -1;
	}

	// Clear method
	obj.clearData();
	if (obj.getData(&data_r, &size_r) != 0) {
		PRINT_ERR(true, PREF, "getData()");
		return -1;
	} else {
		if (size_r != 0) {
			PRINT_ERR(true, PREF, "size_r after clearData()");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_test::fullTest()
{
	DSrv_Storage obj;
	obj.setDebug(true);

	if (pNull(obj) != 0) {
		PRINT_ERR(true, PREF, "pNull");
		return -1;
	}

	if (data(obj) != 0) {
		PRINT_ERR(true, PREF, "data");
		return -1;
	}

	PRINT_DBG(true, PREF, "Test was successful");
	return 0;
}
