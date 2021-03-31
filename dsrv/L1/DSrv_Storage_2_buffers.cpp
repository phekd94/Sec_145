
#include "DSrv_Storage_2_buffers.h"
#include "../other/DSrv_Defines.h"  // MAX_DATA_SIZE

#include <cstring>       // std::memcpy
#include <utility>       // std::swap
#include <system_error>  // std::system_error
#include <utility>       // std::move

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
DSrv_Storage_2_buffers::DSrv_Storage_2_buffers()
{
	// Allocate a memory for the data
	m_completeData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_completeData)
	{
		PRINT_ERR("Can not allocate a memory (m_completeData)");
		return;
	}
	m_fillingData = new (std::nothrow) uint8_t[MAX_DATA_SIZE];
	if (nullptr == m_fillingData)
	{
		PRINT_ERR("Can not allocate a memory (m_fillingData)");
		return;
	}

	PRINT_DBG(m_debug, "");
}

//-------------------------------------------------------------------------------------------------
DSrv_Storage_2_buffers::~DSrv_Storage_2_buffers()
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
DSrv_Storage_2_buffers::DSrv_Storage_2_buffers(DSrv_Storage_2_buffers && obj)
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		std::lock_guard<std::mutex> lock_obj(obj.m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
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
int32_t DSrv_Storage_2_buffers::setData(Data_set data, const bool add) noexcept
{
	// Check the incoming parameter
	if (nullptr == data.first)
	{
		PRINT_ERR("nullptr == data.first");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR("Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Check the size of the input data
	if (   (true == add && m_fillingIndex + data.second > MAX_DATA_SIZE)
	    || (false == add && data.second > MAX_DATA_SIZE))
	{
		PRINT_ERR("Size of the data is too much "
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
int32_t DSrv_Storage_2_buffers::getData(Data_get data) noexcept
{
	// Check the incoming parameters
	if (nullptr == data.first)
	{
		PRINT_ERR("nullptr == data.first");
		return -1;
	}
	if (nullptr == data.second)
	{
		PRINT_ERR("nullptr == data.second");
		return -1;
	}

	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR("Memory for data have not been allocated");
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
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
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
int32_t DSrv_Storage_2_buffers::clearData() noexcept
{
	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
		return -1;
	}

	// Set the fillingIndex and the completeSize to 0
	m_fillingIndex = 0;
	m_completeSize = 0;

	PRINT_DBG(m_debug, "Data have cleared");

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t DSrv_Storage_2_buffers::completeData() noexcept
{
	// Check the data pointers
	if (nullptr == m_completeData || nullptr == m_fillingData)
	{
		PRINT_ERR("Memory for data have not been allocated");
		return -1;
	}

	// Lock a mutex
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
	}
	catch (std::system_error & obj)
	{
		PRINT_ERR("Exception from mutex.lock() has been occured: %s", obj.what());
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

//-------------------------------------------------------------------------------------------------
uint32_t DSrv_Storage_2_buffers::completeDataCRC()
{
	m_crc.process_bytes(m_completeData, m_completeSize);
	return m_crc.checksum();
}
