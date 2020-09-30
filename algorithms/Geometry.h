
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Geometry structs and types
TODO:
 * test class (logic() method)
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <vector>   // std::vector
#include <cstdlib>  // std::abs()

#ifdef SEC_145_OPENCV_EN
#include "opencv2/core/types.hpp"  // cv::Point type
#endif  // SEC_145_OPENCV_EN

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Struct for presenting of point (coordinates - uint32_t)
struct Point {
	uint32_t x;
	uint32_t y;
};

//-------------------------------------------------------------------------------------------------
// Struct for presenting of point (coordinates - float)
struct Point_f {
	float x;
	float y;
};

//-------------------------------------------------------------------------------------------------
// Class for presenting an ellipse
class Ellipse 
{
public:

	Ellipse()
	{
		m_p_1.x = m_p_2.x = 0;
		m_p_1.y = m_p_2.y = 0;
	}

	// Compare ellipses with a given metric
	bool compareWithMetric(const Ellipse& ell, const uint32_t metric) const noexcept
	{
		for (uint32_t i = 0; i < this->m_points.size(); ++i)
		{
			for (uint32_t j = 0; j < ell.m_points.size(); ++j)
			{
				if (   std::abs(static_cast<int>(this->m_points[i].x) -
				                static_cast<int>(ell.m_points[j].x)) <
					   static_cast<int>(metric)
				    && std::abs(static_cast<int>(this->m_points[i].y) -
				                static_cast<int>(ell.m_points[j].y)) <
				       static_cast<int>(metric)
				   )
				{
					// Ellipses are near
					return true;
				}
			}
		}

		// Ellipses are not near
		return false;
	}

	void compareMaxMinXY(uint32_t& max_x, uint32_t& min_x,
	                     uint32_t& max_y, uint32_t& min_y) const noexcept
	{
		// Points in member
		for (uint32_t k = 0; k < this->m_points.size(); ++k)
		{
			// x
			//  max
			if (this->m_points[k].x > max_x)
				max_x = this->m_points[k].x;
			//  min
			if (this->m_points[k].x < min_x)
				min_x = this->m_points[k].x;

			// y
			//  max
			if (this->m_points[k].y > max_y)
				max_y = this->m_points[k].y;
			//  min
			if (this->m_points[k].y < min_y)
				min_y = this->m_points[k].y;
		}
	}

	// The points belong to this ellipse
	std::vector<Point> m_points;

	// Maximal and minimal points
	Point m_p_1;
	Point m_p_2;

	// Ellipse id
	uint32_t m_id {0};
};

//-------------------------------------------------------------------------------------------------
#ifdef SEC_145_OPENCV_EN

// Class for presenting a point (OpenCV point extension)
class CvPoint : public cv::Point
{
public:

	CvPoint()
	{
	}

	explicit CvPoint(const cv::Point& p) : cv::Point(p)
	{
	}

	// Compare points with a given metric
	bool compareWithMetric(const cv::Point& p, const uint32_t metric) const noexcept
	{
		if (   std::abs(this->x - p.x) < static_cast<int>(metric)
		    && std::abs(this->y - p.y) < static_cast<int>(metric))
		{
			// Points are near
			return true;
		}

		// Points are not near
		return false;
	}

	void compareMaxMinXY(uint32_t& max_x, uint32_t& min_x,
	                     uint32_t& max_y, uint32_t& min_y) const noexcept
	{
		if (this->x < 0 || this->y < 0) {
			PRINT_ERR(true, "[CvPoint]: ", "One of the point coordinate is less than 0");
			return;
		}

		uint32_t x = static_cast<uint32_t>(this->x);
		uint32_t y = static_cast<uint32_t>(this->y);

		// x
		//  max
		if (x > max_x)
			max_x = x;
		//  min
		if (x < min_x)
			min_x = x;

		// y
		//  max
		if (y > max_y)
			max_y = y;
		//  min
		if (y < min_y)
			min_y = y;
	}
};

#endif  // SEC_145_OPENCV_EN

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
