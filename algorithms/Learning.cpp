
#include "Learning.h"

#include <cmath>                  // std::abs(); std::pow(); std::tanh(); std::exp()
#include <vector>                 // std::vector<> class
#include <QImage>                 // QImage class
#include <QDir>                   // QDir class
#include <QFile>                  // QFile class
#include <algorithm>              // std::transform<>(); std::max<>()
#include <random>                 // PRNG
#include <ctime>                  // time()
#include <utility>                // std::pair<>
#include <QByteArray>             // QByteArray class
#include "Sec_145/other/Other.h"  // writeVarInFile<>()
#include "Algorithms.h"           // convertPixelLimits[](); dotProduct();
                                  // sumArrayExpElements<>();
                                  // maxArrayElement<>(); maxArrayElementIndex<>()
                                  // minArrayElement<>(); minArrayElementIndex<>()

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

#include <QThread> // sleep debug

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
// Save kernels
int32_t saveKernels(const Eigen::MatrixXd& kernels,
                    const uint32_t kernel_rows, const uint32_t kernel_cols,
                    const QString& resultPath, const QString& resultName);

//-------------------------------------------------------------------------------------------------
// Save a distribution for kernels
int32_t saveKernelsDistr(const Eigen::MatrixXd& kernelsDistr,
                         const uint32_t num_kernels, const uint32_t conv_w, const uint32_t conv_h,
                         const QString& resultPath, const QString& resultName);

//-------------------------------------------------------------------------------------------------
// Convert vector items to visible view and save its
int32_t convertToVisibleAndSave(std::vector<std::vector<double>> vec,
                                const uint32_t width, const uint32_t height,
                                const QString& resultPath, const QString& resultName);

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
// Batch gradient descent method with convolutional layer
int32_t batch_gradientDescent_conv(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
            const uint32_t batch_size,
            Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
            const uint32_t kernel_rows, const uint32_t kernel_cols, const uint32_t num_kernels,
            const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
            const uint32_t max_error, const uint32_t max_weight);

//-------------------------------------------------------------------------------------------------
// Test neural network weights
int32_t testWeights(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels_test,
            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const uint32_t hidden_size,
            const uint32_t kernel_rows, const uint32_t kernel_cols,
            const uint32_t imagesWidth, const uint32_t imagesHeight,
            const uint32_t max_error, const bool test = false);

//-------------------------------------------------------------------------------------------------
// Get recognition from neural network
int32_t getRecognitionLayer(const QImage& image,
                            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                            Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                            const uint32_t imagesWidth, const uint32_t imagesHeight);

//-------------------------------------------------------------------------------------------------
// Get recognition from neural network with convolution layer
int32_t getRecognitionLayer_cnn(const QImage& image,
                                const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                                Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                                const uint8_t kernel_rows, const uint8_t kernel_cols,
                                const uint32_t imagesWidth, const uint32_t imagesHeight);

//-------------------------------------------------------------------------------------------------
// Get recognition from neural network for copters (version 1)
//int32_t getRecognitionLayer_copters_v1();

//-------------------------------------------------------------------------------------------------
// Max pooling 2D (2x2, step 2)
int32_t maxPooling2D(const Eigen::MatrixXd& in, Eigen::MatrixXd& out);

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
	const double alpha {0.001};

	// Max weight and error for catch a divergence
	const double max_weight {1000};
	const double max_error {1000};

	// Kernels parameters
	uint32_t num_kernels {32};
	uint32_t kernel_rows {5};
	uint32_t kernel_cols {5};

	// Number of intermediate layers
	const uint32_t hidden_size {(imagesWidth - kernel_cols + 1) *
		                        (imagesHeight - kernel_rows + 1) *
		                        num_kernels};

	// Number of labels
	const uint32_t num_labels {static_cast<uint32_t>(images_labels.front().second.size())};

	// Batch size
	const uint32_t batch_size {8}; // {24};

	// Weights
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

		// Apply a gradient descent with convolutional layer
		if (batch_gradientDescent_conv(
		                          images_labels, batch_size, w_0_1, w_1_2, error, correct_cnt,
		                          num_labels, alpha, hidden_size,
		                          kernel_rows, kernel_cols, num_kernels,
		                          imagesWidth, imagesHeight,
		                          test, max_error, max_weight) != 0) {
			PRINT_ERR(true, PREF, "batch_gradientDescent_conv()");
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
		                error, correct_cnt, num_labels, hidden_size, kernel_rows, kernel_cols,
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

	// Save kernels and a distribution for kernels
	if (saveKernels(w_0_1, kernel_rows, kernel_cols, resultPath, "k_") != 0) {
		PRINT_ERR(true, PREF, "saveKernels(w_0_1)");
		return -1;
	}
	if (saveKernelsDistr(w_1_2, num_kernels,
	                     imagesWidth - kernel_cols + 1, imagesHeight - kernel_rows + 1,
	                     resultPath, "l_") != 0) {
		PRINT_ERR(true, PREF, "saveKernelsDistr(w_1_2)");
		return -1;
	}


	////
	Eigen::MatrixXd l_0((imagesWidth - kernel_cols + 1) * (imagesHeight - kernel_rows + 1),
	                    kernel_rows * kernel_cols);
	Eigen::MatrixXd l_1(1, hidden_size);
	Eigen::MatrixXd l_2(1, num_labels);
	if (getRecognitionLayer_cnn(images_labels_test[1].first, w_0_1, w_1_2, l_0, l_1, l_2,
	                            kernel_rows, kernel_cols, imagesWidth, imagesHeight) != 0) {
		PRINT_ERR(true, PREF, "getRecognitionLayer_cnn()");
		return -1;
	}
	// Get a distribution for each kernel from matrix (distributions are located in columns)
	std::vector<std::vector<double>> vec_kernelsDistr;
	for (uint32_t ker = 0; ker < num_kernels; ++ker) {
		vec_kernelsDistr.push_back(std::vector<double>());
		for (uint32_t distr = ker; distr < l_1.cols(); distr += num_kernels) {
			vec_kernelsDistr[ker].push_back(l_1(0, distr));
		}
	}
	if (convertToVisibleAndSave(vec_kernelsDistr,
	                            imagesWidth - kernel_cols + 1, imagesHeight - kernel_rows + 1,
	                            resultPath + "/l_1/",
	                            "") != 0) {
		PRINT_ERR(true, PREF, "convertToVisibleAndSave()");
		return -1;
	}
	return 0;
	////


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
int32_t saveKernels(const Eigen::MatrixXd& kernels,
                    const uint32_t kernel_rows, const uint32_t kernel_cols,
                    const QString& resultPath, const QString& resultName)
{
	// Check the incoming parameters
	if (kernels.rows() != kernel_rows * kernel_cols) {
		PRINT_ERR(true, PREF, "Number of rows in matrix with kernels is not equal to "
		                      "kernel_rows * kernel_cols");
		return -1;
	}

	// Get each kernel from matrix (each kernel is located in columns)
	std::vector<std::vector<double>> vec_kernels;
	for (uint32_t col = 0; col < kernels.cols(); ++col) {
		vec_kernels.push_back(std::vector<double>());
		for (uint32_t row = 0; row < kernels.rows(); ++row) {
			vec_kernels[col].push_back(kernels(row, col));
		}
	}

	// Convert a vector and save images with kernels
	return convertToVisibleAndSave(vec_kernels, kernel_cols, kernel_rows,
	                               resultPath + "/kernels/", resultName);
}

//-------------------------------------------------------------------------------------------------
int32_t saveKernelsDistr(const Eigen::MatrixXd& kernelsDistr,
                         const uint32_t num_kernels, const uint32_t conv_w, const uint32_t conv_h,
                         const QString& resultPath, const QString& resultName)
{
	// Check the incoming parameters
	if (kernelsDistr.rows() % num_kernels != 0) {
		PRINT_ERR(true, PREF, "Number of rows in matrix is not divided by the number of kernels");
		return -1;
	}

	// Loop for each label (column) for kernels
	for (uint32_t col = 0; col < kernelsDistr.cols(); ++col) {

	// Get a distribution for each kernel from matrix (distributions are located in columns)
	std::vector<std::vector<double>> vec_kernelsDistr;
	for (uint32_t ker = 0; ker < num_kernels; ++ker) {
		vec_kernelsDistr.push_back(std::vector<double>());
		for (uint32_t distr = ker; distr < kernelsDistr.rows(); distr += num_kernels) {
			vec_kernelsDistr[ker].push_back(kernelsDistr(distr, col));
		}
	}

	// Convert each distribution for each kernel to a visible view and save their
	if (convertToVisibleAndSave(vec_kernelsDistr, conv_w, conv_h,
	                            resultPath + "/kernel_distr/",
	                            resultName + QString::number(col) + "_") != 0) {
		PRINT_ERR(true, PREF, "convertToVisibleAndSave()");
		return -1;
	}

	} // Loop for each label (column) for kernels

	return 0;
}

//-------------------------------------------------------------------------------------------------
// Convert vector items to visible view and save its
int32_t convertToVisibleAndSave(std::vector<std::vector<double>> vec,
                                const uint32_t width, const uint32_t height,
                                const QString& resultPath, const QString& resultName)
{
	// Convert each vector to a visible view and save their
	for (uint32_t i = 0; i < vec.size(); ++i) {

		// Check the size of vector
		if (vec[i].size() != width * height) {
			PRINT_ERR(true, PREF, "Size of vector is not equal to width * height");
			return -1;
		}

		// Get vector data
		double* const vec_data = vec[i].data();

		// Convert: item [min, max] -> item += min -> item [0, max + min]
		/*const double min = std::abs(minArrayElement(vec_data,
													static_cast<uint32_t>(vec[i].size())));
		for (uint32_t j = 0; j < vec[i].size(); ++j) {
			vec_data[j] += min;
		}*/

		// Create an image for vector and get its data
		QImage image(width, height, QImage::Format_Grayscale8);
		uint8_t* const image_data = image.bits();

		// Convert: item [0, max + min] -> item *= 255/max -> item [0, 255]
		const double max = maxArrayElement(vec_data, static_cast<uint32_t>(vec[i].size()));
		if (max > 255) {
			PRINT_ERR(true, PREF, "Maximal element from %lu vector is too large",
			          static_cast<unsigned long>(i));
			continue;
		}
		for (uint32_t j = 0; j < vec[i].size(); ++j) {
			vec_data[j] *= 255.0 / max;
			//image_data[j] = vec_data[j];
			image_data[j] = (vec_data[j] > 0 ? vec_data[j] : 0);
		}

		// Save an image with vector
		const QString s = resultPath + resultName + QString::number(i) + ".png";
		if (image.save(s) != true) {
			PRINT_ERR(true, PREF, "Can not save an image with %lu vector",
			          static_cast<unsigned long>(i));
			return -1;
		}
	}

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
				// Normalization of image data
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
int32_t batch_gradientDescent_conv(
            const std::vector<std::pair<QImage, std::vector<uint32_t>>>& images_labels,
            const uint32_t batch_size,
            Eigen::MatrixXd& w_0_1, Eigen::MatrixXd& w_1_2,
            double& error, uint32_t& correct_cnt,
            const uint32_t num_labels, const double alpha, const uint32_t hidden_size,
            const uint32_t kernel_rows, const uint32_t kernel_cols, const uint32_t num_kernels,
            const uint32_t imagesWidth, const uint32_t imagesHeight, const bool test,
            const uint32_t max_error, const uint32_t max_weight)
{
	// Layers and delta for layers
	std::vector<Eigen::MatrixXd> l_0(batch_size, Eigen::MatrixXd(
	                           (imagesWidth - kernel_cols + 1) * (imagesHeight - kernel_rows + 1),
	                           kernel_rows * kernel_cols));
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
			for (uint32_t l_row = 0; l_row < imagesHeight - kernel_rows + 1; ++l_row) {
				for (uint32_t l_col = 0; l_col < imagesWidth - kernel_cols + 1; ++l_col) {
					for (uint32_t k_row = 0; k_row < kernel_rows; ++k_row) {
						for (uint32_t k_col = 0; k_col < kernel_cols; ++k_col) {
							l_0[batch](l_row * (imagesWidth - kernel_cols + 1) + l_col,
							           k_row * kernel_cols + k_col) =
							    image_data[(l_row + k_row) * imagesWidth + l_col + k_col] / 255.0;
						}
					}
				}
			}

			// Calculate an layer 1
			//  Get a convolutions
			Eigen::MatrixXd tmp = l_0[batch] * w_0_1;
			//  Matrix to vecor transformation
			for (uint32_t row = 0; row < tmp.rows(); ++row) {
				for (uint32_t col = 0; col < tmp.cols(); ++col) {
					l_1[batch](0, row * tmp.cols() + col) = tmp(row, col);
				}
			}

			// Apply an activation function to the layer 1 (hidden layer)
			// Hyperbolic tangent
			// tanh(l_1[batch].data(), l_1[batch].size());
			ReLU(l_1[batch].data(), l_1[batch].size());

			// Dropout a some neurons in layer 1
			std::vector<bool> dropout_mask(l_1[batch].size());
			if (getB_D(dropout_mask, 0.7, test) != 0) {
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
			//tanh_derivative(l_1[batch].data(), l_1[batch].size());
			ReLU_derivative(l_1[batch].data(), l_1[batch].size());
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

			// Correct w_1_2
			w_1_2 += alpha * l_1[batch].transpose() * l_2_delta[batch];

			// Correct w_0_1
			//  Vector to matrix transformation
			Eigen::MatrixXd tmp(
			            (imagesWidth - kernel_cols + 1) * (imagesHeight - kernel_rows + 1),
			            num_kernels);
			for (uint32_t row = 0; row < tmp.rows(); ++row) {
				for (uint32_t col = 0; col < tmp.cols(); ++col) {
					tmp(row, col) = l_1_delta[batch](row * tmp.cols() + col);
				}
			}
			//  Correct
			w_0_1 += alpha * l_0[batch].transpose() * tmp;
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
            const uint32_t kernel_rows, const uint32_t kernel_cols,
            const uint32_t imagesWidth, const uint32_t imagesHeight,
            const uint32_t max_error, const bool test)
{
	// Test images loop
	for (const auto& image_label : images_labels_test) {

		// Layers
//		Eigen::MatrixXd l_0(1, imagesWidth * imagesHeight);
//		Eigen::MatrixXd l_1(1, hidden_size);
//		Eigen::MatrixXd l_2(1, num_labels);

//		if (getRecognitionLayer(image_label.first, w_0_1, w_1_2, l_0, l_1, l_2,
//		                        imagesWidth, imagesHeight) != 0) {
//			PRINT_ERR(true, PREF, "getRecognitionLayer()");
//			return -1;
//		}

		Eigen::MatrixXd l_0((imagesWidth - kernel_cols + 1) * (imagesHeight - kernel_rows + 1),
		                    kernel_rows * kernel_cols);
		Eigen::MatrixXd l_1(1, hidden_size);
		Eigen::MatrixXd l_2(1, num_labels);

		if (getRecognitionLayer_cnn(image_label.first, w_0_1, w_1_2, l_0, l_1, l_2,
		                            kernel_rows, kernel_cols, imagesWidth, imagesHeight) != 0) {
			PRINT_ERR(true, PREF, "getRecognitionLayer_cnn()");
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
		PRINT_ERR(true, PREF, "Bad image");
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
int32_t getRecognitionLayer_cnn(const QImage& image,
                                const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                                Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                                const uint8_t kernel_rows, const uint8_t kernel_cols,
                                const uint32_t imagesWidth, const uint32_t imagesHeight)
{
	// Check an image
	if (image.isNull() == true || image.sizeInBytes() != imagesWidth * imagesHeight) {
		PRINT_ERR(true, PREF, "Bad image");
		return -1;
	}

	// Get an image (layer 0)
	auto image_data = image.bits();
	for (uint32_t l_row = 0; l_row < imagesHeight - kernel_rows + 1; ++l_row) {
		for (uint32_t l_col = 0; l_col < imagesWidth - kernel_cols + 1; ++l_col) {
			for (uint32_t k_row = 0; k_row < kernel_rows; ++k_row) {
				for (uint32_t k_col = 0; k_col < kernel_cols; ++k_col) {
					l_0(l_row * (imagesWidth - kernel_cols + 1) + l_col,
					    k_row * kernel_cols + k_col) =
					    image_data[(l_row + k_row) * imagesWidth + l_col + k_col] / 255.0;
				}
			}
		}
	}

	// Calculate an layer 1
	//  Get a convolutions
	Eigen::MatrixXd tmp = l_0 * w_0_1;
	//  Matrix to vecor transformation
	for (uint32_t row = 0; row < tmp.rows(); ++row) {
		for (uint32_t col = 0; col < tmp.cols(); ++col) {
			l_1(0, row * tmp.cols() + col) = tmp(row, col);
		}
	}

	// Apply an activation function to the layer 1 (hidden layer)
	// ReLU
	ReLU(l_1.data(), l_1.size());

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
int32_t maxPooling2D(const Eigen::MatrixXd& in, Eigen::MatrixXd& out, const uint32_t line_size)
{
	// Check the number of rows and columns of incomming matrix
	if (   in.rows() / (2 * 2) != out.rows()
	    || in.cols() != out.cols()
	    || in.rows() / line_size != line_size) {
		PRINT_ERR(true, PREF, "Bad size of input or output matrix");
		return -1;
	}

	// Max pooling 2D for each kernel
	for (uint32_t ker = 0; ker < in.cols(); ++ker) {

	// Max pooling 2D
	for (uint32_t i = 0; i < line_size / 2; ++i) {
		for (uint32_t j = 0, k = 0; j < line_size; j += 2, ++k) {
			// qDebug() << 2 * i << j; // in
			// qDebug() << 2 * i * line_size + j; // in
			// qDebug() << 2 * i * line_size + j + line_size; // in
			// qDebug() << i * line_size / 2 + k; // out

			double max_1 = std::max(in(2 * i * line_size + j, ker),
			                        in(2 * i * line_size + j + 1, ker));
			double max_2 = std::max(in(2 * i * line_size + j + line_size, ker),
			                        in(2 * i * line_size + j + line_size + 1, ker));
			out(i * line_size / 2 + k, ker) = std::max(max_1, max_2);
		}
		//qDebug() << "--------";
	} // Max pooling 2D

	for (uint32_t row = 0; row < out.rows(); ++row) {
		//for (uint32_t col = 0; col < out.cols(); ++col) {
		    qDebug() << out(row, 0);
	    }
	    //qDebug() << "----";
	//}
	qDebug() << "========";

	QThread::sleep(2);
	return 0;

	} // Max pooling 2D for each kernel
}

//-------------------------------------------------------------------------------------------------
// Number of convolute layers
const uint32_t num_conv_layers {4};

// In/out size for convolute layers
const uint32_t in_conv_size[num_conv_layers]  {152, 75, 36, 17};
const uint32_t out_conv_size[num_conv_layers] {75,  36, 17,  8};

// Rows and columns of kernels
const uint32_t kernels_rows {3};
const uint32_t kernels_cols {3};

// Number of kernels for convolute layers
const uint32_t num_of_kernels[num_conv_layers] {32, 64, 128, 128};

// Depth of kernels
const uint32_t depth_of_kernels[num_conv_layers] {3, 32, 64, 128};

// Names of kernels
const QString kernels_names[num_conv_layers] {
	"conv2d_12", "conv2d_13", "conv2d_14", "conv2d_15"
};

// Inputs and outputs for convolute layers
std::vector<std::vector<Eigen::MatrixXd>> in_conv(num_conv_layers);
std::vector<std::vector<Eigen::MatrixXd>> out_conv(num_conv_layers);

// Kernels
std::vector<std::vector<Eigen::MatrixXd>> kernels(num_conv_layers);

//-------------------------------------------------------------------------------------------------
// Get recognition from neural network for copters (version 1)
int32_t getRecognitionLayer_copters_v1()
{
	// For tensor with image data
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

	// Loop for depth of convolution
	for (uint32_t depth_of_kernels_i = 0;
	     depth_of_kernels_i < depth_of_kernels[0];
	     ++depth_of_kernels_i) {

		// Load kernels
		//  File name with kernels
		QString fileName =   "C:/Users/ekd/AppData/Local/Programs/Python/Python38/Scripts/"
		                     "copter_rgb/tensor_"
		                   + QString::number(depth_of_kernels_i)
		                   + ".txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read kernels
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        in_conv_size[0] * in_conv_size[0]) != 0) {
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get tensor
		for (uint32_t l_row = 0; l_row < in_conv_size[0] - kernels_rows + 1; ++l_row) {
			for (uint32_t l_col = 0; l_col < in_conv_size[0] - kernels_cols + 1; ++l_col) {
				for (uint32_t k_row = 0; k_row < kernels_rows; ++k_row) {
					for (uint32_t k_col = 0; k_col < kernels_cols; ++k_col) {
						in_conv[0][depth_of_kernels_i](
						        l_row * (in_conv_size[0] - kernels_cols + 1) + l_col,
						        k_row * kernels_cols + k_col) =
						    vectors[0][(l_row + k_row) * in_conv_size[0] + l_col + k_col];
						    // / 255.0; !!
					}
				}
			}
		}

		// Clears vectors with file contents
		vectors.clear();
	}


	//---------------------------

	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < 1; // num_conv_layers; ///////////////////////////////////////////
	     ++num_conv_layers_i) {

	// Loop for depth of convolution
	for (uint32_t depth_of_kernels_i = 0;
	     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i];
	     ++depth_of_kernels_i) {

		// Get convolution result
		Eigen::MatrixXd conv = in_conv[num_conv_layers_i][depth_of_kernels_i] *
		                       kernels[num_conv_layers_i][depth_of_kernels_i];

//		// Matrix for result of max pooling 2D
//		Eigen::MatrixXd pooling(conv.rows() / (2 * 2), conv.cols());

//		// Apply max pooling 2D
//		maxPooling2D(conv, pooling, in_conv_size[num_conv_layers_i] - kernels_rows + 1);

//		return 0;
	} // Loop for depth of convolution

	// Loop for depth of convolution
	for (uint32_t depth_of_kernels_i = 1;
	     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i];
	     ++depth_of_kernels_i) {

	} // Loop for depth of convolution

	}


	return 0;
}

//-------------------------------------------------------------------------------------------------
// Load neural network for copters (version 1)
int32_t loadModel_copters_v1(const QString& pathToModel)
{
	// Inputs/outputs for convolute layers
	for (uint32_t i = 0; i < num_conv_layers; ++i) {
		for (uint32_t j = 0; j < depth_of_kernels[i]; ++j) {
			in_conv[i].push_back(
			            Eigen::MatrixXd(
			                (in_conv_size[i] - kernels_cols + 1) *
			                (in_conv_size[i] - kernels_rows + 1),
			                kernels_rows * kernels_cols));
		}

		out_conv[i].push_back(
		            Eigen::MatrixXd(
		                (in_conv_size[i] - kernels_cols + 1) *
		                (in_conv_size[i] - kernels_rows + 1),
		                num_of_kernels[i])); // ?!?!?!?!?!?!?!?!?!?!?!??!!?!?!?!?!?
	}

	// Kernels
	for (uint32_t i = 0; i < num_conv_layers; ++i) {
		for (uint32_t j = 0; j < depth_of_kernels[i]; ++j) {
			kernels[i].push_back(
			            Eigen::MatrixXd(kernels_rows * kernels_cols, num_of_kernels[i]));
		}
	}

	// For files with weights
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < 1; // num_conv_layers; ///////////////////////////////////////////
	     ++num_conv_layers_i) {

	// Loop for depth of convolution
	for (uint32_t depth_of_kernels_i = 0;
	     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i];
	     ++depth_of_kernels_i) {

		// Load kernels
		//  File name with kernels
		QString fileName =   pathToModel + "/"
		                   + kernels_names[num_conv_layers_i] + "_kernel_"
		                   + QString::number(depth_of_kernels_i)
		                   + ".txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read kernels
		if (readVectorsFromFile(f, vectors,
		                        num_of_kernels[num_conv_layers_i],
		                        kernels_rows * kernels_cols) != 0) {
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get matrix with kernels
		for (uint32_t i = 0; i < num_of_kernels[num_conv_layers_i]; ++i) {
			for (uint32_t j = 0; j < kernels_rows * kernels_cols; ++j) {
				kernels[num_conv_layers_i][depth_of_kernels_i](j, i) = vectors[i][j];
			}
		}

		// Clears vectors with file contents
		vectors.clear();

	} // Loop for depth of convolution

	} // Loop for each convolution


/*
	//================= Layer 0 =================
	// Open a file with weight (conv2d)
	//f.setFileName(pathToModel + "/conv2d_kernels.txt");
	//f.setFileName(pathToModel + "/conv2d_bias.txt");
	f.setFileName(pathToModel + "/tensor_blue.txt");
	if (f.open(QIODevice::ReadOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open a file with conv2d weights");
		return -1;
	}

	// Read a weight w_0_1

	//if (readVectorsFromFile(f, vectors, 32, 9) != 0) {
	//if (readVectorsFromFile(f, vectors, 1, 32) != 0) {
	if (readVectorsFromFile(f, vectors, 1, 152*152) != 0) {
		return -1;
	}

	// Close file with weight w_0_1
	f.close();

	// Layer 0
	for (uint32_t l_row = 0; l_row < imagesHeight - kernels_rows + 1; ++l_row) {
		for (uint32_t l_col = 0; l_col < imagesWidth - kernels_cols + 1; ++l_col) {
			for (uint32_t k_row = 0; k_row < kernels_rows; ++k_row) {
				for (uint32_t k_col = 0; k_col < kernels_cols; ++k_col) {
					l_0(l_row * (imagesWidth - kernels_cols + 1) + l_col,
						k_row * kernels_cols + k_col) =
					    vectors[0][(l_row + k_row) * imagesWidth + l_col + k_col]; // / 255.0; !!
				}
			}
		}
	}
	vectors.clear();

	//================= conv2d =================
	// Open a file with weight (conv2d)
	f.setFileName(pathToModel + "/conv2d_12_kernels_blue.txt");
	//f.setFileName(pathToModel + "/conv2d_bias.txt");
	//f.setFileName(pathToModel + "/tensor.txt");
	if (f.open(QIODevice::ReadOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open a file with conv2d weights");
		return -1;
	}

	// Read a weight w_0_1
	if (readVectorsFromFile(f, vectors, 32, 9) != 0) {
	//if (readVectorsFromFile(f, vectors, 1, 32) != 0) {
	//if (readVectorsFromFile(f, vectors, 1, 152*152) != 0) {
		return -1;
	}

	// Close file with weight w_0_1
	f.close();

	for (uint32_t i = 0; i < vectors.size(); ++i) {
		for (uint32_t j = 0; j < kernels_rows * kernels_cols; ++j) {
			conv2d(j, i) = vectors[i][j];
		}
	}
	vectors.clear();

	//===========================================
	Eigen::MatrixXd res = l_0 * conv2d;
	//qDebug() << res.rows() << res.cols();
*/
	/*//================= biases =================
	// Open a file with weight (conv2d)
	//f.setFileName(pathToModel + "/conv2d_kernels.txt");
	f.setFileName(pathToModel + "/conv2d_bias.txt");
	//f.setFileName(pathToModel + "/tensor.txt");
	if (f.open(QIODevice::ReadOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open a file with conv2d weights");
		return -1;
	}

	// Read a weight w_0_1
	//if (readVectorsFromFile(f, vectors, 32, 9) != 0) {
	if (readVectorsFromFile(f, vectors, 1, 32) != 0) {
	//if (readVectorsFromFile(f, vectors, 1, 152*152) != 0) {
		return -1;
	}

	// Close file with weight w_0_1
	f.close();


	for (uint32_t col = 0; col < res.cols(); ++col) {
		for (uint32_t row = 0; row < res.rows(); ++row) {
			res(row, col) += vectors[0][col];
		}
	}
	vectors.clear();*/

	//================= RELU =================
	//ReLU(res.data(), res.size());

	//===========================================
/*	f.setFileName(pathToModel + "/res_blue.txt");
	if (f.open(QIODevice::WriteOnly) == false) {
		PRINT_ERR(true, PREF, "Can't open a file for res");
		return -1;
	}
	if (writeMatrixInFile(f, res) != 0) {
		PRINT_ERR(true, PREF, "writeMatrixInFile");
		return -1;
	}
	f.close();

	return 0;

	//=========================================== compare
	QFile file_1(pathToModel + "/res.txt");
	QFile file_2(pathToModel + "/conv2d.txt");
	bool c;
	if (compareFiles(file_1, file_2, res.rows() * res.cols(), c) != 0) {
		PRINT_ERR(true, PREF, "compareFiles");
		return -1;
	}

	qDebug() << c;
*/
//	for (auto vector : vectors) {
//		for (auto el : vector) {
//			qDebug() << el;
//		}
//	}
//	QThread::sleep(1);

//	for (uint32_t i = 0; i < res.rows(); ++i) {
//		for (uint32_t j = 0; j < res.cols(); ++j) {
//			qDebug() << res(i, j);
//		}
//		qDebug() << "==============";
//	}
//	QThread::sleep(3);

	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
