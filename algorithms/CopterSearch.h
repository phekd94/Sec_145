
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class for copter search
TODO: * test class (logic() method)
	  * drawFilterRectangles(): boundaries works incorrect
FIXME:
DANGER: * drawFilterRectangles(): boundaries work incorrect
NOTE:

Sec_145::CopterSearch
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES(*)   |    YES     |
+---------------+------------+
(*) - single parameters for all threads
*/

//-------------------------------------------------------------------------------------------------
#include "DisjointSet_Ellipses.h"  // DisjointSet_Ellipses class (for inheritance)

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class CopterSearch : public DisjointSet_Ellipses {

public:

	CopterSearch();

	// Get index of set of set contains object
	virtual int32_t getIndexOfObjectSet() override final;

	// Draws a rectangles for filter
	void drawFilterRectangles(uint8_t* const data,
	                          const uint32_t width, const uint32_t height) const;

	// ***** Getters *****

	// Gets object parameters: left top coordinates of rectangle, width and height
	int32_t get_ObjParameters(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const;

	// Gets object parameters: ellipses in copter set
	const std::vector<Ellipse>& get_ObjParameters() const;

	// *******************

	// ***** Setters *****

	// m_wihtout_recognize_num
	void set_wihtout_recognize_num(const uint32_t wihtout_recognize_num)
	{ m_wihtout_recognize_num = wihtout_recognize_num; }

	// m_max_set_size, m_min_set_size
	void set_sizeOfSet(const uint32_t max_set_size, const uint32_t min_set_size)
	{ m_max_set_size = max_set_size; m_min_set_size = min_set_size; }

	// m_max_delta_x, m_min_delta_x, m_max_delta_y, m_min_delta_y
	void set_deltaXY(const uint32_t max_delta_x, const uint32_t min_delta_x,
	                 const uint32_t max_delta_y, const uint32_t min_delta_y)
	{ m_max_delta_x = max_delta_x; m_min_delta_x = min_delta_x;
	  m_max_delta_y = max_delta_y; m_min_delta_y = min_delta_y; }

	// m_delta_old_object
	void set_deltaOld(const int32_t delta_old_object)
	{ m_delta_old_object = (delta_old_object < 0 ? 0 : delta_old_object); }

	// m_flag_set_size
	void setFlag_setSize(const bool flag)
	{ m_flag_set_size = flag; }

	// m_flag_size
	void setFlag_size(const bool flag)
	{ m_flag_size = flag; }

	// m_flag_old_object
	void setFlag_oldObject(const bool flag)
	{ m_flag_old_object = flag; }

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
		{  }

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
	std::vector<Ellipse> m_copterSet;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
