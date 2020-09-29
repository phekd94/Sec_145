
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class for UAV contour search
TODO: 
 * test class (logic() method)
FIXME:
 * drawFilterRectangles(): boundaries works incorrect
DANGER:
NOTE:

Sec_145::UavContourSearch
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |    YES(*)  |
+---------------+------------+
(*) - set with suitable parameters is not valid after clearContours() is called
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <vector>   // std::vector
#include <cstdlib>  // std::abs()

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class for UAV contour search
template <typename ContourSearchClass> class UavContourSearch : public ContourSearchClass
{
public:

	// Sets a debug enable flag
	using ContourSearchClass::setDebug;

	// Sets a metric for disjoint set
	using ContourSearchClass::setMetric;

	// Object parameters
	struct ObjParameters
	{
		ObjParameters(uint32_t x = 0, uint32_t y = 0,
		              uint32_t w = 0, uint32_t h = 0) : m_x(x), m_y(y), m_w(w), m_h(h)
		{
		}

		uint32_t m_x;
		uint32_t m_y;
		uint32_t m_w;
		uint32_t m_h;
	};

	UavContourSearch() : m_wihtout_recognize_cnt(0),
	                     m_wihtout_recognize_num(0),
	                     m_max_set_size(35), m_min_set_size(5),
	                     m_max_delta_x(350), m_min_delta_x(100),
	                     m_max_delta_y(300), m_min_delta_y(50),
	                     m_delta_old_object(50),
	                     m_flag_set_size(false),
	                     m_flag_size(false),
	                     m_flag_old_object(false),
	                     m_flag_recognized(false),
	                     m_is_working(false)
	{
		PRINT_DBG(UavContourSearch<ContourSearchClass>::m_debug, PREF, "");
	}

	// Processes all contours
	// (exception from push_back() can be thrown (it is unlikely))
	void processContours(const uint8_t* const greyData,
	                     const uint32_t width, const uint32_t height);

	// Clears contours
	void clearContours() noexcept
	{
		m_suitable_objects_params.clear();
		ContourSearchClass::clearDisjointSet();
	}

	// Draws a rectangles for filter
	void drawFilterRectangles(uint8_t* const data,
	                          const uint32_t width, const uint32_t height) const noexcept;

	// ***********************
	// ******* Getters *******
	// ***********************

	// Gets UAV parameters: left top coordinates of rectangle, width and height
	// (contour with maximum size of set (which passed all filters))
	int32_t getUavParameters(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const noexcept;

	// Gets UAV parameters: left top coordinates of rectangle, width and height
	// (contour with maximum size of set (which passed all filters))
	int32_t getUavParameters(ObjParameters& obj_params) const noexcept;

	// Gets UAV parameters: center of the rectangle with found object
	// (contour with maximum size of set (which passed all filters))
	int32_t getUavParameters(uint32_t& x, uint32_t& y) const noexcept;

	// Gets set with suitable objects parameters
	const std::vector<ObjParameters>& getSuitableObjParameters() const noexcept
	{
		return m_suitable_objects_params;
	}

	// Gets is_working flag
	bool getIsWorking() const noexcept
	{
		return m_is_working;
	}

	// ***********************
	// ******* Setters *******
	// ***********************

	// m_wihtout_recognize_num
	void setWihtoutRecognizeNum(const uint32_t wihtout_recognize_num) noexcept
	{ 
		m_wihtout_recognize_num = wihtout_recognize_num; 
	}

	// m_max_set_size, m_min_set_size
	void setMaxMinSizeOfSet(const uint32_t max_set_size, const uint32_t min_set_size) noexcept
	{ 
		m_max_set_size = max_set_size; 
		m_min_set_size = min_set_size; 
	}

	// m_max_delta_x, m_min_delta_x, m_max_delta_y, m_min_delta_y
	void setDeltaXY(const uint32_t max_delta_x, const uint32_t min_delta_x,
	                const uint32_t max_delta_y, const uint32_t min_delta_y) noexcept
	{ 
		m_max_delta_x = max_delta_x; 
		m_min_delta_x = min_delta_x; 
		m_max_delta_y = max_delta_y; 
		m_min_delta_y = min_delta_y; 
	}

	// m_delta_old_object
	void setDeltaOld(const uint32_t delta_old_object) noexcept
	{ 
		m_delta_old_object = delta_old_object;
	}

	// m_flag_set_size
	void setFlagSetSize(const bool flag) noexcept
	{ 
		m_flag_set_size = flag;
	}

	// m_flag_size
	void setFlagSize(const bool flag) noexcept
	{ 
		m_flag_size = flag; 
	}

	// m_flag_old_object
	void setFlagOldObject(const bool flag) noexcept
	{ 
		m_flag_old_object = flag; 
	}

private:

	// Preface in debug message
	static const char* const PREF;

	// Struct with coordinates and some parameters of member
	struct MinMax_XY
	{
		// Constants
		static const uint32_t DEF_MAX_X {1024};
		static const uint32_t DEF_MAX_Y {768};
		static const uint32_t DEF_MIN_X {0};
		static const uint32_t DEF_MIN_Y {0};

		MinMax_XY() : m_i(0),
		              m_max_x(DEF_MIN_X), m_max_y(DEF_MIN_Y),
		              m_min_x(DEF_MAX_X), m_min_y(DEF_MAX_Y),
		              m_delta_x(0), m_delta_y(0),
		              m_set_size(0),
		              m_valid(false)
		{
		}

		// Prints members of the struct
		void print() const noexcept
		{
			PRINT_DBG(true, PREF,
			          "i = %lu, delta x = %lu, delta y = %lu, set_size = %lu,  "
			          "max(%lu, %lu), min(%lu, %lu)",
			          static_cast<unsigned long>(m_i),
			          static_cast<unsigned long>(m_delta_x),
			          static_cast<unsigned long>(m_delta_y),
			          static_cast<unsigned long>(m_set_size),
			          static_cast<unsigned long>(m_max_x), static_cast<unsigned long>(m_max_y),
			          static_cast<unsigned long>(m_min_x), static_cast<unsigned long>(m_min_y));
		}

		// Index
		uint32_t m_i;

		// Maximum x and y
		uint32_t m_max_x;
		uint32_t m_max_y;

		// Minimum x and y
		uint32_t m_min_x;
		uint32_t m_min_y;

		// Delta x and y
		uint32_t m_delta_x;
		uint32_t m_delta_y;

		// Size of set
		uint32_t m_set_size;

		// Valid of the struct
		bool m_valid;
	};

	// Old recognized object
	MinMax_XY m_old_object;

	// Set with suitable objects parameters
	std::vector<ObjParameters> m_suitable_objects_params;

	// For old object
	uint32_t m_wihtout_recognize_cnt;
	uint32_t m_wihtout_recognize_num;

	// Maximum and minimum size of set
	uint32_t m_max_set_size;
	uint32_t m_min_set_size;

	// Maximum and minimum delta x and y
	uint32_t m_max_delta_x;
	uint32_t m_min_delta_x;
	uint32_t m_max_delta_y;
	uint32_t m_min_delta_y;

	// Maximum difference with old object
	uint32_t m_delta_old_object;

	// Enable flags for recognition filters
	bool m_flag_set_size;
	bool m_flag_size;
	bool m_flag_old_object;

	// Flag; UAV was recognized
	bool m_flag_recognized;

	// Flag; applyDetector() method is working
	volatile bool m_is_working;
};

//=================================================================================================
// *****************************************************
// ******* UavContourSearch class implementation *******
// *****************************************************

//-------------------------------------------------------------------------------------------------
template <typename ContourSearchClass>
const char* const UavContourSearch<ContourSearchClass>::PREF = "[UavContourSearch]: ";

//-------------------------------------------------------------------------------------------------
template <typename ContourSearchClass>
void UavContourSearch<ContourSearchClass>::processContours(const uint8_t* const greyData,
                                                           const uint32_t width,
                                                           const uint32_t height)
{
	// Set is_working flag
	m_is_working = true;

	// Apply contours detector
	ContourSearchClass::applyDetector(greyData, width, height);

	// Get a disjoint set
	decltype ( ContourSearchClass::getDisjointSet() ) m_d_set =
	        ContourSearchClass::getDisjointSet();

	// Create a vector with minimum and maximum values of coordinates
	// (size of this vector is equal to size of disjoint set)
	std::vector<MinMax_XY> mm_xy(m_d_set.size());

	// Sets in disjoint set
	for (uint32_t i = 0; i < m_d_set.size(); ++i)
	{
		// Set index
		mm_xy[i].m_i = i;

		// Check the size of set
		uint32_t set_size = static_cast<uint32_t>(m_d_set[i].size());
		if (0 == set_size)
		{
			mm_xy[i].m_valid = false;
			continue;
		}
		mm_xy[i].m_valid = true;
		mm_xy[i].m_set_size = set_size;

		// Members in set
		for (uint32_t j = 0; j < m_d_set[i].size(); ++j)
		{
			m_d_set[i][j].compareMaxMinXY(mm_xy[i].m_max_x, mm_xy[i].m_min_x,
			                              mm_xy[i].m_max_y, mm_xy[i].m_min_y);
		}

		// Calculate delta x and y
		mm_xy[i].m_delta_x = mm_xy[i].m_max_x - mm_xy[i].m_min_x;
		mm_xy[i].m_delta_y = mm_xy[i].m_max_y - mm_xy[i].m_min_y;
	}

	// Print a content of the vector member with maximum, minimum values and other parameters
	if (true == ContourSearchClass::m_debug)
	{
		for (const auto& el : mm_xy)
		{
			if (false == el.m_valid)
				continue;
			el.print();
		}
		PRINT_DBG(true, PREF, "-----------------");
	}

	// Index for return value; maximal size of suitable set
	uint32_t index {0}, max_set_size {0};

	// Process all parameters in vector
	for (uint32_t i = 0; i < mm_xy.size(); ++i)
	{
		// Valid
		if (false == mm_xy[i].m_valid)
			continue;

		// Filter a member by size in set
		if (true == m_flag_set_size)
		{
			if (mm_xy[i].m_set_size < m_min_set_size || mm_xy[i].m_set_size > m_max_set_size)
			{
				PRINT_DBG(ContourSearchClass::m_debug, PREF,
				          "stop: filter by set size, index = %lu",
				          static_cast<unsigned long>(mm_xy[i].m_i));
				continue;
			}
		}

		// Filter a member by size
		if (true == m_flag_size)
		{
			if (!(   mm_xy[i].m_delta_x > m_min_delta_x
			      && mm_xy[i].m_delta_x < m_max_delta_x
			      && mm_xy[i].m_delta_y > m_min_delta_y
			      && mm_xy[i].m_delta_y < m_max_delta_y))
			{
				PRINT_DBG(ContourSearchClass::m_debug, PREF,
				          "stop: filter by size, index = %lu",
				          static_cast<unsigned long>(mm_xy[i].m_i));
				continue;
			}
		}

		// Filter a member by old object position
		if (true == m_flag_old_object)
		{
			if (true == m_old_object.m_valid)
			{
				if (   std::abs(static_cast<int>(m_old_object.m_max_x) -
				                static_cast<int>(mm_xy[i].m_max_x)) >
				       static_cast<int>(m_delta_old_object)
				    && std::abs(static_cast<int>(m_old_object.m_max_y) -
				                static_cast<int>(mm_xy[i].m_max_y)) >
				       static_cast<int>(m_delta_old_object)
				    && std::abs(static_cast<int>(m_old_object.m_min_x) -
				                static_cast<int>(mm_xy[i].m_min_x)) >
				       static_cast<int>(m_delta_old_object)
				    && std::abs(static_cast<int>(m_old_object.m_min_y) -
				                static_cast<int>(mm_xy[i].m_min_y)) >
				       static_cast<int>(m_delta_old_object)
				   )
				{
					PRINT_DBG(ContourSearchClass::m_debug, PREF,
					          "stop: filter by old object, index = %lu",
					          static_cast<unsigned long>(mm_xy[i].m_i));
					continue;
				}
			}
		}

		PRINT_DBG(ContourSearchClass::m_debug, PREF, "detect, index = %lu",
		          static_cast<unsigned long>(mm_xy[i].m_i));

		// Add suitable object parameters
		m_suitable_objects_params.push_back(ObjParameters(mm_xy[i].m_min_x, mm_xy[i].m_min_y,
		                                                  mm_xy[i].m_delta_x,
		                                                  mm_xy[i].m_delta_y));

		// Choice a set with maximum size
		if (mm_xy[i].m_set_size > max_set_size)
		{
			max_set_size = mm_xy[i].m_set_size;
			index = i;
		}
	}

	// Verify recognition
	if (0 == max_set_size)
	{
		// Check without recognize counter and number
		if (++m_wihtout_recognize_cnt > m_wihtout_recognize_num)
		{
			// Set recognized flag
			m_flag_recognized = false;

			// Set without recognize counter to 0
			m_wihtout_recognize_cnt = 0;

			// Set empty object as old object
			m_old_object = MinMax_XY();

			PRINT_DBG(ContourSearchClass::m_debug, PREF, "m_wihtout_recognize_cnt > %lu",
					  static_cast<unsigned long>(m_wihtout_recognize_num));
		}

		PRINT_DBG(ContourSearchClass::m_debug, PREF, "Copter was not recognized");
	}
	else
	{
		// Set recognized flag
		m_flag_recognized = true;

		// Set without recognize counter to 0
		m_wihtout_recognize_cnt = 0;

		// Set old object
		m_old_object = mm_xy[index];

		if (true == ContourSearchClass::m_debug)
			mm_xy[index].print();
		PRINT_DBG(ContourSearchClass::m_debug, PREF, "is an old object now");
	}

	PRINT_DBG(ContourSearchClass::m_debug, PREF, "=================");

	// Clear is_working flag
	m_is_working = false;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void UavContourSearch<T>::drawFilterRectangles(uint8_t* const data,
                                               const uint32_t width,
                                               const uint32_t height) const noexcept
{
	const uint32_t OFFSET = 20;

	// Check the size of image
	if (   OFFSET + m_max_delta_y + 2 * m_delta_old_object > height
		|| OFFSET + m_max_delta_x + 2 * m_delta_old_object > width)
	{
		PRINT_ERR(true, PREF, "Out of bounds");
		return;
	}

	// Rectangle for old object
	// Lines
	for (uint32_t i = 0; i < m_max_delta_x + 2 * m_delta_old_object; i += 4)
	{
		data[(i + OFFSET) + width * OFFSET] = 122;
		data[(i + OFFSET) + width * (m_max_delta_y + OFFSET + 2 * m_delta_old_object)] = 122;
	}
	// Columns
	for (uint32_t i = 0; i < m_max_delta_y + 2 * m_delta_old_object; i += 4)
	{
		data[OFFSET + width * (i + OFFSET)] = 122;
		data[OFFSET + m_max_delta_x + width * (i + OFFSET) + 2 * m_delta_old_object] = 122;
	}

	// Rectangle for object (max)
	// Lines
	for (uint32_t i = 0; i < m_max_delta_x; ++i)
	{
		data[(i + OFFSET + m_delta_old_object) + width * (OFFSET + m_delta_old_object)] = 170;
		data[(i + OFFSET + m_delta_old_object) + width * (m_max_delta_y + OFFSET +
														  m_delta_old_object)] = 170;
	}
	// Columns
	for (uint32_t i = 0; i < m_max_delta_y; ++i)
	{
		data[(OFFSET + m_delta_old_object) + width * (i + OFFSET + m_delta_old_object)] = 170;
		data[(OFFSET + m_delta_old_object) + m_max_delta_x + width * (i + OFFSET +
																	  m_delta_old_object)] = 170;
	}

	// Rectangle for object (min)
	// Lines
	for (uint32_t i = 0; i < m_min_delta_x; ++i)
	{
		data[(i + OFFSET + m_delta_old_object) + width * (OFFSET + m_delta_old_object)] = 170;
		data[(i + OFFSET + m_delta_old_object) + width * (m_min_delta_y + OFFSET +
														  m_delta_old_object)] = 170;
	}
	// Columns
	for (uint32_t i = 0; i < m_min_delta_y; ++i)
	{
		data[(OFFSET + m_delta_old_object) + width * (i + OFFSET + m_delta_old_object)] = 170;
		data[(OFFSET + m_delta_old_object) + m_min_delta_x + width * (i + OFFSET +
																	  m_delta_old_object)] = 170;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t UavContourSearch<T>::getUavParameters(uint32_t& x, uint32_t& y,
                                              uint32_t& w, uint32_t& h) const noexcept
{
	// Check a valid of recognized object
	if (false == m_flag_recognized)
	{
		x = y = w = h = 0;
		return -1;
	}

	// Get parameters
	x = m_old_object.m_min_x;
	y = m_old_object.m_min_y;
	w = m_old_object.m_delta_x;
	h = m_old_object.m_delta_y;

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t UavContourSearch<T>::getUavParameters(ObjParameters& obj_params) const noexcept
{
	// Check a valid of recognized object
	if (false == m_flag_recognized)
	{
		obj_params.m_x = obj_params.m_y = obj_params.m_w = obj_params.m_h = 0;
		return -1;
	}

	// Get parameters
	obj_params.m_x = m_old_object.m_min_x;
	obj_params.m_y = m_old_object.m_min_y;
	obj_params.m_w = m_old_object.m_delta_x;
	obj_params.m_h = m_old_object.m_delta_y;

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t UavContourSearch<T>::getUavParameters(uint32_t& x, uint32_t& y) const noexcept
{
	// Check a valid of recognized object
	if (false == m_flag_recognized)
	{
		x = y = 0;
		return -1;
	}

	// Get parameters
	x = m_old_object.m_min_x + m_old_object.m_delta_x / 2;
	y = m_old_object.m_min_y + m_old_object.m_delta_y / 2;

	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
