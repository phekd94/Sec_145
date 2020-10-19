
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO:
 * test class (logic() method)
 * Eigen exceptions (memory allocation)
 * Qt classes exceptions
FIXME:
DANGER:
NOTE:
 * Access only across '*' or '[]' to data received from Eigen::MatrixXd::data() method

Sec_145::NeuralNetwork class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>      // integer types
#include <vector>       // std::vector<>
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

	// Load a neural network
	// (exception from push_back() can be thrown (it is unlikely))
	int32_t loadModel(const QString& pathToModel);

	// ***********************
	// ******* Getters *******
	// ***********************

	// Gets a recognition label from neural network
	int32_t getRecognitionLabel(const uint8_t* const r,
	                            const uint8_t* const g,
	                            const uint8_t* const b); // noexcept
	int32_t getRecognitionLabel(const QString& fileName); // noexcept

	// Gets a recognition value from neural network
	double getRecognitionValue() noexcept
	{
		return m_recognitionValue;
	}

	// Gets start time
	uint64_t getStartTime() const noexcept
	{
		return m_startTime;
	}

	// Gets finish time
	uint64_t getFinishTime() const noexcept
	{
		return m_finishTime;
	}

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[NeuralNetwork]: "};

	// Start and finish time
	volatile uint64_t m_startTime {0};
	volatile uint64_t m_finishTime {0};

	// Recognition value
	volatile double m_recognitionValue {0};

	// Flag; model has been loaded
	bool m_loaded {false};

//---------------------
	// Convolute layers

	// Number of convolute layers
	uint32_t m_num_conv_layers;

	// Incoming size for convolute layers
	std::vector<uint32_t> m_in_conv_size;

	// Outgoing size for max pooling layers
	std::vector<uint32_t> m_out_pooling_size;

	// Rows and columns of kernels
	uint32_t m_kernels_rows;
	uint32_t m_kernels_cols;

	// Number of kernels
	std::vector<uint32_t> m_num_of_kernels;

	// Depth of kernels
	std::vector<uint32_t> m_depth_of_kernels;

	// Names of kernels layers
	std::vector<QString> m_kernels_names;

	// Inputs for convolute layers
	std::vector<std::vector<Eigen::MatrixXd>> m_conv_in;

	// Outputs for convolute layers
	std::vector<Eigen::MatrixXd> m_conv_out;

	// Kernels
	std::vector<std::vector<Eigen::MatrixXd>> m_kernels;

	// Biases for convolute layers
	std::vector<std::vector<double>> m_conv_biases;

//-----------------
	// Dense layers

	// Number of dense layers
	uint32_t m_num_dense_layers;

	// Incoming size for dense layers
	std::vector<uint32_t> m_in_dense_size;

	// Outgoing size for dense layers
	std::vector<uint32_t> m_out_dense_size;

	// Names of denses layers
	std::vector<QString> m_denses_names;

	// Inputs for dense layers
	std::vector<Eigen::MatrixXd> m_dense_in;

	// Outputs for dense layers
	std::vector<Eigen::MatrixXd> m_dense_out;

	// Dense matrixes with weights
	std::vector<Eigen::MatrixXd> m_denses;

	// Biases for dense layers
	std::vector<std::vector<double>> m_dense_biases;

//------------------------------------------------
	// Get a recognition label from neural network
	int32_t getRecognitionLabel(); // noexcept
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
