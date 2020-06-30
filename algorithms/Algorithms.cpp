
#include "Algorithms.h"

#include <cmath>   // std::sqrt(), std::abs(), std::pow()
#include <vector>  // std::vector template class
#include <QImage>  // QImage class
#include <QDir>    // QDir class
#include <QFile>   // QFile class

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// Simple learning
int32_t simpleLearning(const std::vector<QImage>& images,
                       const bool brightness,
                       const uint32_t imagesWidth,
                       const uint32_t imagesHeight,
                       const QString& resultPath,
                       const QString& resultName);

// Neural network learning
int32_t neuralLearning(const std::vector<QImage>& images,
                       const uint32_t imagesWidth,
                       const uint32_t imagesHeight,
                       const QString& resultPath,
                       const QString& resultName);

//-------------------------------------------------------------------------------------------------
#define PREF  "[Algorithms]: "

//-------------------------------------------------------------------------------------------------
int32_t simpleLearning(const std::vector<QImage>& images,
                       const bool brightness,
                       const uint32_t imagesWidth,
                       const uint32_t imagesHeight,
                       const QString& resultPath,
                       const QString& resultName)
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

	// Save a result
	QImage(res_uint8.data(), imagesWidth, imagesHeight,
	       QImage::Format_Grayscale8).save(resultPath + "/" + resultName);

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t neuralLearning(const std::vector<QImage>& images,
                       const uint32_t imagesWidth,
                       const uint32_t imagesHeight,
                       const QString& resultPath,
                       const QString& resultName)
{
  #define MAX_WEIGHT  1000

	// Goal prediction
	double goal_prediction = 1;

	// Alpha coefficient
	double alpha = 0.01;

	// Weights (result)
	std::vector<double> weights(imagesWidth * imagesHeight, 0);

	// Create and open files
	QFile f_w(resultPath + "/" + resultName + ".txt");
	QFile f_p(resultPath + "/" + resultName + "_p.txt");
	QFile f_e(resultPath + "/" + resultName + "_e.txt");
	if (   f_w.open(QIODevice::WriteOnly) == false
	    || f_p.open(QIODevice::WriteOnly) == false
	    || f_e.open(QIODevice::WriteOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open one of the files in WriteOnly mode");
		return -1;
	}

	// Loop for all images
	for (auto image : images) {

		// Get an image data
		uint8_t* data = image.bits();

		// Converse an image data in input data
		std::vector<double> input(&data[0], &data[imagesWidth * imagesHeight]);
		for (auto & pixel : input)
			pixel /= 255;

		// Get a prediction and save it in file
		double prediction = dotProduct(input.data(), weights.data(), imagesWidth * imagesHeight);
		if (f_p.write(QByteArray::number(prediction) + "\n") == -1) {
			PRINT_ERR(true, PREF, "f_p.write() error");
			return -1;
		}

		// Calculate a delta
		double delta = prediction - goal_prediction;

		// Calculate an error and save it in file
		double error = std::pow(goal_prediction - prediction, 2);
		if (f_e.write(QByteArray::number(error) + "\n") == -1) {
			PRINT_ERR(true, PREF, "f_e.write() error");
			return -1;
		}

		// Weights corrections
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {

			// Weight correction
			weights[i] -= alpha * delta * input[i];

			if (std::fabs(weights[i]) > MAX_WEIGHT) {
				PRINT_ERR(true, PREF, "divergence");
				return -1;
			}
		}
	}

	// Save weights (result)
	for (auto weight : weights) {
		if (f_w.write(QByteArray::number(weight) + "\n") == -1) {
			PRINT_ERR(true, PREF, "f_w.write() error");
			return -1;
		}
	}

	return 0;

  #undef MAX_WEIGHT
}

//-------------------------------------------------------------------------------------------------
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPath,
                      const QString& resultName,
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
	switch (type) {
	case LearningType::Simple:
		if (simpleLearning(images, brightness,
		                   imagesWidth, imagesHeight,
		                   resultPath, resultName) != 0) {
			PRINT_ERR(true, PREF, "simpleLearning() is not successful");
			return -1;
		} else {
			PRINT_DBG(true, PREF, "simpleLearning() is successful");
		}
		break;
	case LearningType::Neural:
		if (neuralLearning(images, imagesWidth, imagesHeight, resultPath, resultName) != 0) {
			PRINT_ERR(true, PREF, "neuralLearning() is not successful");
			return -1;
		} else {
			PRINT_DBG(true, PREF, "neuralLearning() is successful");
		}
		break;
	default:
		PRINT_ERR(true, PREF, "Unknown learning type");
		return -1;
		break;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
double dotProduct(const double* const d_1, const double* const d_2, const uint32_t n)
{
	// Check the incoming parameters
	if (nullptr == d_1 || nullptr == d_2) {
		PRINT_ERR(true, PREF, "nullptr == d_1 or nullptr == d_2");
		return 0;
	}

	// Calculate a dot product
	double output = 0;
	for (uint32_t i = 0; i < n; ++i) {
		output += d_1[i] * d_2[i];
	}
	return output;
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
