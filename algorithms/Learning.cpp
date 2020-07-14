
#include "Learning.h"

#include <cmath>         // std::abs(), std::pow(); std::tanh(); std::exp()
#include <vector>        // std::vector<> class
#include <QImage>        // QImage class
#include <QDir>          // QDir class
#include <QFile>         // QFile class
#include <algorithm>     // std::transform<>()
#include "Algorithms.h"  // convertPixelLimits[](), dotProduct(), sumArrayExpElements<>()
#include "Eigen/Dense"   // Eigen::MatrixXd class
#include <random>        // PRNG
#include <ctime>         // time()
#include <utility>       // std::pair<>
#include <float.h>       // DBL_MIN
#include <QByteArray>    // QByteArray class


#include <QThread>

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// Simple learning
[[deprecated("This learning method gives inaccurate results")]]
int32_t simpleLearning(const std::vector<QImage>& images,
                       const bool brightness,
                       const uint32_t imagesWidth,
                       const uint32_t imagesHeight,
                       const QString& resultPath,
                       const QString& resultName);

//-------------------------------------------------------------------------------------------------
// Neural network learning (1 layer)
[[deprecated("This learning method gives inaccurate results")]]
int32_t neuralLearning_1_layer(const std::vector<QImage>& images,
                               const uint32_t imagesWidth,
                               const uint32_t imagesHeight,
                               const QString& resultPath,
                               const QString& resultName,
                               const uint32_t numCycle);

//-------------------------------------------------------------------------------------------------
// Neural network learning (2 layer)
int32_t neuralLearning_2_layer(
              const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
              const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels_test,
              const uint32_t imagesWidth,
              const uint32_t imagesHeight,
              const QString& resultPath,
              const QString& resultName,
              const uint32_t numIterations,
              const bool test);

//-------------------------------------------------------------------------------------------------
// Fills the matrix with weights of random numbers
//  test == false: result = a * random + b
//  test == true : result = from file
int32_t fillWeights(Eigen::MatrixXd& matrix,
                    const double a, const double b,
                    const bool test = false, const QString& name = "");

//-------------------------------------------------------------------------------------------------
// Gets a bernoulli distribution vector
int32_t getB_D(std::vector<bool>& vector, double p, const bool test = false);

//-------------------------------------------------------------------------------------------------
// Get images and labels from specified path
int32_t getImagesAndLabels(std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
                           const QString& pathToImages,
                           const uint32_t numCopters,
                           const uint32_t imagesWidth,
                           const uint32_t imagesHeight);

//-------------------------------------------------------------------------------------------------
// ReLU activation function
template <typename DataType>
void ReLU(DataType* const data, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		DataType& element = data[i];
		element = (element > 0) * element;
	}
}

//-------------------------------------------------------------------------------------------------
// Derivative of ReLU activation function
template <typename DataType>
void ReLU_derivative(DataType* const data, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		data[i] = data[i] > 0;
	}
}

//-------------------------------------------------------------------------------------------------
// Hyperbolic tangent activation function
template <typename DataType>
void tanh(DataType* const data, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		data[i] = std::tanh(data[i]);
	}
}

//-------------------------------------------------------------------------------------------------
// Derivative of hyperbolic tangent activation function
template <typename DataType>
void tanh_derivative(DataType* const data, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i) {
		data[i] = 1 - data[i] * data[i];
	}
}

//-------------------------------------------------------------------------------------------------
// Softmax activation function
template <typename DataType>
void softmax(DataType* const data, uint32_t size)
{
	double sum = sumArrayExpElements(data, size);
	for (uint32_t i = 0; i < size; ++i) {
		data[i] = std::exp(data[i]) / sum;
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
int32_t neuralLearning_2_layer(
        const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
        const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels_test,
        const uint32_t imagesWidth,
        const uint32_t imagesHeight,
        const QString& resultPath,
        const QString& resultName,
        const uint32_t numIterations,
        const bool test)
{
	// Debug frequency multiplier
	const uint32_t debug_f_mul = 1;

	// Alpha coefficient
	const double alpha = 0.00025;

	// Max weight and error for catch a divergence
	const double max_weight = 1000;
	const double max_error = 1000;

	// Number of intermediate layers
	const uint32_t hidden_size = 100;

	// Number of labels
	const uint32_t num_labels = static_cast<uint32_t>(images_labels.front().second.size());

	// Batch size
	const uint32_t batch_size = 15; // 4

	// Weights
	Eigen::MatrixXd w_0_1(imagesWidth * imagesHeight, hidden_size);
	Eigen::MatrixXd w_1_2(hidden_size, num_labels);

	// Fill the matrix with weights of random numbers
	if (fillWeights(w_0_1, 0.02, 0.01, test, "w_0_1") != 0) {
		PRINT_ERR(true, PREF, "Can't fill w_0_1");
		return -1;
	}
	if (fillWeights(w_1_2, 0.2, 0.1, test, "w_1_2") != 0) {
		PRINT_ERR(true, PREF, "Can't fill w_1_2");
		return -1;
	}

	// Layers and deltas for layers
	Eigen::MatrixXd l_0(1, imagesWidth * imagesHeight);
	Eigen::MatrixXd l_1(1, hidden_size);
	Eigen::MatrixXd l_2(1, num_labels);
	Eigen::MatrixXd l_2_delta(1, num_labels);
	Eigen::MatrixXd l_1_delta(1, hidden_size);

	// Error
	double error;

	// Correct counter
	uint32_t correct_cnt;

	// Iterations loop
	for (uint32_t it = 0; it < numIterations; ++it) {

		// Set error and correct counter to 0
		error = correct_cnt = 0;

		// Images loop
		for (const auto& image_label : images_labels) {

			// Check image
			if (   image_label.first.isNull() == true
			    || image_label.first.sizeInBytes() != imagesWidth * imagesHeight) {
				PRINT_ERR(true, PREF, "Bad image");
				return -1;
			}

			// Get image (layer 0)
			auto image_data = image_label.first.bits();
			for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
				l_0(0, i) = image_data[i] / 255.0;
			}

			// Calculate an layer 1
			l_1 = l_0 * w_0_1;

			// Apply activation function to the layer 1 (hidden layer)
			// ReLU
			// ReLU(l_1.data(), l_1.size());
			// Hyperbolic tangent
			tanh(l_1.data(), l_1.size());

			// Dropout a some neurons in layer 1
			std::vector<bool> dropout_mask(l_1.size());
			if (getB_D(dropout_mask, 0.5, test) != 0) {
				PRINT_ERR(true, PREF, "Can't get bernoulli distribution vector");
				return -1;
			}
			for (uint32_t i = 0; i < l_1.size(); ++i) {
				l_1(0, i) *= dropout_mask[i] * 2;
			}

			// Calculate an layer 2
			l_2 = l_1 * w_1_2;

			// Apply activation function to the layer 2 (output layer)
			softmax(l_2.data(), l_2.size());

			// Get a label
			Eigen::MatrixXd label(1, num_labels);
			for (uint32_t i = 0; i < num_labels; ++i) {
				label(0, i) = image_label.second[i];
			}

			// Calculate an error
			error += ((label - l_2) * (label - l_2).transpose()).sum();

			// Add to correct counter
			if (   maxArrayElementIndex(l_2.data(), l_2.size())
			    == maxArrayElementIndex(label.data(), label.size())) {
				++correct_cnt;
			}

			// Calculate a delta for layer 2
			l_2_delta = label - l_2;

			// Calculate a delta for layer 1
			l_1_delta = l_2_delta * w_1_2.transpose();

			// Apply derivative of activation function
			// ReLU
			// ReLU_derivative(l_1_delta.data(), l_1_delta.size());
			// Hyperbolic tangent
			tanh_derivative(l_1.data(), l_1.size());
			for (uint32_t i = 0; i < l_1.size(); ++i) {
				l_1_delta(0, i) *= l_1(0, i);
			}

			// Dropout the same neurons as in layer 1
			for (uint32_t i = 0; i < l_1_delta.size(); ++i) {
				l_1_delta(0, i) *= dropout_mask[i];
			}

			// Correct weights
			w_1_2 += alpha * l_1.transpose() * l_2_delta;
			w_0_1 += alpha * l_0.transpose() * l_1_delta;

			if (   error > max_error
			    || maxArrayElement(w_1_2.data(), w_1_2.size()) > max_weight
			    || maxArrayElement(w_0_1.data(), w_0_1.size()) > max_weight) {
				PRINT_ERR(true, PREF, "divergence");
				return -1;
			}

		} // Images loop

		// Print results of training
		if (it % debug_f_mul == 0) {
			PRINT_DBG(true, PREF,
			          "Iteration: %lu;  Training correct: %f;  Training error: %f",
			          static_cast<unsigned long>(it),
			          static_cast<double>(correct_cnt) / images_labels.size(),
			          error / images_labels.size());
		}

		// Set error and correct counter to 0
		error = correct_cnt = 0;

		// Test images loop
		for (const auto& image_label : images_labels_test) {

			// Check image
			if (   image_label.first.isNull() == true
			    || image_label.first.sizeInBytes() != imagesWidth * imagesHeight) {
				PRINT_ERR(true, PREF, "Bad test image");
				return -1;
			}

			// Get image (layer 0)
			auto image_data = image_label.first.bits();
			for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
				l_0(0, i) = image_data[i] / 255.0;
			}

			// Calculate an layer 1
			l_1 = l_0 * w_0_1;

			// Apply activation function to the layer 1 (hidden layer)
			// ReLU
			// ReLU(l_1.data(), l_1.size());
			// Hyperbolic tangent
			tanh(l_1.data(), l_1.size());

			// Calculate an layer 2
			l_2 = l_1 * w_1_2;

			// Get a label
			Eigen::MatrixXd label(1, num_labels);
			for (uint32_t i = 0; i < num_labels; ++i) {
				label(0, i) = image_label.second[i];
			}

			// Calculate an error
			error += ((label - l_2) * (label - l_2).transpose()).sum();

			// Add to correct counter
			if (   maxArrayElementIndex(l_2.data(), l_2.size())
			    == maxArrayElementIndex(label.data(), label.size())) {
				++correct_cnt;
			}

		} // Test images loop

		// Print results of test
		if (it % debug_f_mul == 0) {
			PRINT_DBG(true, PREF,
			          "Iteration: %lu;  Test correct: %f;  Test error: %f",
			          static_cast<unsigned long>(it),
			          static_cast<double>(correct_cnt) / images_labels_test.size(),
			          error / images_labels_test.size());
			PRINT_DBG(true, PREF, "");
		}

	} // Iterations loop

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t fillWeights(Eigen::MatrixXd& matrix,
                    const double a, const double b,
                    const bool test, const QString& name)
{
	if (test == true) {
		QFile file("C:/Users/ekd/Documents/deep_learning/images/learn/random_n/" + name + ".txt");
		if (file.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open file %s",
			          file.fileName().toStdString().c_str());
			return -1;
		}
		for (uint32_t i = 0; i < matrix.rows(); ++i) {
			for (uint32_t j = 0; j < matrix.cols(); ++j) {
				QByteArray ar = file.readLine();
				if (ar.isEmpty() == true) {
					PRINT_ERR(true, PREF, "File %s is small",
					          file.fileName().toStdString().c_str());
					return -1;
				}
				bool ok;
				matrix(i, j) = ar.toDouble(&ok);
				if (ok == false) {
					PRINT_ERR(true, PREF, "Bad content of file %s",
					          file.fileName().toStdString().c_str());
					return -1;
				}
			}
		}
	} else {
		std::mt19937 gen(time(0));
		std::uniform_int_distribution<int> uid(0, 10000);
		auto data_matrix = matrix.data();
		for (uint32_t i = 0; i < matrix.size(); ++i) {
			data_matrix[i] = a * uid(gen) / 10000.0 - b;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t getB_D(std::vector<bool>& vector, double p, const bool test)
{
	if (test == true) {
		QFile file("C:/Users/ekd/Documents/deep_learning/images/learn/random_n/b_d.txt");
		if (file.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open file %s",
			          file.fileName().toStdString().c_str());
			return -1;
		}
		for (uint32_t i = 0; i < vector.size(); ++i) {
			QByteArray ar = file.readLine();
			if (ar.isEmpty() == true) {
				PRINT_ERR(true, PREF, "File %s is small",
				          file.fileName().toStdString().c_str());
				return -1;
			}
			bool ok;
			vector[i] = ar.toUInt(&ok);
			if (ok == false) {
				PRINT_ERR(true, PREF, "Bad content of file %s",
				          file.fileName().toStdString().c_str());
				return -1;
			}
		}
	} else {
		std::mt19937 gen(time(0));
		std::bernoulli_distribution b_d(p);
		for (uint32_t i = 0; i < vector.size(); ++i) {
			vector[i] = b_d(gen);
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t getImagesAndLabels(std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
                           const QString& pathToImages,
                           const uint32_t numCopters,
                           const uint32_t imagesWidth,
                           const uint32_t imagesHeight)
{
	// Create a directory class object and check it exist
	QDir dir(pathToImages);
	if (dir.exists() == false) {
		PRINT_ERR(true, PREF, "Directory is not exist");
		return -1;
	}

	// Get an folder list and check it
	QFileInfoList folders_list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
	if (static_cast<uint32_t>(folders_list.size()) != numCopters) {
		PRINT_ERR(true, PREF, "Number of folders is not equal to number of copters");
		return -1;
	}

	// Get image files lists
	std::vector<QFileInfoList> files_lists(numCopters);
	for (uint32_t i = 0; i < numCopters; ++i) {
		qDebug() << folders_list[i].filePath();
		QDir dir(folders_list[i].filePath());
		files_lists[i] = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
		if (files_lists[i].size() != files_lists.front().size()) {
			PRINT_ERR(true, PREF, "Different number of files with copters");
			return -1;
		}
	}

	// Get images and labels
	for (uint32_t i = 0; i < static_cast<uint32_t>(files_lists.front().size()); ++i) {

		// Get an image from each folder
		for (uint32_t j = 0; j < numCopters; ++j) {

			// Get an image and covert it
			QImage image = QImage(files_lists[j][i].filePath());
			image.convertTo(QImage::Format_Grayscale8);

			// Create a label
			std::vector<uint32_t> label(numCopters, 0);
			label[j] = 1;

			// Add image and label
			images_labels.push_back(
			            std::pair<QImage, std::vector<uint32_t>>(
			                image.scaled(imagesWidth, imagesHeight), label));
		}
	}
	if (images_labels.size() == 0) {
		PRINT_ERR(true, PREF, "Container for images and labels is empty");
		return -1;
	}

	PRINT_DBG(true, PREF, "%lu images from %lu folders were loaded",
	          static_cast<unsigned long>(files_lists.front().size()),
	          static_cast<unsigned long>(numCopters));

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t modelLearning(const QString& pathToImages,
                      const QString& pathToImagesTest,
                      const QString& resultPath,
                      const QString& resultName,
                      const LearningType type,
                      const uint32_t numIterations,
                      const uint32_t numCopters,
                      const uint32_t imagesWidth,
                      const uint32_t imagesHeight,
                      const bool test)
{
	// Containers for images, labels and test
	std::vector<std::pair<QImage, std::vector<uint32_t>>> images_labels;
	std::vector<std::pair<QImage, std::vector<uint32_t>>> images_labels_test;

	// Get containers
	if (getImagesAndLabels(images_labels, pathToImages, numCopters,
	                       imagesWidth, imagesHeight) != 0) {
		PRINT_ERR(true, PREF, "getImagesAndLabels(images_labels)");
		return -1;
	}
	if (getImagesAndLabels(images_labels_test, pathToImagesTest, numCopters,
	                       imagesWidth, imagesHeight) != 0) {
		PRINT_ERR(true, PREF, "getImagesAndLabels(images_labels_test)");
		return -1;
	}

	// Apply a learning method
	switch (type) {
	case LearningType::Neural_2_layer:
		if (neuralLearning_2_layer(images_labels, images_labels_test,
		                           imagesWidth, imagesHeight,
		                           resultPath, resultName,
		                           numIterations,
		                           test) != 0) {
			PRINT_ERR(true, PREF, "neuralLearning_2_layer() is not successful");
			return -1;
		} else {
			PRINT_DBG(true, PREF, "neuralLearning_2_layer() is successful");
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
