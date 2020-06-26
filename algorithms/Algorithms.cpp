
#include "Algorithms.h"

#include <cmath>   // std::sqrt(), std::abs()
#include <vector>  // std::vector template class
#include <QImage>  // QImage class
#include <QDir>    // QDir class

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
#define PREF  "[Algorithms]: "

//-------------------------------------------------------------------------------------------------
const std::vector<uint8_t> simpleLearning(const std::vector<QImage>& images,
                                          const bool brightness,
                                          const uint32_t imagesWidth,
                                          const uint32_t imagesHeight)
{
	// Get a brightness
	double b = 255.0 / (imagesWidth * imagesHeight);

	// Result (double)
	std::vector<double> res_double(imagesWidth * imagesHeight, 0);

	// Get a result (double)
	// Images loop
	for (auto image : images) {

		// Get image bits
		uint8_t* data = image.bits();

		// Pixels loop
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {

			// Process data
			if (data[i] == 255) {
				res_double[i] += b;
			}
		}
	}

	// Get a result (uint8_t)
	std::vector<uint8_t> res_uint8(imagesWidth * imagesHeight);
	for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
		res_uint8[i] = res_double[i];
	}

	// Increase a brightness (if need)
	if (true == brightness) {
		uint8_t max_value = *std::max_element(res_uint8.begin(), res_uint8.end());
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
			if (res_uint8[i] != 0) {
				res_uint8[i] += (255 - max_value);
			}
		}
	}

	return res_uint8;
}

//-------------------------------------------------------------------------------------------------
const std::vector<uint8_t> neuralLearning(const std::vector<QImage>& images,
                                          const uint32_t imagesWidth,
                                          const uint32_t imagesHeight)
{
	double goal_pred = 0.8;

	std::vector<double> weigths(imagesWidth * imagesHeight, 0);
	double pred;

	for (auto image : images) {

		// Get data and process it
		uint8_t* data = image.bits();

		// Pixels loop
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {

			pred = data[i] * weigths[i];
			double delta = pred - goal_pred;
			double weigth_delta = delta * data[i];
			weigths[i] -= (weigth_delta * 0.0000320); // alpha
		}

	}

	// Get a result (uint8_t)
	std::vector<uint8_t> res_uint8(imagesWidth * imagesHeight);
	for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {

		weigths[i] = std::fabs(weigths[i]);

		if (weigths[i] > 1) {
			weigths[i] = 1;
			PRINT_ERR(true, PREF, "%d", i);
		}

		res_uint8[i] = weigths[i] * 255;

		// PRINT_DBG(true, PREF, "%f", weigths[i]);

	}

	// PRINT_DBG(true, PREF, "%f", weigths[0]);

	return res_uint8;
}

//-------------------------------------------------------------------------------------------------
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPathAndName,
                      const LearningType type,
                      const bool scale,
                      const bool brightness,
                      const uint32_t imagesWidth,
                      const uint32_t imagesHeight)
{
	// Create a directory class object and check it exist
	QDir dir(pathCopterImages);
	if (dir.exists() == false) {
		PRINT_ERR(true, PREF, "Directory is not exist");
		return -1;
	}

	// Get an images list
	QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

	// Get images
	std::vector<QImage> images;
	for (auto file : entries) {

		// Scale an inmage (if need)
		if (true == scale) {
			// Get an image
			QImage image = QImage(file.filePath());

			// Leave only 0 and 255 pixels
			uint8_t* data = image.bits();
			for (uint32_t i = 0; i < image.sizeInBytes(); ++i)
				if (data[i] < 255)
					data[i] = 0;

			// Add an image to vector
			images.push_back(image.scaled(imagesWidth, imagesHeight));
		} else {

			// Add an image to vector
			images.push_back(QImage(file.filePath()));
		}

		// Check the size of image and entrance size
		if (   static_cast<uint32_t>(images.back().width()) != imagesWidth
		    || static_cast<uint32_t>(images.back().height()) != imagesHeight) {
			PRINT_ERR(true, PREF, "Size of the %s image is not correct",
			                      file.filePath().toStdString().c_str());
			return -1;
		}
	}

	// Apply a learning method to the vector with images; get a result
	std::vector<uint8_t> res;
	switch (type) {
	case LearningType::Simple:
		res = simpleLearning(images, brightness, imagesWidth, imagesHeight);
		break;
	case LearningType::Neural:
		res = neuralLearning(images, imagesWidth, imagesHeight);
		break;
	default:
		PRINT_ERR(true, PREF, "Unknown learning type");
		return -1;
		break;
	}

	// Save a result
	QImage(res.data(), imagesWidth, imagesHeight,
	       QImage::Format_Grayscale8).save(resultPathAndName);

	return 0;
}

//-------------------------------------------------------------------------------------------------
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n)
{
	double sum_d_1 = 0, sum_d_2 = 0, sum_d_1_d_2 = 0;
	double squareSum_d_1 = 0, squareSum_d_2 = 0;

	// Check the incoming parameters
	if (nullptr == d_1 || nullptr == d_2) {
		PRINT_ERR(true, PREF, "nullptr == d_1 or nullptr == d_2");
		return 0;
	}

	for (uint32_t i = 0; i < n; ++i)
	{
		// Sum of elements of array d_1
		sum_d_1 += d_1[i];

		// Sum of elements of array d_2
		sum_d_2 += d_2[i];

		// Sum of d_1[i] * d_2[i]
		sum_d_1_d_2 += d_1[i] * d_2[i];

		// Sum of square of array elements
		squareSum_d_1 += d_1[i] * d_1[i];
		squareSum_d_2 += d_2[i] * d_2[i];
	}

	// Use a formula for calculating correlation coefficient
	double res = (n * sum_d_1_d_2 - sum_d_1 * sum_d_2) /
	              std::sqrt((n * squareSum_d_1 - sum_d_1 * sum_d_1) *
	                        (n * squareSum_d_2 - sum_d_2 * sum_d_2));

	// Return an absolute value
	return std::abs(res);
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
