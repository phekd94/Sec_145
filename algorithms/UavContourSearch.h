
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class for UAV contour search
TODO: 
 * test class (logic() method)
 * Members in set part in getIndexOfUavSet() method
FIXME:
 * drawFilterRectangles(): boundaries works incorrect
DANGER:
NOTE:

Sec_145::UavContourSearch
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES(*)   |    YES     |
+---------------+------------+
(*) - single parameters for all threads
*/

//-------------------------------------------------------------------------------------------------
#include "DisjointSet.h"  // DisjointSet template class (for inheritance)
#include <cstdint>        // integer types
#include <vector>         // std::vector

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class for UAV contour search
template <typename T>
class UavContourSearch : public DisjointSet<T>
{
public:

	UavContourSearch();

	// Get index of set of set contains UAV contour
	virtual int32_t getIndexOfUavSet();

	// Draws a rectangles for filter
	void drawFilterRectangles(uint8_t* const data,
	                          const uint32_t width, 
							  const uint32_t height) const;

	// ***** Getters *****

	// Gets UAV parameters: left top coordinates of rectangle, width and height
	int32_t getUavParameters(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const;

	// Gets UAV parameters: set with UAV contour
	const std::vector<T>& getUavParameters() const;

	// *******************

	// ***** Setters *****

	// m_wihtout_recognize_num
	void setWihtoutRecognizeNum(const uint32_t wihtout_recognize_num)
	{ 
		m_wihtout_recognize_num = wihtout_recognize_num; 
	}

	// m_max_set_size, m_min_set_size
	void setMaxMinSizeOfSet(const uint32_t max_set_size, const uint32_t min_set_size)
	{ 
		m_max_set_size = max_set_size; 
		m_min_set_size = min_set_size; 
	}

	// m_max_delta_x, m_min_delta_x, m_max_delta_y, m_min_delta_y
	void setDeltaXY(const uint32_t max_delta_x, const uint32_t min_delta_x,
	                const uint32_t max_delta_y, const uint32_t min_delta_y)
	{ 
		m_max_delta_x = max_delta_x; 
		m_min_delta_x = min_delta_x; 
		m_max_delta_y = max_delta_y; 
		m_min_delta_y = min_delta_y; 
	}

	// m_delta_old_object
	void setDeltaOld(const int32_t delta_old_object)
	{ 
		m_delta_old_object = (delta_old_object < 0 ? 0 : delta_old_object); 
	}

	// m_flag_set_size
	void setFlagSetSize(const bool flag)
	{ 
		m_flag_set_size = flag;
	}

	// m_flag_size
	void setFlagSize(const bool flag)
	{ 
		m_flag_size = flag; 
	}

	// m_flag_old_object
	void setFlagOldObject(const bool flag)
	{ 
		m_flag_old_object = flag; 
	}

	// *******************

private:

	// Preface in debug message
	static const char* const PREF;

	// Struct with coordinates and some parameters of member
	struct MinMax_XY {

		// Constants
		uint32_t DEF_MAX_X = 1024;
		uint32_t DEF_MAX_Y = 768;
		uint32_t DEF_MIN_X = 0;
		uint32_t DEF_MIN_Y = 0;

		MinMax_XY() : i(0),
		              max_x(DEF_MIN_X),
		              max_y(DEF_MIN_Y),
		              min_x(DEF_MAX_X),
		              min_y(DEF_MAX_Y),
		              delta_x(0),
		              delta_y(0),
		              set_size(0),
		              valid(false)
		{
		}

		// Prints members of the struct
		void print() const
		{
			PRINT_DBG(true, PREF,
			          "i = %lu, delta x = %lu, delta y = %lu, set_size = %lu,  "
			          "max(%lu, %lu), min(%lu, %lu)",
			          static_cast<unsigned long>(i),
			          static_cast<unsigned long>(delta_x),
			          static_cast<unsigned long>(delta_y),
			          static_cast<unsigned long>(set_size),
			          static_cast<unsigned long>(max_x),
			          static_cast<unsigned long>(max_y),
			          static_cast<unsigned long>(min_x),
			          static_cast<unsigned long>(min_y));
		}

		// Index
		uint32_t i;

		// Max x and y
		uint32_t max_x;
		uint32_t max_y;

		// Min x and y
		uint32_t min_x;
		uint32_t min_y;

		// Delta x and y
		uint32_t delta_x;
		uint32_t delta_y;
		uint32_t set_size;

		// Valid of the struct
		bool valid;
	};

	// Old recognized object
	MinMax_XY m_old_object;

	// For old object
	uint32_t m_wihtout_recognize;
	uint32_t m_wihtout_recognize_num;

	// Max and min size of set
	uint32_t m_max_set_size;
	uint32_t m_min_set_size;

	// Max and min delta x and y
	uint32_t m_max_delta_x;
	uint32_t m_min_delta_x;
	uint32_t m_max_delta_y;
	uint32_t m_min_delta_y;

	// Maximum difference with old object
	int32_t m_delta_old_object;

	// Flags enable/disable of recognitions
	bool m_flag_set_size;
	bool m_flag_size;
	bool m_flag_old_object;

	// Flag; copter was recognized
	bool m_flag_recognized;

	// Index of set with recognized copter
	std::vector<T> m_copterSet;
};

//=================================================================================================
// UavContourSearch class implementation

//-------------------------------------------------------------------------------------------------
template <typename T>
const char* const UavContourSearch<T>::PREF = "[UavContourSearch]: ";

//-------------------------------------------------------------------------------------------------
template <typename T>
UavContourSearch<T>::UavContourSearch() : m_wihtout_recognize(0), 
                                          m_wihtout_recognize_num(1),
                                          m_max_set_size(35), m_min_set_size(5),
                                          m_max_delta_x(350), m_min_delta_x(100),
                                          m_max_delta_y(300), m_min_delta_y(50),
                                          m_delta_old_object(50),
                                          m_flag_set_size(false),
                                          m_flag_size(false),
                                          m_flag_old_object(false),
                                          m_flag_recognized(false),
                                          m_copterSet()
{
	PRINT_DBG(UavContourSearch<T>::m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
template <typename T>
int32_t UavContourSearch<T>::getIndexOfUavSet()
{
	// ...
	const std::vector<std::vector<T>> m_d_set = DisjointSet<T>::getDisjointSet();

	// Create a vector with minimum and maximum values of coordinates
	// (size of this vector is equal to size of disjoint set)
	std::vector<MinMax_XY> mm_xy(m_d_set.size());

	// Sets in disjoint set
	for (uint32_t i = 0; i < m_d_set.size(); ++i)
	{
		// Set index
		mm_xy[i].i = i;

		// Check the size of set
		uint32_t set_size = static_cast<uint32_t>(m_d_set[i].size());
		if (0 == set_size)
		{
			mm_xy[i].valid = false;
			continue;
		}
		mm_xy[i].valid = true;
		mm_xy[i].set_size = set_size;

		// Members in set
		for (uint32_t j = 0; j < m_d_set[i].size(); ++j)
		{
			// x
			//  max
			if (m_d_set[i][j].x > mm_xy[i].max_x)
				mm_xy[i].max_x = m_d_set[i][j].x;
			//  min
			if (m_d_set[i][j].x < mm_xy[i].min_x)
				mm_xy[i].min_x = m_d_set[i][j].x;

			// y
			//  max
			if (m_d_set[i][j].y > mm_xy[i].max_y)
				mm_xy[i].max_y = m_d_set[i][j].y;
			//  min
			if (m_d_set[i][j].y < mm_xy[i].min_y)
				mm_xy[i].min_y = m_d_set[i][j].y;

			/*
			// Points in member
			for (uint32_t k = 0; k < m_d_set[i][j].points.size(); ++k)
			{
				// x
				// max
				if (m_d_set[i][j].points[k].x > mm_xy[i].max_x)
					mm_xy[i].max_x = m_d_set[i][j].points[k].x;
				// min
				if (m_d_set[i][j].points[k].x < mm_xy[i].min_x)
					mm_xy[i].min_x = m_d_set[i][j].points[k].x;

				// y
				// max
				if (m_d_set[i][j].points[k].y > mm_xy[i].max_y)
					mm_xy[i].max_y = m_d_set[i][j].points[k].y;
				// min
				if (m_d_set[i][j].points[k].y < mm_xy[i].min_y)
					mm_xy[i].min_y = m_d_set[i][j].points[k].y;
			}
			*/
		}

		// Calculate delta x and y
		mm_xy[i].delta_x = mm_xy[i].max_x - mm_xy[i].min_x;
		mm_xy[i].delta_y = mm_xy[i].max_y - mm_xy[i].min_y;
	}

	// Print a content of the vector member with maximum, minimum values
	// and other parameters
	if (true == DisjointSet<T>::m_debug)
	{
		for (const auto& el : mm_xy)
		{
			if (false == el.valid)
				continue;
			el.print();
		}
		PRINT_DBG(true, PREF, "-----------------");
	}

	// Index for return value; maximal size of suitable set
	int32_t index = 0;
	uint32_t max_set_size = 0;

	// Process all parameters in vector
	for (uint32_t i = 0; i < mm_xy.size(); ++i)
	{
		// Valid
		if (false == mm_xy[i].valid)
			continue;

		// Filter a member by size in set
		if (true == m_flag_set_size)
		{
			if (mm_xy[i].set_size < m_min_set_size || mm_xy[i].set_size > m_max_set_size)
				continue;
		}

		// Filter a member by size
		if (true == m_flag_size)
		{
			if (!(mm_xy[i].delta_x > m_min_delta_x
				  && mm_xy[i].delta_x < m_max_delta_x
				  && mm_xy[i].delta_y > m_min_delta_y
				  && mm_xy[i].delta_y < m_max_delta_y))
			{
				PRINT_DBG(DisjointSet<T>::m_debug, PREF, "stop: filter by size, index = %lu",
						  static_cast<unsigned long>(mm_xy[i].i));
				continue;
			}
		}

		// Filter a member by old object position
		if (true == m_flag_old_object)
		{
			if (true == m_old_object.valid)
			{
				if (std::abs(static_cast<int>(m_old_object.max_x) -
							 static_cast<int>(mm_xy[i].max_x)) > m_delta_old_object
					&& std::abs(static_cast<int>(m_old_object.max_y) -
								static_cast<int>(mm_xy[i].max_y)) > m_delta_old_object
					&& std::abs(static_cast<int>(m_old_object.min_x) -
								static_cast<int>(mm_xy[i].min_x)) > m_delta_old_object
					&& std::abs(static_cast<int>(m_old_object.min_y) -
								static_cast<int>(mm_xy[i].min_y)) > m_delta_old_object)
				{
					PRINT_DBG(DisjointSet<T>::m_debug, PREF, "stop: filter by old obj, index = %lu",
							  static_cast<unsigned long>(mm_xy[i].i));
					continue;
				}
			}
		}

		PRINT_DBG(DisjointSet<T>::m_debug, PREF, "detect, index = %lu",
				  static_cast<unsigned long>(mm_xy[i].i));

		// Choice a set with maximum size
		if (mm_xy[i].set_size > max_set_size)
		{
			max_set_size = mm_xy[i].set_size;
			index = static_cast<int32_t>(i);
		}
	}

	// Verify recognition
	if (0 == max_set_size)
	{
		m_flag_recognized = false;
		m_copterSet.resize(0);
		if (++m_wihtout_recognize > m_wihtout_recognize_num)
		{
			PRINT_DBG(DisjointSet<T>::m_debug, PREF, "m_wihtout_recognize > %lu",
					  static_cast<unsigned long>(m_wihtout_recognize_num));
			m_wihtout_recognize = 0;
			m_old_object = MinMax_XY();
		}
		PRINT_DBG(DisjointSet<T>::m_debug, PREF, "Copter was not recognized");
		index = -1;
	}
	else
	{
		m_flag_recognized = true;
		m_copterSet = m_d_set[static_cast<uint32_t>(index)];
		if (true == DisjointSet<T>::m_debug)
			mm_xy[static_cast<uint32_t>(index)].print();
		PRINT_DBG(DisjointSet<T>::m_debug, PREF, "is an old object now");
		m_old_object = mm_xy[static_cast<uint32_t>(index)];
	}

	PRINT_DBG(DisjointSet<T>::m_debug, PREF, "=================");

	return index;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
void UavContourSearch<T>::drawFilterRectangles(uint8_t* const data,
                                               const uint32_t width,
                                               const uint32_t height) const
{
	const uint32_t OFFSET = 20;

	// Check the size of image
	if (   OFFSET + m_max_delta_y + 2 * m_delta_old_object > height
		|| OFFSET + m_max_delta_x + 2 * m_delta_old_object > width)
	{
		PRINT_ERR(true, PREF, "");
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
                                              uint32_t& w, uint32_t& h) const
{
	// Check a valid of recognized object
	if (false == m_flag_recognized)
		return -1;

	// Get parameters
	x = m_old_object.min_x;
	y = m_old_object.min_y;
	w = m_old_object.delta_x;
	h = m_old_object.delta_y;

	return 0;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
const std::vector<T>& UavContourSearch<T>::getUavParameters() const
{
	// Empty set
	static const std::vector<T> empty_set;

	// Check a valid of recognized object
	if (false == m_flag_recognized || m_copterSet.size() == 0)
		return empty_set;

	// Get parameters
	return m_copterSet;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
