
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

	Ellipse() : id(0)
	{
		p_1.x = p_2.x = 0;
		p_1.y = p_2.y = 0;
	}

	// Compare ellipses with a given metric
	const bool compareWithMetric(const Ellipse& ell, const uint32_t metric) const
	{
		for (uint32_t i = 0; i < this->points.size(); ++i)
		{
			for (uint32_t j = 0; j < ell.points.size(); ++j)
			{
				if (   std::abs(static_cast<int>(this->points[i].x) -
				                static_cast<int>(ell.points[j].x)) <
					   static_cast<int>(metric)
					&& std::abs(static_cast<int>(this->points[i].y) -
				                static_cast<int>(ell.points[j].y)) <
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

	// The points belong to this ellipse
	std::vector<Point> points;

	// Maximal and minimal points
	Point p_1;
	Point p_2;

	// Ellipse id
	uint32_t id;
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
	const bool compareWithMetric(const cv::Point& p, const uint32_t metric) const
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
};

#endif  // SEC_145_OPENCV_EN

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
