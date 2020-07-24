
#include "Learning.h"

#include <cmath>          // std::abs(), std::pow(); std::tanh(); std::exp()
#include <vector>         // std::vector<> class
#include <QImage>         // QImage class
#include <QDir>           // QDir class
#include <QFile>          // QFile class
#include <algorithm>      // std::transform<>()
#include <random>         // PRNG
#include <ctime>          // time()
#include <utility>        // std::pair<>
#include <QByteArray>     // QByteArray class
#include "other/Other.h"  // writeVarInFile<>()
#include "Algorithms.h"   // convertPixelLimits[](); dotProduct(); sumArrayExpElements<>();
                          // maxArrayElement<>(); maxArrayElementIndex<>()

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
                    const bool fromFile = false, const QString& name = "");

//-------------------------------------------------------------------------------------------------
// Gets a bernoulli distribution vector
int32_t getB_D(std::vector<bool>& vector, const double p, const bool fromFile = false);

//-------------------------------------------------------------------------------------------------
// Get images and labels from specified path
int32_t getImagesAndLabels(std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
                           const QString& pathToImages,
                           const uint32_t numCopters,
                           const uint32_t imagesWidth,
                           const uint32_t imagesHeight);

//-------------------------------------------------------------------------------------------------
// Gradient descent method
int32_t gradientDescent(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
            Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
            const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
            const uint32_t max_error, const uint32_t max_weight);

//-------------------------------------------------------------------------------------------------
// Batch gradient descent method
int32_t batch_gradientDescent(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
            const uint32_t batch_size,
            Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
            const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
            const uint32_t max_error, const uint32_t max_weight);

//-------------------------------------------------------------------------------------------------
// Test neural network weights
int32_t testWeights(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels_test,
            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const uint32_t hidden_size,
            const uint32_t imagesWidth, const uint32_t imagesHeight,
            const uint32_t max_error, const bool test = false);

//-------------------------------------------------------------------------------------------------
// Get recognition from neural network
int32_t getRecognitionLayer(const QImage& image,
                            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                            Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                            const uint32_t imagesWidth, const uint32_t imagesHeight);

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
			if (255 == data[i]) {
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
	const uint32_t debug_f_mul {1};

	// Alpha coefficient
	const double alpha {0.01};

	// Max weight and error for catch a divergence
	const double max_weight {1000};
	const double max_error {1000};

	// Kernels parameters
	uint32_t num_kernels {16};
	uint32_t kernel_rows {3};
	uint32_t kernel_cols {3};

	// Number of intermediate layers
	// const uint32_t hidden_size = 100;
	const uint32_t hidden_size {(imagesWidth - kernel_rows) *
		                        (imagesHeight - kernel_cols) *
		                        num_kernels};

	// Number of labels
	const uint32_t num_labels {static_cast<uint32_t>(images_labels.front().second.size())};

	// Batch size
	// const uint32_t batch_size {24};
	const uint32_t batch_size {1};

	// Weights
	// Eigen::MatrixXd w_0_1(imagesWidth * imagesHeight, hidden_size);
	Eigen::MatrixXd w_0_1(kernel_rows * kernel_cols, num_kernels); // kernels
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

	// Error
	double error;

	// Correct counter
	uint32_t correct_cnt;

	// Create and open files for results
	QFile f_w_0_1(resultPath + "/" + resultName + "_w_0_1.txt");
	QFile f_w_1_2(resultPath + "/" + resultName + "_w_1_2.txt");
	QFile f_params(resultPath + "/" + resultName + "_params.txt");
	QFile f_acc(resultPath + "/" + resultName + "_acc.txt");
	QFile f_err(resultPath + "/" + resultName + "_err.txt");
	QFile f_acc_test(resultPath + "/" + resultName + "_acc_test.txt");
	QFile f_err_test(resultPath + "/" + resultName + "_err_test.txt");
	if (   f_w_0_1.open(QIODevice::WriteOnly) == false
	    || f_w_1_2.open(QIODevice::WriteOnly) == false
	    || f_params.open(QIODevice::WriteOnly) == false
	    || f_acc.open(QIODevice::WriteOnly) == false
	    || f_err.open(QIODevice::WriteOnly) == false
	    || f_acc_test.open(QIODevice::WriteOnly) == false
	    || f_err_test.open(QIODevice::WriteOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open one of the files in WriteOnly mode");
		return -1;
	}

	// Iterations loop
	for (uint32_t it = 0; it < numIterations; ++it) {

		// Set an error and a correct counter to 0
		error = correct_cnt = 0;

		// Apply a gradient descent
		if (batch_gradientDescent(images_labels, batch_size, w_0_1, w_1_2, error, correct_cnt,
		                          num_labels, alpha, hidden_size, imagesWidth, imagesHeight,
		                          test, max_error, max_weight) != 0) {
			PRINT_ERR(true, PREF, "batch_gradientDescent()");
			return -1;
		}

		// Print results of training (if need)
		if (debug_f_mul != 0 && it % debug_f_mul == 0) {
			PRINT_DBG(true, PREF,
			          "Iteration: %lu;  Training correct: %f;  Training error: %f",
			          static_cast<unsigned long>(it),
			          static_cast<double>(correct_cnt) / images_labels.size(),
			          error / images_labels.size());
		}

		// Save an error and a correct counter for training
		if (writeVarInFile(f_err, error / images_labels.size()) != 0) {
			PRINT_ERR(true, PREF, "writeVarInFile(error)");
			return -1;
		}
		if (writeVarInFile(f_acc,
		                   static_cast<double>(correct_cnt) / images_labels.size()) != 0) {
			PRINT_ERR(true, PREF, "writeVarInFile(correct_cnt)");
			return -1;
		}

		// Set an error and a correct counter to 0
		error = correct_cnt = 0;

		// Test neural network weights
		if (testWeights(images_labels_test, w_0_1, w_1_2,
		                error, correct_cnt, num_labels, hidden_size,
		                imagesWidth, imagesHeight, max_error, test) != 0) {
			PRINT_ERR(true, PREF, "testWeights()");
			return -1;
		}

		// Print results of test
		if (debug_f_mul != 0 && it % debug_f_mul == 0) {
			PRINT_DBG(true, PREF,
			          "Iteration: %lu;  Test correct: %f;  Test error: %f",
			          static_cast<unsigned long>(it),
			          static_cast<double>(correct_cnt) / images_labels_test.size(),
			          error / images_labels_test.size());
			PRINT_DBG(true, PREF, "");
		}

		// Save an error and a correct counter for test
		if (writeVarInFile(f_err_test, error / images_labels_test.size()) != 0) {
			PRINT_ERR(true, PREF, "writeVarInFile(error for test)");
			return -1;
		}
		if (writeVarInFile(f_acc_test,
		                   static_cast<double>(correct_cnt) / images_labels_test.size()) != 0) {
			PRINT_ERR(true, PREF, "writeVarInFile(correct_cnt for test)");
			return -1;
		}

	} // Iterations loop

	// Save neural network parameters
	if (writeVarInFile(f_params, num_labels) != 0) {
		PRINT_ERR(true, PREF, "writeVarInFile(alpha)");
		return -1;
	}
	if (writeVarInFile(f_params, hidden_size) != 0) {
		PRINT_ERR(true, PREF, "writeVarInFile(hidden_size)");
		return -1;
	}

	// Save weights
	if (writeMatrixInFile(f_w_0_1, w_0_1) != 0) {
		PRINT_ERR(true, PREF, "writeMatrixInFile(w_0_1)");
		return -1;
	}
	if (writeMatrixInFile(f_w_1_2, w_1_2) != 0) {
		PRINT_ERR(true, PREF, "writeMatrixInFile(w_1_2)");
		return -1;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t fillWeights(Eigen::MatrixXd& matrix,
                    const double a, const double b,
                    const bool fromFile, const QString& name)
{
	if (true == fromFile) {
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
				if (false == ok) {
					PRINT_ERR(true, PREF, "Bad content of file %s",
					          file.fileName().toStdString().c_str());
					return -1;
				}
			}
		}
	} else {
		const double ker = 10000;
		std::mt19937 gen(time(0));
		std::uniform_int_distribution<int> uid(0, static_cast<int>(ker));
		auto data_matrix = matrix.data();
		for (uint32_t i = 0; i < matrix.size(); ++i) {
			data_matrix[i] = a * uid(gen) / ker - b;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t getB_D(std::vector<bool>& vector, const double p, const bool fromFile)
{
	if (true == fromFile) {
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
			if (false == ok) {
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
		PRINT_DBG(true, PREF, "Copter in \"%s\" folder has a label = %lu",
		          folders_list[i].filePath().toStdString().c_str(),
		          static_cast<unsigned long>(i));
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
			QImage image_scale = image.scaled(imagesWidth, imagesHeight);
			if (image_scale.isNull() == true) {
				PRINT_ERR(true, PREF, "Scaled image is null");
				return -1;
			}

			// Create a label
			std::vector<uint32_t> label(numCopters, 0);
			label[j] = 1;

			// Add image and label
			images_labels.push_back(
			            std::pair<QImage, std::vector<uint32_t>>(image_scale, label));
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
int32_t gradientDescent(
        const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
        Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
        double& error, uint32_t& correct_cnt,
        const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
        const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
        const uint32_t max_error, const uint32_t max_weight)
{
	// Layers and delta for layers
	Eigen::MatrixXd l_0(1, imagesWidth * imagesHeight);
	Eigen::MatrixXd l_1(1, hidden_size);
	Eigen::MatrixXd l_2(1, num_labels);
	Eigen::MatrixXd l_2_delta(1, num_labels);
	Eigen::MatrixXd l_1_delta(1, hidden_size);

	// Images loop
	for (const auto& image_label : images_labels) {

		// Check an image
		if (   image_label.first.isNull() == true
		    || image_label.first.sizeInBytes() != imagesWidth * imagesHeight) {
			PRINT_ERR(true, PREF, "Bad image");
			return -1;
		}

		// Get an image (layer 0)
		auto image_data = image_label.first.bits();
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
			l_0(0, i) = image_data[i] / 255.0;
		}

		// Calculate an layer 1
		l_1 = l_0 * w_0_1;

		// Apply an activation function to the layer 1 (hidden layer)
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

		// Apply an activation function to the layer 2 (output layer)
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

		// Apply an derivative of activation function
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

		// Check a divergence
		if (   error > max_error
		    || maxArrayElement(w_1_2.data(), w_1_2.size()) > max_weight
		    || maxArrayElement(w_0_1.data(), w_0_1.size()) > max_weight) {
			PRINT_ERR(true, PREF, "divergence");
			return -1;
		}

	} // Images loop

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t batch_gradientDescent(
        const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
        const uint32_t batch_size,
        Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
        double& error, uint32_t& correct_cnt,
        const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
        const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
        const uint32_t max_error, const uint32_t max_weight)
{
	// Layers and delta for layers
	std::vector<Eigen::MatrixXd> l_0(batch_size, Eigen::MatrixXd(1, imagesWidth * imagesHeight));
	std::vector<Eigen::MatrixXd> l_1(batch_size, Eigen::MatrixXd(1, hidden_size));
	std::vector<Eigen::MatrixXd> l_2(batch_size, Eigen::MatrixXd(1, num_labels));
	std::vector<Eigen::MatrixXd> l_2_delta(batch_size, Eigen::MatrixXd(1, num_labels));
	std::vector<Eigen::MatrixXd> l_1_delta(batch_size, Eigen::MatrixXd(1, hidden_size));

	// Images loop
	for (uint32_t i_image = 0; i_image < images_labels.size() / batch_size; ++i_image) {

		// Batch loop
		for (uint32_t batch = 0; batch < batch_size; ++batch) {

			// Get an image and label from list
			const auto& image_label = images_labels[i_image * batch_size + batch];

			// Check an image
			if (   image_label.first.isNull() == true
			    || image_label.first.sizeInBytes() != imagesWidth * imagesHeight) {
				PRINT_ERR(true, PREF, "Bad image");
				return -1;
			}

			// Get an image (layer 0)
			auto image_data = image_label.first.bits();
			for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
				l_0[batch](0, i) = image_data[i] / 255.0;
			}

			// Calculate an layer 1
			l_1[batch] = l_0[batch] * w_0_1;

			// Apply an activation function to the layer 1 (hidden layer)
			// Hyperbolic tangent
			tanh(l_1[batch].data(), l_1[batch].size());

			// Dropout a some neurons in layer 1
			std::vector<bool> dropout_mask(l_1[batch].size());
			if (getB_D(dropout_mask, 0.5, test) != 0) {
				PRINT_ERR(true, PREF, "Can't get bernoulli distribution vector");
				return -1;
			}
			for (uint32_t i = 0; i < l_1[batch].size(); ++i) {
				l_1[batch](0, i) *= dropout_mask[i] * 2;
			}

			// Calculate an layer 2
			l_2[batch] = l_1[batch] * w_1_2;

			// Apply an activation function to the layer 2 (output layer)
			softmax(l_2[batch].data(), l_2[batch].size());

			// Get a label
			Eigen::MatrixXd label(1, num_labels);
			for (uint32_t i = 0; i < num_labels; ++i) {
				label(0, i) = image_label.second[i];
			}

			// Calculate an error
			error += ((label - l_2[batch]) * (label - l_2[batch]).transpose()).sum();

			// Add to correct counter
			if (   maxArrayElementIndex(l_2[batch].data(), l_2[batch].size())
			    == maxArrayElementIndex(label.data(), label.size())) {
				++correct_cnt;
			}

			// Calculate a delta for layer 2
			l_2_delta[batch] = label - l_2[batch];
			for (uint32_t i = 0; i < l_2_delta[batch].size(); ++i) {
				l_2_delta[batch](0, i) /= (batch_size * num_labels);
			}

			// Calculate a delta for layer 1
			l_1_delta[batch] = l_2_delta[batch] * w_1_2.transpose();

			// Apply an derivative of activation function
			// Hyperbolic tangent
			tanh_derivative(l_1[batch].data(), l_1[batch].size());
			for (uint32_t i = 0; i < l_1[batch].size(); ++i) {
				l_1_delta[batch](0, i) *= l_1[batch](0, i);
			}

			// Dropout the same neurons as in layer 1
			for (uint32_t i = 0; i < l_1_delta[batch].size(); ++i) {
				l_1_delta[batch](0, i) *= dropout_mask[i];
			}

		} // Batch loop

		// Correct weights
		for (uint32_t batch = 0; batch < batch_size; ++batch) {
			w_1_2 += alpha * l_1[batch].transpose() * l_2_delta[batch];
			w_0_1 += alpha * l_0[batch].transpose() * l_1_delta[batch];
		}

		// Check a divergence
		if (   error > max_error
		    || maxArrayElement(w_1_2.data(), w_1_2.size()) > max_weight
		    || maxArrayElement(w_0_1.data(), w_0_1.size()) > max_weight) {
			PRINT_ERR(true, PREF, "divergence");
			return -1;
		}

	} // Images loop

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t testWeights(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels_test,
            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const uint32_t hidden_size,
            const uint32_t imagesWidth, const uint32_t imagesHeight,
            const uint32_t max_error, const bool test)
{
	// Test images loop
	for (const auto& image_label : images_labels_test) {

		// Layers
		Eigen::MatrixXd l_0(1, imagesWidth * imagesHeight);
		Eigen::MatrixXd l_1(1, hidden_size);
		Eigen::MatrixXd l_2(1, num_labels);

		if (getRecognitionLayer(image_label.first, w_0_1, w_1_2, l_0, l_1, l_2,
		                        imagesWidth, imagesHeight) != 0) {
			PRINT_ERR(true, PREF, "getRecognitionLayer()");
			return -1;
		}

		// Get a label
		Eigen::MatrixXd label(1, num_labels);
		for (uint32_t i = 0; i < num_labels; ++i) {
			label(0, i) = image_label.second[i];

			if (true == test) {
				PRINT_DBG(true, PREF, "%f", l_2(0, i));
			}
		}

		// Calculate an error
		error += ((label - l_2) * (label - l_2).transpose()).sum();

		// Add to correct counter
		if (   maxArrayElementIndex(l_2.data(), l_2.size())
		    == maxArrayElementIndex(label.data(), label.size())) {
			++correct_cnt;
		}

		// Check a divergence
		if (error > max_error) {
			PRINT_ERR(true, PREF, "divergence");
			return -1;
		}

	} // Test images loop

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t getRecognitionLayer(const QImage& image,
                            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                            Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                            const uint32_t imagesWidth, const uint32_t imagesHeight)
{
	// Check an image
	if (image.isNull() == true || image.sizeInBytes() != imagesWidth * imagesHeight) {
		PRINT_ERR(true, PREF, "Bad test image");
		return -1;
	}

	// Get an image (layer 0)
	auto image_data = image.bits();
	for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {
		l_0(0, i) = image_data[i] / 255.0;
	}

	// Calculate an layer 1
	l_1 = l_0 * w_0_1;

	// Apply an activation function to the layer 1 (hidden layer)
	// Hyperbolic tangent
	tanh(l_1.data(), l_1.size());

	// Calculate an layer 2
	l_2 = l_1 * w_1_2;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t getRecognitionLabel(const QImage& image,
                            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                            Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                            const uint32_t imagesWidth, const uint32_t imagesHeight)
{
	if (getRecognitionLayer(image, w_0_1, w_1_2, l_0, l_1, l_2, imagesWidth, imagesHeight) != 0) {
		PRINT_ERR(true, PREF, "getRecognitionLayer()");
		return -1;
	}

	return maxArrayElementIndex(l_2.data(), l_2.size());
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
