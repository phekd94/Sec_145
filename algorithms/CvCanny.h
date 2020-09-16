
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class implemets a contour search by Canny contour detector from OpenCV
TODO:
 * test class (logic() method)
FIXME:
DANGER:
NOTE:

Sec_145::CvCanny
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "DisjointSet.h"  // DisjointSet template class (for inheritance)
#include "Geometry.h"     // CvPoint class
#include <cstdint>        // integer types
#include <QImage>         // QImage class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Class implemets a contour search by Canny contour detector from OpenCV
class CvCanny : protected DisjointSet<CvPoint>
{
public:

	explicit CvCanny(const uint32_t threshold = 15, const uint32_t kernelSize = 3);

	// Apply Canny detecrot to the image data
	int32_t applyDetector(QImage image); // const QImage

	// ***** Setters *****

	// Sets a threshold
	void setThreshold(const uint32_t threshold)
	{
		m_threshold = threshold;
	}

	// Sets a kernel size
	void setKernelSize(const uint32_t kernelSize)
	{
		m_kernelSize = kernelSize;
	}

	// *******************

private:

	// Preface in debug message
	static const char* const PREF;

	// Set of sets with contour pointers
	std::vector<std::vector<cv::Point>> m_contours;

	// Threshold for Canny detector
	uint32_t m_threshold;

	// Kernel size for Sobel operator
	uint32_t m_kernelSize;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
