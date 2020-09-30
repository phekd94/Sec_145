
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class for UAV contour search with executions in another thread possibility
TODO:
 * test class (logic() method)
 * skip: uint64_t + exception over
 * whole: uint64_t + exception over
FIXME:
DANGER:
NOTE:

Sec_145::UavContourSearchThread
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
(*) - set with suitable parameters is not valid after clearContours() is called
*/

//-------------------------------------------------------------------------------------------------
#include "UavContourSearch.h"  // UavContourSearch<> (for inheritance)
#include <mutex>               // std::mutex
#include <QImage>              // QImage class
#include <system_error>        // std::system_error
#include <exception>           // std::exception

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class for UAV contour search with executions in another thread possibility
template <typename ContourSearchClass> class UavContourSearchThread :
        public UavContourSearch<ContourSearchClass>
{
public:

	// Processes image
	void process() noexcept;

	// Sets new data (image)
	bool setNewData(QImage image) noexcept;

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[UavContourSearchThread]: "};

	// Image for process
	QImage m_image;

	// Mutex
	std::mutex m_mutex;

	// Flag; new data have been added
	volatile bool m_new_data {false};

	// Flag; process() method is working
	volatile bool m_is_working {false};
};

//=================================================================================================
// ***********************************************************
// ******* UavContourSearchThread class implementation *******
// ***********************************************************

//-------------------------------------------------------------------------------------------------
template <typename ContourSearchClass>
void UavContourSearchThread<ContourSearchClass>::process() noexcept
{
	try
	{

	// Lock a mutex
	m_mutex.lock();

	// Check new data flag
	if (false == m_new_data)
	{
		// Unlock a mutex
		m_mutex.unlock();

		return;
	}

	// Set working flag
	m_is_working = true;

	// Unlock a mutex
	m_mutex.unlock();

	// Clear old contours
	UavContourSearch<ContourSearchClass>::clearContours();

	// Process an image
	UavContourSearch<ContourSearchClass>::processContours(m_image.bits(),
	                                                      m_image.width(), m_image.height());

	// Lock a mutex
	m_mutex.lock();

	// Set new data flag
	m_new_data = false;

	// Set working flag
	m_is_working = false;

	// Unlock a mutex
	m_mutex.unlock();

	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, PREF, "Exception from mutex has been occured: %s", obj.what());
		return;
	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, PREF, "Exception from processContours() has been occured: %s",
		          obj.what());

		// Unlock a mutex
		m_mutex.unlock();

		return;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename ContourSearchClass>
bool UavContourSearchThread<ContourSearchClass>::setNewData(QImage image) noexcept
{
	try
	{

	// Return value
	bool ret;

	// Lock a mutex
	m_mutex.lock();

	// Add new data
	if (false == m_is_working)
	{
		// Set new image
		m_image = image;

		// Set new data flag
		m_new_data = true;

		// Set return value; new data have been accepted
		ret = true;
	}
	else
	{
		// Set return value; new data have not been accepted
		ret = false;
	}

	// Unlock a mutex
	m_mutex.unlock();

	return ret;

	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, PREF, "Exception from mutex has been occured: %s", obj.what());
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
