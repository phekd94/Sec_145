
#include "CopterSearch.h"

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
#define PREF  "[CopterSearch]: "

//-------------------------------------------------------------------------------------------------
CopterSearch::CopterSearch() : m_wihtout_recognize(0), m_wihtout_recognize_num(1),
                               m_max_set_size(35), m_min_set_size(5),
                               m_max_delta_x(350), m_min_delta_x(100),
                               m_max_delta_y(300), m_min_delta_y(50),
                               m_delta_old_object(50),
                               m_flag_set_size(true),
                               m_flag_size(true),
                               m_flag_old_object(true),
                               m_flag_recognized(false),
                               m_copterSet()
{
	PRINT_DBG(true, PREF, "");
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearch::getIndexOfObjectSet()
{
	// Create a vector with minimum and maximum values of coordinates
	// (size of this vector is equal to size of disjoint set)
	std::vector<MinMax_XY> mm_xy(m_d_set.size());

	// Sets in disjoint set
	for (uint32_t i = 0; i < m_d_set.size(); ++i) {

		// Set index
		mm_xy[i].i = i;

		// Check the size of set
		uint32_t set_size = static_cast<uint32_t>(m_d_set[i].size());
		if (0 == set_size) {
			mm_xy[i].valid = false;
			continue;
		}
		mm_xy[i].valid = true;
		mm_xy[i].set_size = set_size;

		// Members in set
		for (uint32_t j = 0; j < m_d_set[i].size(); ++j) {

			// Points in member
			for (uint32_t k = 0; k < m_d_set[i][j].points.size(); ++k) {
				// x
				// max
				if (m_d_set[i][j].points[k].x > mm_xy[i].max_x)
					mm_xy[i].max_x = m_d_set[i][j].points[k].x;
				// min
				else if (m_d_set[i][j].points[k].x < mm_xy[i].min_x)
					mm_xy[i].min_x = m_d_set[i][j].points[k].x;

				// y
				// max
				if (m_d_set[i][j].points[k].y > mm_xy[i].max_y)
					mm_xy[i].max_y = m_d_set[i][j].points[k].y;
				// min
				else if (m_d_set[i][j].points[k].y < mm_xy[i].min_y)
					mm_xy[i].min_y = m_d_set[i][j].points[k].y;
			}

		}

		// Calculate delta x and y
		mm_xy[i].delta_x = mm_xy[i].max_x - mm_xy[i].min_x;
		mm_xy[i].delta_y = mm_xy[i].max_y - mm_xy[i].min_y;
	}

	// Print a content of the vector member with maximum, minimum values
	// and other parameters
	if (true == m_debug) {
		for (auto el : mm_xy) {
			if (false == el.valid)
				continue;
			el.print();
		}
		PRINT_DBG(true, PREF, "-----------------");
	}

	// Index for return value; maximal size of suitable set
	uint32_t index, max_set_size = 0;

	// Process all parameters in vector
	for (uint32_t i = 0; i < mm_xy.size(); ++i) {

		// Valid
		if (false == mm_xy[i].valid)
			continue;

		// Filter a member by size in set
		if (true == m_flag_set_size) {
			if (mm_xy[i].set_size < m_min_set_size || mm_xy[i].set_size > m_max_set_size)
				continue;
		}

		// Filter a member by size
		if (true == m_flag_size) {
			if (!(   mm_xy[i].delta_x > m_min_delta_x
			      && mm_xy[i].delta_x < m_max_delta_x
			      && mm_xy[i].delta_y > m_min_delta_y
			      && mm_xy[i].delta_y < m_max_delta_y)) {
				PRINT_DBG(m_debug, PREF, "stop: filter by size, index = %lu",
				          static_cast<unsigned long>(mm_xy[i].i));
				continue;
			}
		}

		// Filter a member by old object position
		if (true == m_flag_old_object) {
			if (true == m_old_object.valid) {
				if (!(   std::abs(static_cast<int>(m_old_object.max_x) -
				                  static_cast<int>(mm_xy[i].max_x)) <
				         m_delta_old_object
				      && std::abs(static_cast<int>(m_old_object.max_y) -
				                  static_cast<int>(mm_xy[i].max_y)) <
				         m_delta_old_object
				      && std::abs(static_cast<int>(m_old_object.min_x) -
				                  static_cast<int>(mm_xy[i].min_x)) <
				         m_delta_old_object
				      && std::abs(static_cast<int>(m_old_object.min_y) -
				                  static_cast<int>(mm_xy[i].min_y)) <
				         m_delta_old_object)) {
					PRINT_DBG(m_debug, PREF, "stop: filter by old obj, index = %lu",
					          static_cast<unsigned long>(mm_xy[i].i));
					continue;
				}
			}
		}

		PRINT_DBG(m_debug, PREF, "detect, index = %lu",
		          static_cast<unsigned long>(mm_xy[i].i));

		// Choice a set with maximum size
		if (mm_xy[i].set_size > max_set_size) {
			max_set_size = mm_xy[i].set_size;
			index = i;
		}
	}

	// Verify recognition
	if (0 == max_set_size) {
		m_flag_recognized = false;
		m_copterSet.resize(0);
		if (++m_wihtout_recognize > m_wihtout_recognize_num) {
			PRINT_DBG(m_debug, PREF, "m_wihtout_recognize > %lu",
			          static_cast<unsigned long>(m_wihtout_recognize_num));
			m_wihtout_recognize = 0;
			m_old_object = MinMax_XY();
		}
		PRINT_DBG(m_debug, PREF, "Copter was not recognized");
		index = -1;
	} else {
		m_flag_recognized = true;
		m_copterSet = m_d_set[index];
		mm_xy[index].print();
		PRINT_DBG(m_debug, PREF, "is an old object now");
		m_old_object = mm_xy[index];
	}

	PRINT_DBG(m_debug, PREF, "=================");

	return index;
}

//-------------------------------------------------------------------------------------------------
void CopterSearch::drawFilterRectangles(uint8_t* const data,
                                        const uint32_t width, const uint32_t height) const
{
  #define OFFSET  20

	// Check the size of image
	if (   OFFSET + m_max_delta_y + 2 * m_delta_old_object > height
	    || OFFSET + m_max_delta_x + 2 * m_delta_old_object > width) {
		PRINT_ERR(true, PREF, "");
		return;
	}

	// Rectangle for old object
	// Lines
	for (uint32_t i = 0; i < m_max_delta_x + 2 * m_delta_old_object; i += 4) {
		data[(i + OFFSET) + width * OFFSET] = 122;
		data[(i + OFFSET) + width * (m_max_delta_y + OFFSET + 2 * m_delta_old_object)] = 122;
	}
	// Columns
	for (uint32_t i = 0; i < m_max_delta_y + 2 * m_delta_old_object; i += 4) {
		data[OFFSET + width * (i + OFFSET)] = 122;
		data[OFFSET + m_max_delta_x + width * (i + OFFSET) + 2 * m_delta_old_object] = 122;
	}

	// Rectangle for object (max)
	// Lines
	for (uint32_t i = 0; i < m_max_delta_x; ++i) {
		data[(i + OFFSET + m_delta_old_object) + width * (OFFSET + m_delta_old_object)] = 170;
		data[(i + OFFSET + m_delta_old_object) + width * (m_max_delta_y + OFFSET +
		                                                  m_delta_old_object)] = 170;
	}
	// Columns
	for (uint32_t i = 0; i < m_max_delta_y; ++i) {
		data[(OFFSET + m_delta_old_object) + width * (i + OFFSET + m_delta_old_object)] = 170;
		data[(OFFSET + m_delta_old_object) + m_max_delta_x + width * (i + OFFSET +
		                                                              m_delta_old_object)] = 170;
	}

	// Rectangle for object (min)
	// Lines
	for (uint32_t i = 0; i < m_min_delta_x; ++i) {
		data[(i + OFFSET + m_delta_old_object) + width * (OFFSET + m_delta_old_object)] = 170;
		data[(i + OFFSET + m_delta_old_object) + width * (m_min_delta_y + OFFSET +
		                                                  m_delta_old_object)] = 170;
	}
	// Columns
	for (uint32_t i = 0; i < m_min_delta_y; ++i) {
		data[(OFFSET + m_delta_old_object) + width * (i + OFFSET + m_delta_old_object)] = 170;
		data[(OFFSET + m_delta_old_object) + m_min_delta_x + width * (i + OFFSET +
		                                                              m_delta_old_object)] = 170;
	}

  #undef OFFSET
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearch::get_ObjParameters(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const
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
const std::vector<Ellipse>& CopterSearch::get_ObjParameters() const
{
	// Empty set
	static const std::vector<Ellipse> empty_set;

	// Check a valid of recognized object
	if (false == m_flag_recognized || m_copterSet.size() == 0)
		return empty_set;

	// Get parameters
	return m_copterSet;
}
