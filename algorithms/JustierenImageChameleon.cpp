
#include "JustierenImageChameleon.h"

#include <algorithm>  // std::max(), std::min()
#include <cmath>      // std::sin(), std::fabs()

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
#define PREF  "[JustierenImageChameleon]: "

//-------------------------------------------------------------------------------------------------
JustierenImageChameleon::JustierenImageChameleon(
        uint32_t image_width, uint32_t image_height,
        uint32_t cells_num_x, uint32_t cells_num_y,
        uint32_t cell_width, uint32_t cell_height,
        uint32_t angle_max, uint32_t diff_max) :
    IMAGE_WIDTH(image_width), IMAGE_HEIGHT(image_height),
    CELLS_NUM_X(cells_num_x), CELLS_NUM_Y(cells_num_y),
    CELL_WIDTH(cell_width == 0 ? IMAGE_WIDTH / CELLS_NUM_X : cell_width),
    CELL_HEIGHT(cell_height == 0 ? IMAGE_HEIGHT / CELLS_NUM_Y : cell_height),
    ANGLE_MAX(angle_max), DIFF_MAX(diff_max),
    m_test(false),
    m_init(false),
    m_shift_x(0),
    m_shift_y(0),
    m_Top_Left{-1, -1},
    m_Top_Right{-1, -1},
    m_Bottom_Left{-1, -1},
    m_Bottom_Right{-1, -1},
    s11(0), s21(0), s12(0), s22(0)
{

}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon::calcAngles(uint32_t cell_x, uint32_t cell_y,
                                            float& angle_yaw,
                                            float& angle_pitch,
                                            const float static_shift_yaw,
                                            const float static_shift_pitch,
                                            const bool dynamic_enable,
                                            float dynamic_R)
{
	// Check incoming parameters
	if (m_test == false ? cell_x >= CELLS_NUM_X : cell_x > CELLS_NUM_X) {
		PRINT_ERR(true, PREF, "x >= %lu", static_cast<unsigned long>(CELLS_NUM_X));
		return -1;
	}
	if (m_test == false ? cell_y >= CELLS_NUM_Y : cell_y > CELLS_NUM_Y) {
		PRINT_ERR(true, PREF, "y >= %lu", static_cast<unsigned long>(CELLS_NUM_Y));
		return -1;
	}

	// Check edge nodes
	if (   m_Top_Left.x < 0 || m_Top_Left.y < 0
	    || m_Top_Right.x < 0 || m_Top_Right.y < 0
	    || m_Bottom_Left.x < 0 || m_Bottom_Left.y < 0
	    || m_Bottom_Right.x < 0 || m_Bottom_Right.y < 0) {
		PRINT_ERR(true, PREF, "one component of the edge nodes less than 0");
		return -1;
	}

	// Initialization of the rectangle edge nodes
	if (m_init == false) {
		init();
		m_init = true;
	}

	// Need a center of the cell
	float cell_x_f = static_cast<float>(2 * cell_x + (m_test == false ? 1 : 0));
	float cell_y_f = static_cast<float>(2 * cell_y + (m_test == false ? 1 : 0));

	// Apply a dynamic compression
	if (dynamic_enable == true) {
		float c;

		if (dynamic_R < 0) {
			PRINT_ERR(true, PREF, "dynamic_R < 0");
			return -1;
		}

		if (cell_x_f >= (CELLS_NUM_X / 2) * 2) {
			c = cell_x_f - (CELLS_NUM_X / 2) * 2;
			cell_x_f = (CELLS_NUM_X / 2) * 2 + dynamic_R * std::sin(c / dynamic_R);
		} else {
			c = (CELLS_NUM_X / 2) * 2 - 1 * 2 - cell_x_f ;
			cell_x_f = (CELLS_NUM_X / 2) * 2 - 1 * 2 - dynamic_R * std::sin(c / dynamic_R);
		}

		if (cell_y_f >= (CELLS_NUM_Y / 2) * 2) {
			c = cell_y_f - (CELLS_NUM_Y / 2) * 2;
			cell_y_f = (CELLS_NUM_Y / 2) * 2 + dynamic_R * std::sin(c / dynamic_R);
		} else {
			c = (CELLS_NUM_Y / 2) * 2 - 1 * 2 - cell_y_f ;
			cell_y_f = (CELLS_NUM_Y / 2) * 2 - 1 * 2 - dynamic_R * std::sin(c / dynamic_R);
		}
	}

	// Coordinates (in angles) of the specify cell centre
	angle_yaw   = s11 * cell_x_f + s12 * cell_y_f + m_Top_Left.x;
	angle_pitch = s21 * cell_x_f + s22 * cell_y_f + m_Top_Left.y;

	// Shifts
	angle_yaw   -= m_shift_x;
	angle_pitch -= m_shift_y;

	// Apply static shifts
	angle_yaw   += static_shift_yaw;
	angle_pitch += static_shift_pitch;

	// Check boundare for angle_yaw
	if (angle_yaw >= ANGLE_MAX) {
		angle_yaw -= ANGLE_MAX;
	} else if (angle_yaw < 0) {
		angle_yaw += ANGLE_MAX;
	}
	if (angle_yaw >= ANGLE_MAX || angle_yaw < 0) {
		PRINT_ERR(true, PREF, "angle_yaw - something went wrong");
		return -1;
	}

	// Check boundare for angle_pitch
	if (angle_pitch >= ANGLE_MAX) {
		angle_pitch -= ANGLE_MAX;
	} else if (angle_pitch < 0) {
		angle_pitch += ANGLE_MAX;
	}
	if (angle_pitch >= ANGLE_MAX || angle_pitch < 0) {
		PRINT_ERR(true, PREF, "angle_pitch - something went wrong");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon::calcAngles(uint32_t cell_x, uint32_t cell_y,
                                            float* angle_yaw,
                                            float* angle_pitch,
                                            const float static_shift_yaw,
                                            const float static_shift_pitch,
                                            const bool dynamic_enable,
                                            float dynamic_R)
{
	if (angle_yaw == nullptr || angle_pitch == nullptr) {
		PRINT_ERR(true, PREF, "angle_yaw == nullptr or angle_pitch == nullptr");
		return -1;
	}
	return calcAngles(cell_x, cell_y, *angle_yaw, *angle_pitch,
	                  static_shift_yaw, static_shift_pitch,
	                  dynamic_enable, dynamic_R);
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon::calcCells(float angle_yaw, float angle_pitch,
                                           uint32_t& cell_x,
                                           uint32_t& cell_y,
                                           const float static_shift_yaw,
                                           const float static_shift_pitch,
                                           const bool dynamic_enable,
                                           float dynamic_R)
{
	// Check edge nodes
	if (   m_Top_Left.x < 0 || m_Top_Left.y < 0
	    || m_Top_Right.x < 0 || m_Top_Right.y < 0
	    || m_Bottom_Left.x < 0 || m_Bottom_Left.y < 0
	    || m_Bottom_Right.x < 0 || m_Bottom_Right.y < 0) {
		PRINT_ERR(true, PREF, "one component of the edge nodes less than 0");
		return -1;
	}

	// Initialization of the rectangle edge nodes
	if (m_init == false) {
		init();
		m_init = true;
	}

	// Apply shift to the angle_yaw
	angle_yaw += m_shift_x;
	if (angle_yaw > ANGLE_MAX) {
		angle_yaw -= ANGLE_MAX;
	} else if (angle_yaw == ANGLE_MAX) {
		if (m_shift_x > 0) {
			angle_yaw = 0;  // 1.1 x, 1.2 x, 3.1 x, 3.2 x
		}
	} else if (angle_yaw < 0) {
		angle_yaw += ANGLE_MAX;
	} else if (angle_yaw == 0) {
		if (m_shift_x < 0) {
			angle_yaw = static_cast<float>(ANGLE_MAX);  // 2.1 x, 2.2 x
		}
	}

	if (   angle_yaw < std::min(m_Top_Left.x, m_Bottom_Left.x)
	    || angle_yaw > std::max(m_Top_Right.x, m_Bottom_Right.x)) {
		PRINT_ERR(true, PREF, "angle_yaw < minLeft_x || angle_yaw > maxRight_x");
		return -1;
	}

	// Apply shift to the angle_pitch
	angle_pitch += m_shift_y;
	if (angle_pitch > ANGLE_MAX) {
		angle_pitch -= ANGLE_MAX;
	} else if (angle_pitch == ANGLE_MAX) {
		if (m_shift_y > 0) {
			angle_pitch = 0;  // 1.1 y, 1.2 y, 3.1 y, 3.2 y
		}
	} else if (angle_pitch < 0) {
		angle_pitch += ANGLE_MAX;
	} else if (angle_pitch == 0) {
		if (m_shift_y < 0) {
			angle_pitch = static_cast<float>(ANGLE_MAX);  // 2.1 y, 2.2 y
		}
	}

	if (   angle_pitch < std::min(m_Top_Left.y, m_Top_Right.y)
	    || angle_pitch > std::max(m_Bottom_Left.y, m_Bottom_Right.y)) {
		PRINT_ERR(true, PREF, "angle_pitch < minTop_y || angle_pitch > maxBottom_y");
		return -1;
	}

	// Apply static shifts
	angle_yaw   += static_shift_yaw;
	angle_pitch += static_shift_pitch;

	// Coordinates (in cells) of the specify angles
	// (.../2 due to need just a coordinates of the cell)
	float cell_x_tmp = (s22 * angle_yaw - s12 * angle_pitch
	                    - s22 * m_Top_Left.x + s12 * m_Top_Left.y)
	                   / (s11 * s22 - s21 * s12) / 2;
	float cell_y_tmp = (-s21 * angle_yaw + s11 * angle_pitch
	                    + s21 * m_Top_Left.x - s11 * m_Top_Left.y)
	                   / (s11 * s22 - s21 * s12) / 2;

	// Check boundare for cell_x
	if (   (m_test == false ? cell_x_tmp >= CELLS_NUM_X : cell_x_tmp > CELLS_NUM_X)
	    || cell_x_tmp < 0) {
		PRINT_ERR(true, PREF, "cell_x >= CELLS_NUM_X || cell_x < 0");
		return -1;
	}

	// Check boundare for cell_y
	if (   (m_test == false ? cell_y_tmp >= CELLS_NUM_Y : cell_y_tmp > CELLS_NUM_Y)
	    || cell_y_tmp < 0) {
		PRINT_ERR(true, PREF, "cell_y >= CELLS_NUM_Y || cell_y < 0");
		return -1;
	}

	// Apply dynamic compression
	if (dynamic_enable == true) {
		float c;

		if (dynamic_R < 0) {
			PRINT_ERR(true, PREF, "dynamic_R < 0");
			return -1;
		}

		if (cell_x_tmp >= CELLS_NUM_X / 2) {
			c = cell_x_tmp - CELLS_NUM_X / 2;
			cell_x_tmp = CELLS_NUM_X / 2 + dynamic_R * std::sin(c / dynamic_R);
		} else {
			c = CELLS_NUM_X / 2 - 1 - cell_x_tmp;
			cell_x_tmp = CELLS_NUM_X / 2 - 1 - dynamic_R * std::sin(c / dynamic_R);
		}

		if (cell_y_tmp >= CELLS_NUM_Y / 2) {
			c = cell_y_tmp - CELLS_NUM_Y / 2;
			cell_y_tmp = CELLS_NUM_Y / 2 + dynamic_R * std::sin(c / dynamic_R);
		} else {
			c = CELLS_NUM_Y / 2 - 1 - cell_y_tmp;
			cell_y_tmp = CELLS_NUM_Y / 2 - 1 - dynamic_R * std::sin(c / dynamic_R);
		}
	}

	// Set a result
	cell_x = static_cast<uint32_t>(cell_x_tmp);
	cell_y = static_cast<uint32_t>(cell_y_tmp);
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon::calcCells(float angle_yaw, float angle_pitch,
                                           uint32_t* cell_x,
                                           uint32_t* cell_y,
                                           const float static_shift_yaw,
                                           const float static_shift_pitch,
                                           const bool dynamic_enable,
                                           float dynamic_R)
{
	if (cell_x == nullptr || cell_y == nullptr) {
		PRINT_ERR(true, PREF, "cell_x == nullptr or cell_y == nullptr");
		return -1;
	}
	return calcCells(angle_yaw, angle_pitch, *cell_x, *cell_y,
	                 static_shift_yaw, static_shift_pitch,
	                 dynamic_enable, dynamic_R);
}

//-------------------------------------------------------------------------------------------------
void JustierenImageChameleon::init()
{
	init_x();
	init_y();
	getTransMatrix();
}

//-------------------------------------------------------------------------------------------------
void JustierenImageChameleon::getTransMatrix()
{
	// Transformation matrix
	// (.../2 due to need a center of the cell)
	s11 = (m_Top_Right.x - m_Top_Left.x)   / CELLS_NUM_X / 2;
	s21 = (m_Top_Right.y - m_Top_Left.y)   / CELLS_NUM_Y / 2;
	s12 = (m_Bottom_Left.x - m_Top_Left.x) / CELLS_NUM_X / 2;
	s22 = (m_Bottom_Left.y - m_Top_Left.y) / CELLS_NUM_Y / 2;
}

//-------------------------------------------------------------------------------------------------
void JustierenImageChameleon::init_x()
{
	float r, ret;

	// 1 x
	r = m_Top_Left.x - m_Bottom_Left.x;
	if (std::fabs(r) > DIFF_MAX) {
		PRINT_DBG(false, PREF, "1 x");
		if (r > 0) {
			// 1.1 x
			PRINT_DBG(false, PREF, "1.1 x");
			m_shift_x = ANGLE_MAX - m_Top_Left.x;
			m_Top_Left.x = 0;
			m_Bottom_Left.x += m_shift_x;
			ret = 11;
		} else if (r < 0) {
			// 1.2 x
			PRINT_DBG(false, PREF, "1.2 x");
			m_shift_x = ANGLE_MAX - m_Bottom_Left.x;
			m_Bottom_Left.x = 0;
			m_Top_Left.x += m_shift_x;
			ret = 12;
		}
		m_Top_Right.x += m_shift_x;
		m_Bottom_Right.x += m_shift_x;
		// printPoints();
		return;
	}

	// 2 x
	r = m_Top_Right.x - m_Bottom_Right.x;
	if (std::fabs(r) > DIFF_MAX) {
		PRINT_DBG(false, PREF, "2 x");
		if (r > 0) {
			// 2.1 x
			PRINT_DBG(false, PREF, "2.1 x");
			m_shift_x = -m_Bottom_Right.x;
			m_Bottom_Right.x = static_cast<float>(ANGLE_MAX);
			m_Top_Right.x += m_shift_x;
			ret = 21;
		} else if (r < 0) {
			// 2.2 x
			PRINT_DBG(false, PREF, "2.2 x");
			m_shift_x = -m_Top_Right.x;
			m_Top_Right.x = static_cast<float>(ANGLE_MAX);
			m_Bottom_Right.x += m_shift_x;
			ret = 22;
		}
		m_Top_Left.x += m_shift_x;
		m_Bottom_Left.x += m_shift_x;
		// printPoints();
		return;
	}

	// normal x
	r = std::max(std::fabs(m_Top_Right.x - m_Top_Left.x),
	             std::fabs(m_Bottom_Right.x - m_Bottom_Left.x));
	if (r < ANGLE_MAX / 2) {
		PRINT_DBG(false, PREF, "normal x");
		ret = 0;
		return;
	}

	// 3 x
	r = m_Top_Left.x - m_Bottom_Left.x;
	if (r >= 0) {
		// 3.1 x
		PRINT_DBG(false, PREF, "3.1 x");
		m_shift_x = ANGLE_MAX - m_Bottom_Left.x;
		m_Bottom_Left.x = 0;
		m_Top_Left.x = r;
		ret = 31;
	} else {
		// 3.2 x
		PRINT_DBG(false, PREF, "3.2 x");
		m_shift_x = ANGLE_MAX - m_Top_Left.x;
		m_Top_Left.x = 0;
		m_Bottom_Left.x = -r;
		ret = 32;
	}
	m_Top_Right.x += m_shift_x;
	m_Bottom_Right.x += m_shift_x;
	// printPoints();
	return;
}

//-------------------------------------------------------------------------------------------------
void JustierenImageChameleon::init_y()
{
	float r, ret;

	// 1 y
	r = m_Top_Left.y - m_Top_Right.y;
	if (std::fabs(r) > DIFF_MAX) {
		PRINT_DBG(false, PREF, "1 y");
		if (r > 0) {
			// 1.1 y
			PRINT_DBG(false, PREF, "1.1 y");
			m_shift_y = ANGLE_MAX - m_Top_Left.y;
			m_Top_Left.y = 0;
			m_Top_Right.y += m_shift_y;
			ret = 11;
		} else if (r < 0) {
			// 1.2 y
			PRINT_DBG(false, PREF, "1.2 y");
			m_shift_y = ANGLE_MAX - m_Top_Right.y;
			m_Top_Right.y = 0;
			m_Top_Left.y += m_shift_y;
			ret = 12;
		}
		m_Bottom_Left.y += m_shift_y;
		m_Bottom_Right.y += m_shift_y;
		// printPoints();
		return;
	}

	// 2 y
	r = m_Bottom_Left.y - m_Bottom_Right.y;
	if (std::fabs(r) > DIFF_MAX) {
		PRINT_DBG(false, PREF, "2 y");
		if (r > 0) {
			// 2.1 y
			PRINT_DBG(false, PREF, "2.1 y");
			m_shift_y = -m_Bottom_Right.y;
			m_Bottom_Right.y = static_cast<float>(ANGLE_MAX);
			m_Bottom_Left.y += m_shift_y;
			ret = 21;
		} else if (r < 0) {
			// 2.2 y
			PRINT_DBG(false, PREF, "2.2 y");
			m_shift_y = -m_Bottom_Left.y;
			m_Bottom_Left.y = static_cast<float>(ANGLE_MAX);
			m_Bottom_Right.y += m_shift_y;
			ret = 22;
		}
		m_Top_Left.y += m_shift_y;
		m_Top_Right.y += m_shift_y;
		// printPoints();
		return;
	}

	// normal y
	r = std::max(std::fabs(m_Top_Right.y - m_Bottom_Right.y),
	             std::fabs(m_Top_Left.y - m_Bottom_Left.y));
	if (r < ANGLE_MAX / 2) {
		PRINT_DBG(false, PREF, "normal y");
		ret = 0;
		return;
	}

	// 3 y
	r = m_Top_Right.y - m_Top_Left.y;
	if (r >= 0) {
		// 3.1 y
		PRINT_DBG(false, PREF, "3.1 y");
		m_shift_y = ANGLE_MAX - m_Top_Left.y;
		m_Top_Left.y = 0;
		m_Top_Right.y = r;
		ret = 31;
	} else {
		// 3.2 y
		PRINT_DBG(false, PREF, "3.2 y");
		m_shift_y = ANGLE_MAX - m_Top_Right.y;
		m_Top_Right.y = 0;
		m_Top_Left.y = -r;
		ret = 32;
	}
	m_Bottom_Right.y += m_shift_y;
	m_Bottom_Left.y += m_shift_y;
	// printPoints();
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon::setEdgeNode(const float angle_x, const float angle_y,
                                             Point_f& point)
{
	// Check incoming parameters
	if (angle_x < 0 || angle_x >= ANGLE_MAX) {
		PRINT_ERR(true, PREF, "x < 0 or x >= %lu", static_cast<unsigned long>(ANGLE_MAX));
		return -1;
	}
	if (angle_y < 0 || angle_y >= ANGLE_MAX) {
		PRINT_ERR(true, PREF, "y < 0 or y >= %lu", static_cast<unsigned long>(ANGLE_MAX));
		return -1;
	}

	// Set angles
	point.x = angle_x;
	point.y = angle_y;

	// Clear an init flag and set shifts to 0
	m_init = false;
	m_shift_x = m_shift_y = 0;

	return 0;
}

//-------------------------------------------------------------------------------------------------
void JustierenImageChameleon::printPoints() const
{
	PRINT_DBG(true, PREF, "\n\t(%6.3f, %6.3f)\t(%6.3f, %6.3f)\n\t(%6.3f, %6.3f)\t(%6.3f, %6.3f)",
	                      m_Top_Left.x, m_Top_Left.y,
	                      m_Top_Right.x, m_Top_Right.y,
	                      m_Bottom_Left.x, m_Bottom_Left.y,
	                      m_Bottom_Right.x, m_Bottom_Right.y);
}

//-------------------------------------------------------------------------------------------------
#undef PREF

//=================================================================================================
// Defenition methods from test class

//-------------------------------------------------------------------------------------------------
#define PREF  "[JustierenImageChameleon_test]: "

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon_test::test()
{
	JustierenImageChameleon obj;
	obj.m_test = true;

	struct Points p_x[] = {
	    {30, 90, 10, 70},     // norm
	    {350, 50, 10, 70},    // 1.1 x
	    {10, 70, 350, 50},    // 1.2 x
	    {300, 350, 320, 10},  // 2.1 x
	    {320, 10, 300, 350},  // 2.2 x
	    {350, 20, 340, 10},   // 3.1 x
	    {340, 10, 350, 20},   // 3.2 x
    };
	struct Points p_y[] = {
	    {20, 10, 50, 40},     // norm
	    {350, 10, 80, 100},   // 1.1 y
	    {10, 350, 100, 80},   // 1.2 y
	    {230, 250, 350, 10},  // 2.1 y
	    {250, 230, 10, 350},  // 2.2 y
	    {300, 320, 50, 70},   // 3.1 y
	    {320, 300, 70, 50},   // 3.1 y
    };

	for (uint32_t i = 0; i < sizeof(p_x) / sizeof(Points); ++i) {
		for (uint32_t j = 0; j < sizeof(p_y) / sizeof(Points); ++j) {
			if (setEdges(obj, p_x, p_y, i, j) != 0) {
				return -1;
			}
			if (checkAngles(obj, p_x, p_y, i, j) != 0) {
				return -1;
			}
			if (checkCells(obj, p_x, p_y, i, j) != 0) {
				return -1;
			}
		}
	}

	PRINT_DBG(true, PREF, "Test was successful");
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon_test::setEdges(JustierenImageChameleon& obj,
                                               const Points* const p_x,
                                               const Points* const p_y,
                                               const uint32_t i,
                                               const uint32_t j)
{
	if (obj.setTopLeft(p_x[i].Top_Left, p_y[j].Top_Left) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, setTopLeft()",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (obj.setTopRight(p_x[i].Top_Right, p_y[j].Top_Right) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, setTopRight()",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (obj.setBottomLeft(p_x[i].Bottom_Left, p_y[j].Bottom_Left) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, setBottomLeft()",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (obj.setBottomRight(p_x[i].Bottom_Right, p_y[j].Bottom_Right) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, setBottomRight()",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon_test::checkAngles(JustierenImageChameleon& obj,
                                                  const Points* const p_x,
                                                  const Points* const p_y,
                                                  const uint32_t i,
                                                  const uint32_t j)
{
	float angle_yaw, angle_pitch;

	if (obj.calcAngles(0, 0, angle_yaw, angle_pitch) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcAngles(): Top_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (angle_yaw != p_x[i].Top_Left || angle_pitch != p_y[j].Top_Left) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check angles: Top_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcAngles(320, 0, angle_yaw, angle_pitch) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcAngles(): Top_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (angle_yaw != p_x[i].Top_Right || angle_pitch != p_y[j].Top_Right) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check angles: Top_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcAngles(0, 320, angle_yaw, angle_pitch) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcAngles(): Bottom_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (angle_yaw != p_x[i].Bottom_Left || angle_pitch != p_y[j].Bottom_Left) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check angles: Bottom_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcAngles(320, 320, angle_yaw, angle_pitch) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcAngles(): Bottom_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (angle_yaw != p_x[i].Bottom_Right || angle_pitch != p_y[j].Bottom_Right) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check angles: Bottom_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t JustierenImageChameleon_test::checkCells(JustierenImageChameleon& obj,
                                                 const Points* const p_x,
                                                 const Points* const p_y,
                                                 const uint32_t i,
                                                 const uint32_t j)
{
	uint32_t cell_x, cell_y;

	if (obj.calcCells(p_x[i].Top_Left, p_y[j].Top_Left, cell_x, cell_y) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcCells(): Top_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (cell_x != 0 || cell_y != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check cells: Top_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcCells(p_x[i].Top_Right, p_y[j].Top_Right, cell_x, cell_y) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcCells(): Top_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (cell_x != 320 || cell_y != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check cells: Top_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcCells(p_x[i].Bottom_Left, p_y[j].Bottom_Left, cell_x, cell_y) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcCells(): Bottom_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (cell_x != 0 || cell_y != 320) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check cells: Bottom_Left",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	if (obj.calcCells(p_x[i].Bottom_Right, p_y[j].Bottom_Right, cell_x, cell_y) != 0) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, calcCells(): Bottom_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}
	if (cell_x != 320 || cell_y != 320) {
		PRINT_ERR(true, PREF, "i = %d, j = %d, check cells: Bottom_Right",
		          static_cast<int>(i), static_cast<int>(j));
		return -1;
	}

	return 0;
}
