
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
#include <cstdint>    // integer types
#include <vector>     // std::vector
#include <cstdlib>    // std::abs()
#include <algorithm>  // std::sort()

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
		ObjParameters(uint32_t x, uint32_t y,
		              uint32_t w, uint32_t h,
		              uint32_t numPoints) : m_x(x), m_y(y), m_w(w), m_h(h), m_numPoints(numPoints)
		{
		}

		uint32_t m_x;
		uint32_t m_y;
		uint32_t m_w;
		uint32_t m_h;
		uint32_t m_numPoints;
	};

	UavContourSearch()
	{
		PRINT_DBG(UavContourSearch<ContourSearchClass>::m_debug, "");
	}

	// Processes all contours
	// (exception from push_back() can be thrown (it is unlikely))
	void processContours(const uint8_t* const greyData,
	                     const uint32_t width, const uint32_t height);

	// Clears contours
	void clearContours() noexcept
	{
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
	const std::vector<ObjParameters>& getSuitableObjParameters() noexcept
	{
		// Sorting by number of points in contour
		std::sort(m_suitable_objects_params.begin(), m_suitable_objects_params.end(),
		          [](const ObjParameters& obj_params_1, const ObjParameters& obj_params_2) {
			          return obj_params_1.m_numPoints > obj_params_2.m_numPoints;
		          });

		return m_suitable_objects_params;
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

	// Struct with coordinates and some parameters of member
	struct MinMax_XY
	{
		// Constants
		static const uint32_t DEF_MAX_X {1024};
		static const uint32_t DEF_MAX_Y {768};
		static const uint32_t DEF_MIN_X {0};
		static const uint32_t DEF_MIN_Y {0};

		// Prints members of the struct
		void print() const noexcept
		{
			PRINT_DBG(true,
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
		uint32_t m_i {0};

		// Maximum x and y
		uint32_t m_max_x {DEF_MIN_X};
		uint32_t m_max_y {DEF_MIN_Y};

		// Minimum x and y
		uint32_t m_min_x {DEF_MAX_X};
		uint32_t m_min_y {DEF_MAX_Y};

		// Delta x and y
		uint32_t m_delta_x {0};
		uint32_t m_delta_y {0};

		// Size of set
		uint32_t m_set_size {0};

		// Valid of the struct
		bool m_valid {false};
	};

	// Old recognized object
	MinMax_XY m_old_object;

	// Set with suitable objects parameters
	std::vector<ObjParameters> m_suitable_objects_params;

	// For old object
	uint32_t m_wihtout_recognize_cnt {0};
	uint32_t m_wihtout_recognize_num {0};

	// Maximum and minimum size of set
	uint32_t m_max_set_size {35};
	uint32_t m_min_set_size {5};

	// Maximum and minimum delta x and y
	uint32_t m_max_delta_x {350};
	uint32_t m_min_delta_x {100};
	uint32_t m_max_delta_y {350};
	uint32_t m_min_delta_y {50};

	// Maximum difference with old object
	uint32_t m_delta_old_object {50};

	// Enable flags for recognition filters
	bool m_flag_set_size    {false};
	bool m_flag_size        {false};
	bool m_flag_old_object  {false};

	// Flag; UAV was recognized
	bool m_flag_recognized {false};
};

//=================================================================================================
// *****************************************************
// ******* UavContourSearch class implementation *******
// *****************************************************

//-------------------------------------------------------------------------------------------------
template <typename ContourSearchClass>
void UavContourSearch<ContourSearchClass>::processContours(const uint8_t* const greyData,
                                                           const uint32_t width,
                                                           const uint32_t height)
{
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
		PRINT_DBG(true, "-----------------");
	}

	// Index for return value; maximal size of suitable set
	uint32_t index {0}, max_set_size {0};

	// Vector with suitable objects parameters
	std::vector<ObjParameters> suitable_objects_params;

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
				PRINT_DBG(ContourSearchClass::m_debug,
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
				PRINT_DBG(ContourSearchClass::m_debug,
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
					PRINT_DBG(ContourSearchClass::m_debug,
					          "stop: filter by old object, index = %lu",
					          static_cast<unsigned long>(mm_xy[i].m_i));
					continue;
				}
			}
		}

		PRINT_DBG(ContourSearchClass::m_debug, "detect, index = %lu",
		          static_cast<unsigned long>(mm_xy[i].m_i));

		// Add suitable object parameters
		suitable_objects_params.push_back(ObjParameters(mm_xy[i].m_min_x, mm_xy[i].m_min_y,
		                                                mm_xy[i].m_delta_x, mm_xy[i].m_delta_y,
		                                                mm_xy[i].m_set_size));

		// Choice a set with maximum size
		if (mm_xy[i].m_set_size > max_set_size)
		{
			max_set_size = mm_xy[i].m_set_size;
			index = i;
		}
	}

	// Set vector with suitable objects parameters
	m_suitable_objects_params = suitable_objects_params;

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

			PRINT_DBG(ContourSearchClass::m_debug, "m_wihtout_recognize_cnt > %lu",
					  static_cast<unsigned long>(m_wihtout_recognize_num));
		}

		PRINT_DBG(ContourSearchClass::m_debug, "Copter was not recognized");
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
		PRINT_DBG(ContourSearchClass::m_debug, "is an old object now");
	}

	PRINT_DBG(ContourSearchClass::m_debug, "=================");
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
		PRINT_ERR("Out of bounds");
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
