
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for storing data
TODO: * checksum (with sensitive to the order of the blocks (bytes) in the data word (message))
	  * mutex test
FIXME:
DANGER:
NOTE:

Sec_145::DSrv_Storage class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |    YES(*)  |
+---------------+------------+
(*) - take into account that only two buffers are available
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <mutex>    // std::mutex, std::lock_guard

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
class DSrv_Storage
{
	friend class DSrv_Storage_test;

protected:

	DSrv_Storage();
	virtual ~DSrv_Storage();

	// Without copy constructor and override an assignment operator
	DSrv_Storage(DSrv_Storage&) = delete;
	DSrv_Storage& operator=(const DSrv_Storage&) = delete;

	// Sets data in the storage
	int32_t setData(const uint8_t* const data, const uint32_t size, const bool add);

	// Gets complete data from the storage
	// (probably the method will be called from another thread)
	int32_t getData(uint8_t** const data, uint32_t* const size);

	// Deletes all data in the storage
	void clearData();

	// Exchanges the m_fillingData and the m_completeData pointers (filling data is complete)
	int32_t completeData();

	// Enable/disable debug messages
	void setDebug(const bool d)
	{ m_debug = d; }

	// Gets a number of the nodes
	uint32_t getNodeNum() const
	{ return 2; }

private:

	// Preface in debug message
	static const char* PREF;

	// Pointers to the data
	uint8_t* m_completeData;
	uint8_t* m_fillingData;

	// Index in the array of filling data
	uint32_t m_fillingIndex;

	// Size of the complete data
	uint32_t m_completeSize;

	// Mutex
	std::mutex m_mutex;

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug;
};

//-------------------------------------------------------------------------------------------------
class DSrv_Storage_test
{
public:

	// Tests methods which utilize pointers
	static int32_t pNull(DSrv_Storage& obj);

	// Tests a work with data
	static int32_t data(DSrv_Storage& obj);

	// Runs all tests
	static int32_t fullTest();

private:

	// Preface in debug message
	static const char* PREF;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
