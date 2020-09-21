
#include "CvCanny.h"

#include <exception>             // std::exception
#include "opencv2/core/mat.hpp"  // cv::Mat
#include "opencv2/imgproc.hpp"   // cv::blur(); cv::Canny(); findContours()

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* const CvCanny::PREF {"[CvCanny]: "};

//-------------------------------------------------------------------------------------------------
int32_t CvCanny::applyDetector(const uint8_t* const greyData,
                               const uint32_t width, const uint32_t height) noexcept
{
	try {

	// Matrix with image data
	cv::Mat image_tmp(height, width, CV_8UC1, const_cast<uint8_t*>(greyData));
	cv::Mat image;

	// Get a matrix with image data
	image_tmp.copyTo(image);

	// Blur an image
	cv::blur(image, image, cv::Size(3, 3));

	// Apply Canny detector
	cv::Canny(image, image, m_threshold, 3 * m_threshold, m_kernelSize);

	// Add points to disjoint set
	/*
	for (int32_t row = 0; row >= 0 && row < image.rows; ++row)
	{
		for (int32_t col = 0; col >= 0 && col < image.cols; ++col)
		{
			if (image.at<uint8_t>(row, col) == 255)
			{
				if (DisjointSet<CvPoint>::addMember(Sec_145::CvPoint(cv::Point(col, row))) != 0)
				{
					PRINT_ERR(true, PREF, "addMember()");
					return -1;
				}
			}
		}
	}

	return 0;
	*/

	// Find contours (get a set of sets with contours points)
	cv::findContours(image, m_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	// Add points to disjoint set
	for (uint32_t i = 0; i < m_contours.size(); ++i)
	{
		for (uint32_t j = 0; j < m_contours[i].size(); ++j)
		{
			if (DisjointSet<CvPoint>::addMember(Sec_145::CvPoint(m_contours[i][j])) != 0)
			{
				PRINT_ERR(true, PREF, "addMember()");
				return -1;
			}
		}
	}

	return 0;

	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, PREF, "Exception (%s) during call OpenCV functions has been occured",
		          obj.what());
		return -1;
	}
}
