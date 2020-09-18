
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class implemets a contour search by Canny contour detector from OpenCV
TODO:
 * test class (logic() method)
 * cv::blur(): why?
FIXME:
DANGER:
NOTE:

Sec_145::CvCanny
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>        // integer types
#include <vector>         // std::vector
#include "DisjointSet.h"  // DisjointSet template class (for inheritance)
#include "Geometry.h"     // CvPoint class

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class implemets a contour search by Canny contour detector from OpenCV
class CvCanny : protected DisjointSet<CvPoint>
{
public:

	explicit CvCanny(const uint32_t threshold = 15, const uint32_t kernelSize = 3) :
	    m_threshold(threshold), m_kernelSize(kernelSize)
	{
		PRINT_DBG(DisjointSet<CvPoint>::m_debug, PREF, "");
	}

	// Applies Canny detector to the image data
	int32_t applyDetector(const uint8_t* const greyData,
	                      const uint32_t width, const uint32_t height) noexcept;

	// ***********************
	// ******* Setters *******
	// ***********************

	// Sets a threshold
	void setThreshold(const uint32_t threshold) noexcept
	{
		m_threshold = threshold;
	}

	// Sets a kernel size
	void setKernelSize(const uint32_t kernelSize) noexcept
	{
		m_kernelSize = kernelSize;
	}

private:

	// Preface in debug message
	static const char* const PREF;

	// Set of sets with contour points
	std::vector<std::vector<cv::Point>> m_contours;

	// Threshold for Canny detector
	uint32_t m_threshold;

	// Kernel size for Sobel operator
	uint32_t m_kernelSize;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
