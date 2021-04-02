
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for storing data
TODO:
 * mutex test
 * setData(): add parameter
 * smart pointers for data pointers
 * Data_get -> Data_get without pointer to pointer
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Storage_2_buffers class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |   YES(**)  |
+---------------+------------+
(*)  - NO for the setDebug()
(**) - take into account that only two buffers are available
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>        // integer types
#include <mutex>          // std::mutex, std::lock_guard
#include <utility>        // std::pair
#include <streambuf>      // std::basic_streambuf
#include <istream>        // std::basic_istream
#include "boost/crc.hpp"  // Boost.CRC library

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for storing data
class DSrv_Storage_2_buffers
{
	friend class DSrv_Storage_2_buffers_test;

public:

	// Buffer class
	class Streambuf : public std::streambuf
	{
	public:
		void setPointers(char * begin, char * curr, char * end)
		{ this->setg(begin, curr, end); }
	} m_streambuf;
	
	// Input stream
	std::istream m_istream;

	// Data type for set method (pointer + size)
	using Data_set = const std::pair<const uint8_t * const, const uint32_t>;

	// Data type for get method (pointer to pointer + pointer to size)
	using Data_get = const std::pair<uint8_t ** const, uint32_t * const>;

	DSrv_Storage_2_buffers();
	virtual ~DSrv_Storage_2_buffers();

	// Without copy constructor and override an assignment operator
	// (due to class members as pointer are presented)
	DSrv_Storage_2_buffers(const DSrv_Storage_2_buffers &) = delete;
	DSrv_Storage_2_buffers & operator=(const DSrv_Storage_2_buffers &) = delete;

	// Move constructor
	//DSrv_Storage_2_buffers(DSrv_Storage_2_buffers && obj);

	// Sets data in the storage
	int32_t setData(Data_set data, const bool add) noexcept;

	// Gets complete data from the storage
	int32_t getData(Data_get data) noexcept;

	// Deletes all data in the storage
	int32_t clearData() noexcept;

	// Exchanges the m_fillingData and the m_completeData pointers (filling data is complete)
	int32_t completeData() noexcept;
	
	// Gets a CRC of complete data
	uint32_t completeDataCRC();

	// Enables debug messages
	void setDebug(const bool d) noexcept
	{
		m_debug = d;
	}

private:

	// Pointers to the data
	uint8_t * m_completeData {nullptr};
	uint8_t * m_fillingData {nullptr};

	// Index in the array of filling data
	uint32_t m_fillingIndex {0};

	// Size of the complete data
	uint32_t m_completeSize {0};

	// Mutex
	std::mutex m_mutex;
	
	// CRC
	boost::crc_32_type m_crc;

	// Enable debug messages
	bool m_debug {true};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
