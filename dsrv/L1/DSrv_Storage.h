
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for storing data
TODO:
 * checksum (with sensitive to the order of the blocks (bytes) in the data word (message))
 * mutex test
 * setData(): add parameter
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Storage class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |   YES(**)  |
+---------------+------------+
(*)  - NO for the setDebug()
(**) - take into account that only two buffers are available
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <mutex>    // std::mutex, std::lock_guard
#include <utility>  // std::pair

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class for storing data
class DSrv_Storage
{
	friend class DSrv_Storage_test;

public:

	// Data type for set method (pointer + size)
	using Data_set = const std::pair<const uint8_t * const, const uint32_t>;

	// Data type for get method (pointer to pointer + pointer to size)
	using Data_get = const std::pair<uint8_t ** const, uint32_t * const>;

protected:

	DSrv_Storage();
	virtual ~DSrv_Storage();

	// Without copy constructor and override an assignment operator
	// (due to class members as pointer are presented)
	DSrv_Storage(DSrv_Storage &) = delete;
	DSrv_Storage & operator=(const DSrv_Storage &) = delete;

	// Move constructor
	DSrv_Storage(DSrv_Storage && obj);

	// Sets data in the storage
	int32_t setData(Data_set data, const bool add) noexcept;

	// Gets complete data from the storage
	// (probably the method will be called from another thread)
	int32_t getData(Data_get data) noexcept;

	// Deletes all data in the storage
	int32_t clearData() noexcept;

	// Exchanges the m_fillingData and the m_completeData pointers (filling data is complete)
	int32_t completeData() noexcept;

	// Enable/disable debug messages
	// (probably the method will be called from another thread)
	void setDebug(const bool d) noexcept
	{
		m_debug = d;
	}

	// Gets a number of the nodes
	uint32_t getNodeNum() const noexcept
	{
		return 2;
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

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug {true};
};

//=================================================================================================
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
