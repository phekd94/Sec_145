
#include "CvCanny.h"

#include "opencv2/core/mat.hpp"  // ss
#include "opencv2/imgproc.hpp"

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* const CvCanny::PREF = "[CvCanny]: ";

//-------------------------------------------------------------------------------------------------
CvCanny::CvCanny(const uint32_t threshold, const uint32_t kernelSize) : m_threshold(threshold),
                                                                        m_kernelSize(kernelSize)
{
	PRINT_DBG(DisjointSet<CvPoint>::m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
int32_t CvCanny::applyDetector(QImage image)
{
	image.convertTo(QImage::Format_Grayscale8);

	cv::Mat im(image.height(), image.width(), CV_8UC1, image.bits());
	cv::Mat im_d;

	im.copyTo(im_d);

	cv::blur(im_d, im_d, cv::Size(3, 3));
	cv::Canny(im_d, im_d, m_threshold, 3 * m_threshold, m_kernelSize);

	findContours(im_d, m_contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	//qDebug() << m_contours.size();

	for (int i = 0; i < m_contours.size(); ++i) {
		//std::cout << contours[i].size() << std::endl;
		for (int j = 0; j < m_contours[i].size(); ++j) {
			//std::cout << contours[i][j].x << "  " << contours[i][j].y << ", ";
			if (DisjointSet<CvPoint>::addMember(Sec_145::CvPoint(m_contours[i][j])) != 0)
			{
				PRINT_ERR(true, "main", "addMember");
			}
		}
		//std::cout << std::endl;
	}

	//qDebug() << DisjointSet<CvPoint>::getDisjointSet().size();

	return 0;
}
