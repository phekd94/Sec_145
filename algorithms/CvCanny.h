
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

	// Type frame enumeration
	enum class eFrameType
	{
		Source,
		Differential
	};

	CvCanny()
	{
		PRINT_DBG(DisjointSet<CvPoint>::m_debug, "");
	}

	// Applies Canny detector to the image data
	int32_t applyDetector(const uint8_t* const greyData,
	                      const uint32_t width, const uint32_t height) noexcept;

	// ***********************
	// ******* Getters *******
	// ***********************

	// Gets a frame type
	eFrameType getFrameType() const noexcept
	{
		return m_frameType;
	}

	// Gets start time
	uint64_t getStartTime() const noexcept
	{
		return m_startTime;
	}

	// Gets finish time
	uint64_t getFinishTime() const noexcept
	{
		return m_finishTime;
	}

	// ***********************
	// ******* Setters *******
	// ***********************

	// Sets a threshold for Canny detector
	void setCannyThreshold(const uint32_t cannyThreshold) noexcept
	{
		m_cannyThreshold = cannyThreshold;
	}

	// Sets a kernel size
	void setKernelSize(const uint32_t kernelSize) noexcept
	{
		m_kernelSize = kernelSize;
	}

	// Sets a step for adjustment of threshold for Canny detector
	void setStepCannyThreshold(const uint32_t stepCannyThreshold) noexcept
	{
		m_stepCannyThreshold = stepCannyThreshold;
	}

	// Sets execution time limits
	int32_t setExecutionTimeLimits(const uint32_t lowExecutionTime,
	                               const uint32_t highExecutionTime) noexcept
	{
		if (lowExecutionTime > highExecutionTime)
		{
			PRINT_ERR("lowExecutionTime > highExecutionTime");
			return -1;
		}
		m_lowExecutionTime = lowExecutionTime;
		m_highExecutionTime = highExecutionTime;
		return 0;
	}

	// Sets a threshold for threshold OpenCV function
	void setThreshold(const uint32_t threshold) noexcept
	{
		m_threshold = threshold;
	}

	// Sets a ratio of sky points and points with hard background
	void setFrameTypeRatio(const double frameTypeRatio) noexcept
	{
		m_frameTypeRatio = frameTypeRatio;
	}

private:

	// Constants
	static const uint32_t lowCannyThreshold {10};
	static const uint32_t highCannyThreshold {150};

	// Set of sets with contour points
	std::vector<std::vector<cv::Point>> m_contours;

	// Start and finish time
	uint64_t m_startTime {0};
	uint64_t m_finishTime {0};

	// Threshold for Canny detector
	uint32_t m_cannyThreshold {20};

	// Kernel size for Sobel operator
	uint32_t m_kernelSize {3};

	// Frame type
	eFrameType m_frameType {eFrameType::Source};

	// Step for adjustment of threshold for Canny detector
	uint32_t m_stepCannyThreshold {5};

	// Execution time limits
	uint32_t m_lowExecutionTime {15};
	uint32_t m_highExecutionTime {25};

	// Threshold for threshold OpenCV function
	uint32_t m_threshold {130};

	// Ratio of sky points and points with hard background
	double m_frameTypeRatio {0.6};

	// Adjusts parameters
	void adjustParameters();
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
