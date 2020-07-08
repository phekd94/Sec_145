
#include "Learning.h"

#include <cmath>         // std::abs(), std::pow()
#include <vector>        // std::vector template class
#include <QImage>        // QImage class
#include <QDir>          // QDir class
#include <QFile>         // QFile class
#include <algorithm>     // std::transform() template function
#include "Algorithms.h"  // convertPixelLimits lambda function, dotProduct() function
#include "Eigen/Dense"   // Eigen::MatrixXd class
#include <random>        // PRNG
#include <ctime>         // time() function

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

// Neural network learning (1 layer)
int32_t neuralLearning_1_layer(const std::vector<QImage>& images,
                               const uint32_t imagesWidth,
                               const uint32_t imagesHeight,
                               const QString& resultPath,
                               const QString& resultName,
                               const uint32_t numCycle);

// Neural network learning (2 layer)
int32_t neuralLearning_2_layer(const std::vector<QImage>& images,
                               const std::vector<std::vector<uint32_t>>& labels,
                               const uint32_t imagesWidth,
                               const uint32_t imagesHeight,
                               const QString& resultPath,
                               const QString& resultName,
                               uint32_t numCycle);

// ReLU activation function
// auto ReLU = [](double node){ return (node > 0) * node; };
//double ReLU(double node) { return 5;/*(node > 0) * node;*/ }

template <typename Array>
void ReLU(Array* const array, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		Array& element = array[i];
		element = (element > 0) * element;
	}
}

//-------------------------------------------------------------------------------------------------
template <typename Array>
void ReLU_derivative(Array* const array, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		array[i] = array[i] > 0;
	}
}

//-------------------------------------------------------------------------------------------------
// Preface in debug message
const char* const PREF = "[Learning]: ";

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
	for (const auto & image : images) {

		// Get image bits
		uint8_t* data = const_cast<uint8_t*>(image.bits());

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
	       QImage::Format_Grayscale8).save(resultPath + "/" + resultName + ".bmp");

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t neuralLearning_1_layer(const std::vector<QImage>& images,
                               const uint32_t imagesWidth,
                               const uint32_t imagesHeight,
                               const QString& resultPath,
                               const QString& resultName,
                               const uint32_t numCycle)
{
	// Goal prediction
	const double goal_prediction = 1;

	// Alpha coefficient
	const double alpha = 0.01;

	// Max weight for catch a divergence
	const double max_weight = 1000;

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

	// Cycle with iterations of learning
	for (uint32_t i = 0; i < numCycle; ++i) {

	// Loop for all images
	for (const auto & image : images) {

		// Get an image data
		uint8_t* data = const_cast<uint8_t*>(image.bits());

		// Converse an image data in input data
		std::vector<double> input(&data[0], &data[imagesWidth * imagesHeight]);
		std::transform(input.begin(), input.end(), input.begin(), convertPixelLimits);

		// Get a prediction and save it in file
		double prediction = dotProduct(input, weights);
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
		for (uint32_t j = 0; j < imagesWidth * imagesHeight; ++j) {

			// Weight correction
			weights[j] -= alpha * delta * input[j];

			// Check weights for divergence
			if (std::fabs(weights[j]) > max_weight) {
				PRINT_ERR(true, PREF, "divergence");
				return -1;
			}
		}

	} // Loop for all images

	PRINT_DBG(true, PREF, "End of %lu iteration", static_cast<unsigned long>(i + 1));

	} // Cycle with iterations of learning

	// Save weights (result)
	for (auto weight : weights) {
		if (f_w.write(QByteArray::number(weight) + "\n") == -1) {
			PRINT_ERR(true, PREF, "f_w.write() error");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t neuralLearning_2_layer(const std::vector<QImage>& images,
                               const std::vector<std::vector<uint32_t>>& labels,
                               const uint32_t imagesWidth,
                               const uint32_t imagesHeight,
                               const QString& resultPath,
                               const QString& resultName,
                               uint32_t numCycle)
{
	// Alpha coefficient
	const double alpha = 0.00005;

	// Max weight for catch a divergence
	const double max_weight = 1000;

	// Number of intermediate layers
	const uint32_t hidden_size = 40;

	// Number of labels
	const uint32_t num_labels = labels[0].size();



	// Weights
	Eigen::MatrixXd w_0_1(imagesWidth * imagesHeight, hidden_size);
	Eigen::MatrixXd w_1_2(hidden_size, num_labels);

	// Fill ...
	std::mt19937 gen(time(0));
	std::uniform_int_distribution<int> uid(0, 1000);
	auto data_w_0_1 = w_0_1.data(); // access only across '*' or '[]'
	for (uint32_t i = 0; i < w_0_1.size(); ++i) {
		data_w_0_1[i] = 0.2 * uid(gen) / 1000.0 - 0.1;
	}
	auto data_w_1_2 = w_1_2.data(); // access only across '*' or '[]'
	for (uint32_t i = 0; i < w_1_2.size(); ++i) {
		data_w_1_2[i] = 0.2 * uid(gen) / 1000.0 - 0.1;
	}

	qDebug() << w_0_1(5, 5) << w_1_2(0, 0);
	qDebug() << w_0_1.rows() << w_0_1.cols() << w_0_1.size();
	qDebug() << w_1_2.rows() << w_1_2.cols();

	Eigen::MatrixXd l_0(1, imagesWidth * imagesHeight);
	Eigen::MatrixXd l_1(1, hidden_size);
	Eigen::MatrixXd l_2(1, num_labels);
	Eigen::MatrixXd l_2_delta(1, num_labels);
	Eigen::MatrixXd l_1_delta(1, hidden_size);

	double error = 0;


	// ... iterations loop ...
	while (numCycle-- > 0) {

	error = 0;

	// ... image loop  ...
	for (auto image : images) {
	//const QImage& image = images[0];

	// if (image.isnull and image.size
	auto l_0_data = l_0.data(); // access only across '*' or '[]'
	auto image_data = image.bits();
	for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
		l_0_data[i] = image_data[i] / 255.0;
	}


	l_1 = l_0 * w_0_1;
	//qDebug() << "*";

	ReLU(l_1.data(), l_1.size());
	//qDebug() << "ReLU";

	/*auto data_l_1 = l_1.data();
	for (uint32_t i = 0; i < l_1.size(); ++i) {
		qDebug() << data_l_1[i];
	}*/

	l_2 = l_1 * w_1_2;
	//qDebug() << "*";

	/*auto data_l_2 = l_2.data();
	for (uint32_t i = 0; i < l_2.size(); ++i) {
		qDebug() << data_l_2[i];
	}*/

	Eigen::MatrixXd label(1, num_labels);
	for (uint32_t i = 0; i < l_2.size(); ++i) {
		label(0, i) = labels[0][i];   ///////////     0000000000
		//qDebug() << label(0, i);
	}
	error += ((label - l_2) * (label - l_2).transpose()).sum();
	//qDebug() << error;

	// correct
	// ...

	l_2_delta = label - l_2;
	//qDebug() << "delta 2";

	/*auto data_l_2_delta = l_2_delta.data();
	for (uint32_t i = 0; i < l_2_delta.size(); ++i) {
		qDebug() << data_l_2_delta[i];
	}*/

	l_1_delta = l_2_delta * w_1_2.transpose();
	//qDebug() << "delta 1";

	ReLU_derivative(l_1_delta.data(), l_1_delta.size());

	/*auto data_l_1_delta = l_1_delta.data();
	for (uint32_t i = 0; i < l_1_delta.size(); ++i) {
		qDebug() << data_l_1_delta[i];
	}*/

	w_1_2 += alpha * l_1.transpose() * l_2_delta;

	/*auto w_1_2_delta = w_1_2.data();
	for (uint32_t i = 0; i < w_1_2.size(); ++i) {
		qDebug() << w_1_2_delta[i];
	}*/

	w_0_1 += alpha * l_0.transpose() * l_1_delta;

	/*auto w_0_1_delta = w_0_1.data();
	for (uint32_t i = 0; i < w_0_1.size(); ++i) {
		qDebug() << w_0_1_delta[i];
	}*/

	}
	qDebug() << error;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPath,
                      const QString& resultName,
                      const LearningType type,
                      const bool scale,
                      const uint32_t numCycle,
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

//	for (auto file : entries)
//		qDebug() << file.fileName();
//	return 0;


	std::vector<std::vector<uint32_t>> labels(entries.size(), std::vector<uint32_t>(2, 0));
	for (uint32_t i = 0; i < labels.size(); ++i) {
		if (i % 2 == 0) {
			labels[i][0] = 1;
		} else {
			labels[i][1] = 1;
		}
		//qDebug() << labels[i][0] << labels[i][1];
	}


	// Get images
	std::vector<QImage> images;
	for (const auto & file : entries) {

		// Scale an image (if need)
		if (true == scale) {
			// Get an image
			QImage image = QImage(file.filePath());

			image.convertTo(QImage::Format_Grayscale8);

			// Leave only 0 and 255 pixels
			/*uint8_t* data = image.bits();
			for (uint32_t i = 0; i < image.sizeInBytes(); ++i)
				if (data[i] < 255)
					data[i] = 0;*/

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

	//images[5].save("C:/Users/ekd/Desktop/222.bmp");


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
		if (/*neuralLearning_1_layer*/neuralLearning_2_layer(images, labels,
		                           imagesWidth, imagesHeight,
		                           resultPath, resultName,
		                           numCycle) != 0) {
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
} // namespace Sec_145
