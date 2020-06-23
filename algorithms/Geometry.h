
#ifndef SEC_145_ALGORITHMS_GEOMETRY_H
#define SEC_145_ALGORITHMS_GEOMETRY_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Geometry structs
TODO:
FIXME:
DANGER:
NOTE:
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <vector>   // std::vector

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
// Struct for presenting of ellipse
struct Ellipse {
	Ellipse() : id(0)
	{
		p_1.x = p_2.x = 0;
		p_1.y = p_2.y = 0;
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
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_GEOMETRY_H
