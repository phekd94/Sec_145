
#include "CvCanny.h"

#include <exception>             // std::exception
#include <chrono>                // std::chrono functions
#include "opencv2/core/mat.hpp"  // cv::Mat
#include "opencv2/imgproc.hpp"   // cv::blur(); cv::Canny(); findContours(); cv::threshold()

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
int32_t CvCanny::applyDetector(const uint8_t* const greyData,
                               const uint32_t width, const uint32_t height) noexcept
{
	// Catch an exceptions from OpenCV functions
	try {

	// Matrix with image data
	cv::Mat image_tmp(height, width, CV_8UC1, const_cast<uint8_t*>(greyData));
	cv::Mat image, grayImage;

	// For threshold result
	cv::Mat dst;

	// Get start time
	m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	                  std::chrono::system_clock::now().time_since_epoch()).count();

	// Get a matrix with image data
	image_tmp.copyTo(image);
	image_tmp.copyTo(grayImage);

	// Blur an image
	cv::blur(image, image, cv::Size(3, 3));

	// Apply Canny detector
	cv::Canny(image, image, m_cannyThreshold, 3 * m_cannyThreshold, m_kernelSize);

	// Find contours (get a set of sets with contours points)
	findContours(image, m_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	// Add points to disjoint set
	for (auto contour : m_contours)
	{
		for (auto point : contour)
		{
			if (DisjointSet<CvPoint>::addMember(Sec_145::CvPoint(point)) != 0)
			{
				PRINT_ERR("addMember()");
				return -1;
			}
		}
	}

	// Get an image after applying a threshold
	cv::threshold(grayImage, dst, m_threshold, 255, cv::ThresholdTypes::THRESH_BINARY);

	// Calculate a points of sky and hard background
	uint32_t sum_255 {0}, sum_0 {0};
	for (int i = 0; i < dst.rows * dst.cols; ++i)
	{
		if (dst.data[i] == 0)
			++sum_0;
		else
			++sum_255;
	}

	// Choose a frame type
	if (static_cast<double>(sum_255) / (sum_0 + sum_255) > m_frameTypeRatio)
	{
		m_frameType = eFrameType::Source;
	}
	else
	{
		m_frameType = eFrameType::Differential;
	}

	// Get execution time
	m_finishTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	                   std::chrono::system_clock::now().time_since_epoch()).count();

	// Adjust parameters
	adjustParameters();

	return 0;

	}
	catch (std::exception& obj)
	{
		PRINT_ERR("Exception (%s) during call OpenCV functions has been occured", obj.what());
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------
void CvCanny::adjustParameters()
{
	// Adjust a threshold for Canny detector
	if (m_finishTime - m_startTime < m_lowExecutionTime)
	{
		if (m_cannyThreshold > lowCannyThreshold)
			m_cannyThreshold -= m_stepCannyThreshold;
	}
	else if (m_finishTime - m_startTime > m_highExecutionTime)
	{
		if (m_cannyThreshold < highCannyThreshold)
			m_cannyThreshold += m_stepCannyThreshold;
	}
}
