
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO:
 * test class (logic() method)
 * Eigen exceptions (memory allocation)
FIXME:
DANGER:
NOTE:
 * Access only across '*' or '[]' to data received from Eigen::MatrixXd::data() method

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>      // integer types
#include <array>        // std::array class
#include <QString>      // QString class
#include <QImage>       // QImage class
#include "Eigen/Dense"  // Eigen::MatrixXd class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class NeuralNetwork
{
public:

	NeuralNetwork(const QString& pathToModel = QString());

	// Load a neural network
	int32_t loadModel(const QString& pathToModel); // noexcept

	// Get a recognition label from neural network
	int32_t getRecognitionLabel(const QImage& image); // noexcept
	int32_t getRecognitionLabel(const QString& fileName); // noexcept

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[NeuralNetwork]: "};

	// Start and finish time
	uint64_t m_startTime;
	uint64_t m_finishTime;

	//
	bool m_loaded;

//---------------------
	// Convolute layers

	// Number of convolute layers
	uint32_t m_num_conv_layers;

	// Incoming size for convolute layers
	std::vector<uint32_t> in_conv_size;

	// Outgoing size for max pooling layers
	std::vector<uint32_t> out_pooling_size;

	// Rows and columns of kernels
	uint32_t kernels_rows;
	uint32_t kernels_cols;

	// Number of kernels
	std::vector<uint32_t> num_of_kernels;

	// Depth of kernels
	std::vector<uint32_t> depth_of_kernels;

	// Names of kernels layers
	std::vector<QString> kernels_names;

	// Inputs for convolute layers
	std::vector<std::vector<Eigen::MatrixXd>> conv_in;

	// Outputs for convolute layers
	std::vector<Eigen::MatrixXd> conv_out;

	// Kernels
	std::vector<std::vector<Eigen::MatrixXd>> kernels;

	// Biases for convolute layers
	std::vector<std::vector<double>> conv_biases;

//-----------------
	// Dense layers

	// Number of dense layers
	uint32_t num_dense_layers;

	// Incoming size for dense layers
	std::vector<uint32_t> in_dense_size;

	// Outgoing size for dense layers
	std::vector<uint32_t> out_dense_size;

	// Names of denses layers
	std::vector<QString> denses_names;

	// Inputs for dense layers
	static std::vector<Eigen::MatrixXd> dense_in;

	// Outputs for dense layers
	static std::vector<Eigen::MatrixXd> dense_out;

	// Dense matrixes with weights
	static std::vector<Eigen::MatrixXd> denses;

	// Biases for dense layers
	static std::vector<std::vector<double>> dense_biases;

//------------------------------------------------
	// Get a recognition label from neural network
	int32_t getRecognitionLabel(); // const noexcept
};

//-------------------------------------------------------------------------------------------------
// Get a recognition label from neural network
int32_t getRecognitionLabel(const QImage& image);
int32_t getRecognitionLabel(const QString& fileName);

//-------------------------------------------------------------------------------------------------
// Load a neural network
int32_t loadModel(const QString& pathToModel);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
