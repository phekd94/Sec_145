
#ifndef SEC_145_ALGORITHMS_JUSTIERENIMAGECHAMELEON_H
#define SEC_145_ALGORITHMS_JUSTIERENIMAGECHAMELEON_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class (and class for test) for justieren of image in chameleon project
TODO: qDebug
FIXME:
DANGER: * width and height of the RECTANGLE must be less than angle_max / 2;
		  this is NOT verified (it is impossible here)
NOTE:

Sec_145::JustierenImageChameleon class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|     YES(*)    |     YES    |
+---------------+------------+
(*) - NO if using sets edge nodes methods
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>     // integer types
#include "Geometry.h"  // Point_f struct

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class JustierenImageChameleon
{
	friend class JustierenImageChameleon_test;

public:
	JustierenImageChameleon(uint32_t image_width = 1280,
	                        uint32_t image_height = 1280,
	                        uint32_t cells_num_x = 320,
	                        uint32_t cells_num_y = 320,
	                        uint32_t cell_width = 0,
	                        uint32_t cell_height = 0,
	                        uint32_t angle_max = 360,
	                        uint32_t diff_max = 50);

	// Sets edge nodes
	int32_t setTopLeft(float angle_x, float angle_y)
	{
		return setEdgeNode(angle_x, angle_y, m_Top_Left);
	}
	int32_t setTopRight(float angle_x, float angle_y)
	{
		return setEdgeNode(angle_x, angle_y, m_Top_Right);
	}
	int32_t setBottomLeft(float angle_x, float angle_y)
	{
		return setEdgeNode(angle_x, angle_y, m_Bottom_Left);
	}
	int32_t setBottomRight(float angle_x, float angle_y)
	{
		return setEdgeNode(angle_x, angle_y, m_Bottom_Right);
	}

	// Calculates angles
	int32_t calcAngles(uint32_t cell_x, uint32_t cell_y,
	                   float& angle_yaw,
	                   float& angle_pitch,
	                   const float static_shift_yaw = 0,
	                   const float static_shift_pitch = 0,
	                   const bool dynamic_enable = false,
	                   float dynamic_R = 200);
	int32_t calcAngles(uint32_t cell_x, uint32_t cell_y,
	                   float* angle_yaw,
	                   float* angle_pitch,
	                   const float static_shift_yaw = 0,
	                   const float static_shift_pitch = 0,
	                   const bool dynamic_enable = false,
	                   float dynamic_R = 200);

	// Calculates cells
	int32_t calcCells(float angle_yaw, float angle_pitch,
	                  uint32_t& cell_x,
	                  uint32_t& cell_y,
	                  const float static_shift_yaw = 0,
	                  const float static_shift_pitch = 0,
	                  const bool dynamic_enable = false,
	                  float dynamic_R = 200);
	int32_t calcCells(float angle_yaw, float angle_pitch,
	                  uint32_t* cell_x,
	                  uint32_t* cell_y,
	                  const float static_shift_yaw = 0,
	                  const float static_shift_pitch = 0,
	                  const bool dynamic_enable = false,
	                  float dynamic_R = 200);

private:
	// Constant parameters
	uint32_t IMAGE_WIDTH;
	uint32_t IMAGE_HEIGHT;
	uint32_t CELLS_NUM_X;
	uint32_t CELLS_NUM_Y;
	uint32_t CELL_WIDTH;
	uint32_t CELL_HEIGHT;
	uint32_t ANGLE_MAX;
	uint32_t DIFF_MAX;

	// Flag for the test class
	bool m_test;

	// Initialization of the rectangle edge nodes
	bool m_init;
	void init();
	void getTransMatrix();
	void init_x();
	void init_y();

	// Shifts
	float m_shift_x;
	float m_shift_y;

	// Image rectangle edge nodes
	Point_f m_Top_Left;
	Point_f m_Top_Right;
	Point_f m_Bottom_Left;
	Point_f m_Bottom_Right;

	// Elements of the transformation matrix
	float s11;
	float s21;
	float s12;
	float s22;

	// Sets edge node
	int32_t setEdgeNode(const float angle_x, const float angle_y, Point_f& point);

	// Prints points
	void printPoints() const;
};

//=================================================================================================
// Test class

class JustierenImageChameleon_test
{

public:
	static int32_t test();

private:
	struct Points {
		float Top_Left;
		float Top_Right;
		float Bottom_Left;
		float Bottom_Right;
	};

	static int32_t setEdges(JustierenImageChameleon& obj,
	                        const Points* const p_x, const Points* const p_y,
	                        const uint32_t i, const uint32_t j);
	static int32_t checkAngles(JustierenImageChameleon& obj,
	                           const Points* const p_x, const Points* const p_y,
	                           const uint32_t i, const uint32_t j);
	static int32_t checkCells(JustierenImageChameleon& obj,
	                          const Points* const p_x, const Points* const p_y,
	                          const uint32_t i, const uint32_t j);
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_JUSTIERENIMAGECHAMELEON_H
