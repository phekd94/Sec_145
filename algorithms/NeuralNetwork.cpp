
#include "NeuralNetwork.h"

#include <cmath>                  // std::tanh(); std::exp()
#include <QFile>                  // QFile class
#include <algorithm>              // std::max<>()
#include <chrono>                 // std::chrono functions
#include "Sec_145/other/Other.h"  // readVectorsFromFile<>()
#include "Algorithms.h"           // sumArrayExpElements<>();
                                  // maxArrayElement<>(); maxArrayElementIndex<>()

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// ReLU activation function
template <typename DataType>
void ReLU(DataType* const data, const uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		DataType& element = data[i];
		element = (element > 0) * element;
	}
}

//-------------------------------------------------------------------------------------------------
// Hyperbolic tangent activation function
template <typename DataType>
void tanh(DataType* const data, const uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		data[i] = std::tanh(data[i]);
	}
}

//-------------------------------------------------------------------------------------------------
// Softmax activation function
template <typename DataType>
void softmax(DataType* const data, const uint32_t size)
{
	double sum = sumArrayExpElements(data, size);
	for (uint32_t i = 0; i < size; ++i)
	{
		data[i] = std::exp(data[i]) / sum;
	}
}

//-------------------------------------------------------------------------------------------------
// Sigmoid activation function
template <typename DataType>
void sigmoid(DataType* const data, const uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		data[i] = std::exp(data[i]) / (std::exp(data[i]) + 1);
	}
}

//-------------------------------------------------------------------------------------------------
// Max pooling 2D (2x2, step 2)
int32_t maxPooling2D(const Eigen::MatrixXd& in, Eigen::MatrixXd& out,
                     const uint32_t in_line_size, const uint32_t out_line_size)
{
	// Check the size of input and output matrix
	if (   in_line_size / 2 != out_line_size
	    || in.cols() != out.cols()
	    || in.rows() / in_line_size != in_line_size
	    || out.rows() / out_line_size != out_line_size
	   )
	{
		PRINT_ERR(true, "Wrong size of input or output matrix");
		return -1;
	}

	// Max pooling 2D for each kernel
	for (uint32_t kernel_i = 0; kernel_i < in.cols(); ++kernel_i)
	{
		// Max pooling 2D
		for (uint32_t i = 0, m = 0; m < out_line_size; i += 2, ++m)
		{
			for (uint32_t j = 0, k = 0; k < out_line_size; j += 2, ++k)
			{
				double max_1 = std::max(in(i * in_line_size + j, kernel_i),
				                        in(i * in_line_size + j + 1, kernel_i));
				double max_2 = std::max(in(i * in_line_size + j + in_line_size, kernel_i),
				                        in(i * in_line_size + j + in_line_size + 1, kernel_i));
				out(m * out_line_size + k, kernel_i) = std::max(max_1, max_2);
			}
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t NeuralNetwork::loadModel(const QString& pathToModel)
{
	// Check flag
	if (true == m_loaded)
	{
		PRINT_ERR(true, "Model already has been loaded");
		return -1;
	}


	if (readParameters(pathToModel) != 0)
	{
		PRINT_ERR(true, "readParameters(%s)", pathToModel.toStdString().c_str());
		return -1;
	}

//-----------------------------
	// For each convolute layer
	for (uint32_t i = 0; i < m_num_conv_layers; ++i)
	{
		// Input
		for (uint32_t j = 0; j < m_depth_of_kernels[i]; ++j)
		{
			m_conv_in[i].push_back(
			            Eigen::MatrixXd((m_in_conv_size[i] - m_kernels_cols + 1) *
			                            (m_in_conv_size[i] - m_kernels_rows + 1),
			                            m_kernels_rows * m_kernels_cols));
		}

		// Output
		m_conv_out[i] = Eigen::MatrixXd((m_in_conv_size[i] - m_kernels_cols + 1) *
		                                (m_in_conv_size[i] - m_kernels_rows + 1),
		                                m_num_of_kernels[i]);

		// Kernels
		for (uint32_t j = 0; j < m_depth_of_kernels[i]; ++j)
		{
			m_kernels[i].push_back(
			            Eigen::MatrixXd(m_kernels_rows * m_kernels_cols, m_num_of_kernels[i]));
		}

		// Biases
		m_conv_biases[i] = std::vector<double>(m_num_of_kernels[i]);
	}

//-------------------------
	// For each dense layer
	for (uint32_t i = 0; i < m_num_dense_layers; ++i)
	{
		// Input
		m_dense_in[i] = Eigen::MatrixXd(1, m_in_dense_size[i]);

		// Output
		m_dense_out[i] = Eigen::MatrixXd(1, m_out_dense_size[i]);

		// Weights
		m_denses[i] = Eigen::MatrixXd(m_in_dense_size[i], m_out_dense_size[i]);

		// Biases
		m_dense_biases[i] = std::vector<double>(m_out_dense_size[i]);
	}

//---------------------------------------
	// For files with kernels and weights
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

//------------------------------
	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < m_num_conv_layers;
	     ++num_conv_layers_i)
	{
		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < m_depth_of_kernels[num_conv_layers_i];
		     ++depth_of_kernels_i)
		{
			// Load kernels
			//  File name with kernels
			QString fileName =   pathToModel + "/"
			                   + m_kernels_names[num_conv_layers_i] + "/"
			                   + QString::number(depth_of_kernels_i)
			                   + ".txt";
			f.setFileName(fileName);

			//  Open file
			if (f.open(QIODevice::ReadOnly) == false)
			{
				PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
				return -1;
			}

			//  Read kernels
			if (readVectorsFromFile(f, vectors,
			                        m_num_of_kernels[num_conv_layers_i],
			                        m_kernels_rows * m_kernels_cols) != 0)
			{
				PRINT_ERR(true, "Can't read from file %s", fileName.toStdString().c_str());
				return -1;
			}

			//  Close file
			f.close();

			// Get matrix with kernels
			for (uint32_t i = 0; i < m_num_of_kernels[num_conv_layers_i]; ++i)
			{
				for (uint32_t j = 0; j < m_kernels_rows * m_kernels_cols; ++j)
				{
					m_kernels[num_conv_layers_i][depth_of_kernels_i](j, i) = vectors[i][j];
				}
			}

			// Clears vectors with file contents
			vectors.clear();

		} // Loop for depth of convolution

		// Load a biases
		//  File name with kernels
		QString fileName =   pathToModel + "/"
		                   + m_kernels_names[num_conv_layers_i] + "/"
		                   + "biases"
		                   + ".txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read biases
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        m_num_of_kernels[num_conv_layers_i]) != 0)
		{
			PRINT_ERR(true, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get biases
		m_conv_biases[num_conv_layers_i] = vectors[0];

		// Clears vectors with file contents
		vectors.clear();

	} // Loop for each convolution

//------------------------
	// Loop for each dense
	for (uint32_t num_dense_layers_i = 0;
	     num_dense_layers_i < m_num_dense_layers;
	     ++num_dense_layers_i)
	{
		// Load dense
		//  File name with dense
		QString fileName =   pathToModel + "/"
		                   + m_denses_names[num_dense_layers_i]
		                   + "/weights.txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read dense
		if (readVectorsFromFile(f, vectors,
		                        m_in_dense_size[num_dense_layers_i],
		                        m_out_dense_size[num_dense_layers_i]) != 0)
		{
			PRINT_ERR(true, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get dense matrix
		for (uint32_t row = 0; row < m_in_dense_size[num_dense_layers_i]; ++row)
		{
			for (uint32_t col = 0; col < m_out_dense_size[num_dense_layers_i]; ++col)
			{
				m_denses[num_dense_layers_i](row, col) = vectors[row][col];
			}
		}

		// Clears vectors with file contents
		vectors.clear();

		// Load biases
		//  File name with dense
		fileName =   pathToModel + "/"
		           + m_denses_names[num_dense_layers_i]
		           + "/biases.txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read biases
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        m_out_dense_size[num_dense_layers_i]) != 0)
		{
			PRINT_ERR(true, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get biases
		m_dense_biases[num_dense_layers_i] = vectors[0];

		// Clears vectors with file contents
		vectors.clear();
	}

//-------------------------------------
	// Set flag (model has been loaded)
	m_loaded = true;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t NeuralNetwork::getRecognitionLabel(const uint8_t* const r,
                                           const uint8_t* const g,
                                           const uint8_t* const b)
{
	// Check the incoming parameters
	if (r == nullptr || g == nullptr || b == nullptr)
	{
		PRINT_ERR(true, "One of the color == nullptr");
		return -1;
	}

	// Get tensors
	for (uint32_t l_row = 0; l_row < m_in_conv_size[0] - m_kernels_rows + 1; ++l_row)
	{
		for (uint32_t l_col = 0; l_col < m_in_conv_size[0] - m_kernels_cols + 1; ++l_col)
		{
			for (uint32_t k_row = 0; k_row < m_kernels_rows; ++k_row)
			{
				for (uint32_t k_col = 0; k_col < m_kernels_cols; ++k_col)
				{
					m_conv_in[0][0](
					        l_row * (m_in_conv_size[0] - m_kernels_cols + 1) + l_col,
					        k_row * m_kernels_cols + k_col) =
					    r[(l_row + k_row) * m_in_conv_size[0] + l_col + k_col] / 255.0;
					m_conv_in[0][1](
					        l_row * (m_in_conv_size[0] - m_kernels_cols + 1) + l_col,
					        k_row * m_kernels_cols + k_col) =
					    g[(l_row + k_row) * m_in_conv_size[0] + l_col + k_col] / 255.0;
					m_conv_in[0][2](
					        l_row * (m_in_conv_size[0] - m_kernels_cols + 1) + l_col,
					        k_row * m_kernels_cols + k_col) =
					    b[(l_row + k_row) * m_in_conv_size[0] + l_col + k_col] / 255.0;
				}
			}
		}
	}

	return getRecognitionLabel();
}

//-------------------------------------------------------------------------------------------------
int32_t NeuralNetwork::getRecognitionLabel(const QString& fileName)
{
	// For file with tensor data
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

	// Loop for depth
	for (uint32_t depth_of_kernels_i = 0;
	     depth_of_kernels_i < m_depth_of_kernels[0];
	     ++depth_of_kernels_i)
	{
		// Load a tensor
		//  File name with tensor
		f.setFileName(fileName + QString::number(depth_of_kernels_i) + ".txt");

		//  Open a file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read a tensor
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        m_in_conv_size[0] * m_in_conv_size[0]) != 0)
		{
			PRINT_ERR(true, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close a file
		f.close();

		// Get a tensor
		for (uint32_t l_row = 0; l_row < m_in_conv_size[0] - m_kernels_rows + 1; ++l_row)
		{
			for (uint32_t l_col = 0; l_col < m_in_conv_size[0] - m_kernels_cols + 1; ++l_col)
			{
				for (uint32_t k_row = 0; k_row < m_kernels_rows; ++k_row)
				{
					for (uint32_t k_col = 0; k_col < m_kernels_cols; ++k_col)
					{
						m_conv_in[0][depth_of_kernels_i](
						        l_row * (m_in_conv_size[0] - m_kernels_cols + 1) + l_col,
						        k_row * m_kernels_cols + k_col) =
						    vectors[0][(l_row + k_row) * m_in_conv_size[0] + l_col + k_col];
					}
				}
			}
		}

		// Clears vectors with file contents
		vectors.clear();

	} // Loop for depth

	return getRecognitionLabel();
}

//-------------------------------------------------------------------------------------------------
int32_t NeuralNetwork::getRecognitionLabel()
{
	// Check flag
	if (false == m_loaded)
	{
		PRINT_ERR(true, "Model has not been loaded");
		return -1;
	}

	// Get start time
	m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	                  std::chrono::system_clock::now().time_since_epoch()).count();

//------------------------------
	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < m_num_conv_layers;
	     ++num_conv_layers_i)
	{
		// Set output matrix elements to 0
		for (uint32_t i = 0; i < m_conv_out[num_conv_layers_i].size(); ++i)
		{
			m_conv_out[num_conv_layers_i].data()[i] = 0;
		}

		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < m_depth_of_kernels[num_conv_layers_i];
		     ++depth_of_kernels_i)
		{
			// Get a convolution result
			Eigen::MatrixXd conv = m_conv_in[num_conv_layers_i][depth_of_kernels_i] *
			                       m_kernels[num_conv_layers_i][depth_of_kernels_i];

			// Summury for all (depth) convolution result
			m_conv_out[num_conv_layers_i] += conv;

		} // Loop for depth of convolution

		// Add biases
		for (uint32_t j = 0; j < m_conv_out[num_conv_layers_i].cols(); ++j)
		{
			for (uint32_t i = 0; i < m_conv_out[num_conv_layers_i].rows(); ++i)
			{
				m_conv_out[num_conv_layers_i](i, j) += m_conv_biases[num_conv_layers_i][j];
			}
		}

		// Apply a ReLU activation function
		ReLU(m_conv_out[num_conv_layers_i].data(), m_conv_out[num_conv_layers_i].size());

		// Matrix for result of max pooling 2D
		Eigen::MatrixXd pooling(m_out_pooling_size[num_conv_layers_i] *
		                        m_out_pooling_size[num_conv_layers_i],
		                        m_conv_out[num_conv_layers_i].cols());

		// Apply a max pooling 2D
		if (maxPooling2D(m_conv_out[num_conv_layers_i], pooling,
		                 m_in_conv_size[num_conv_layers_i] - m_kernels_rows + 1,
		                 m_out_pooling_size[num_conv_layers_i]) != 0)
		{
			PRINT_ERR(true, "maxPooling2D() for layer %s",
			          m_kernels_names[num_conv_layers_i].toStdString().c_str());
			return -1;
		}

		// Flatten last convolution layer
		if (m_num_conv_layers - 1 == num_conv_layers_i)
		{
			for (uint32_t i = 0; i < pooling.rows(); ++i)
			{
				for (uint32_t j = 0; j < pooling.cols(); ++j)
				{
					m_dense_in[0](0, i * pooling.cols() + j) = pooling(i, j);
				}
			}
			continue;
		}

		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < m_depth_of_kernels[num_conv_layers_i + 1];
		     ++depth_of_kernels_i)
		{
			// Set an input for next convolution from output from this convolution
			for (uint32_t l_row = 0;
			     l_row < m_in_conv_size[num_conv_layers_i + 1] - m_kernels_rows + 1;
			     ++l_row)
			{
				for (uint32_t l_col = 0;
				     l_col < m_in_conv_size[num_conv_layers_i + 1] - m_kernels_cols + 1;
				     ++l_col)
				{
					for (uint32_t k_row = 0; k_row < m_kernels_rows; ++k_row) {
						for (uint32_t k_col = 0; k_col < m_kernels_cols; ++k_col) {
							m_conv_in[num_conv_layers_i + 1][depth_of_kernels_i](
							          l_row * (m_in_conv_size[num_conv_layers_i + 1] -
							          m_kernels_cols + 1) + l_col,
							        k_row * m_kernels_cols + k_col) =
							    pooling((l_row + k_row) * m_in_conv_size[num_conv_layers_i + 1] +
							            l_col + k_col,
							            depth_of_kernels_i);
						}
					}
				}
			}
		} // Loop for depth of convolution

	} // Loop for each convolution

//------------------------
	// Loop for each dense
	for (uint32_t num_dense_layers_i = 0;
	     num_dense_layers_i < m_num_dense_layers;
	     ++num_dense_layers_i)
	{
		// Get a result
		m_dense_out[num_dense_layers_i] = m_dense_in[num_dense_layers_i] *
		                                  m_denses[num_dense_layers_i];

		// Add biases
		for (uint32_t bias_i = 0; bias_i < m_dense_out[num_dense_layers_i].cols(); ++bias_i)
		{
			m_dense_out[num_dense_layers_i](0, bias_i) +=
			        m_dense_biases[num_dense_layers_i][bias_i];
		}

		// Apply an activation function
		if (num_dense_layers_i < m_num_dense_layers - 1)
		{
			// ReLU
			ReLU(m_dense_out[num_dense_layers_i].data(), m_dense_out[num_dense_layers_i].size());

			// Set an input from output
			m_dense_in[num_dense_layers_i + 1] = m_dense_out[num_dense_layers_i];
		}
		else
		{
			// Choice an activation function
			if (m_out_dense_size.back() == 1)
			{
				// Sigmoid
				sigmoid(m_dense_out[num_dense_layers_i].data(),
				        m_dense_out[num_dense_layers_i].size());
			}
			else
			{
				// Softmax
				softmax(m_dense_out[num_dense_layers_i].data(),
				        m_dense_out[num_dense_layers_i].size());
			}

			PRINT_DBG(true, "Index of maximal element: %lu;  value: %f",
			          static_cast<unsigned long>(
			              maxArrayElementIndex(m_dense_out[num_dense_layers_i].data(),
			                                   m_dense_out[num_dense_layers_i].size())),
			          maxArrayElement(m_dense_out[num_dense_layers_i].data(),
			                          m_dense_out[num_dense_layers_i].size()));

//			qDebug() << m_dense_out[num_dense_layers_i].data()[0]
//			         << m_dense_out[num_dense_layers_i].data()[1]
//			         << m_dense_out[num_dense_layers_i].data()[2];
		}

	} // Loop for each dense

//--------------------
	// Get finish time
	m_finishTime = std::chrono::duration_cast<std::chrono::milliseconds>(
	                   std::chrono::system_clock::now().time_since_epoch()).count();

	// Get a recognition value
	m_recognitionValue = maxArrayElement(m_dense_out[m_num_dense_layers - 1].data(),
	                                     m_dense_out[m_num_dense_layers - 1].size());

	return maxArrayElementIndex(m_dense_out[m_num_dense_layers - 1].data(),
	                            m_dense_out[m_num_dense_layers - 1].size());
}

//-------------------------------------------------------------------------------------------------
int32_t NeuralNetwork::readParameters(const QString& pathToModel)
{
	// For files with network parameters
	QFile f;

	// Variable for file content
	uint32_t var;

	// --- Get network parameters ---

	// Set a file name
	QString fileName(pathToModel + "/parameters.txt");
	f.setFileName(fileName);

	// Open a file
	if (f.open(QIODevice::ReadOnly) == false)
	{
		PRINT_ERR(true, "Can't open a file %s", fileName.toStdString().c_str());
		return -1;
	}

	// --- Get convolute layers ---

	// Read a number of convolute layers
	if (readVarFromFile(f, m_num_conv_layers) != 0)
	{
		PRINT_ERR(true, "Can't read a m_num_conv_layers from file %s",
		          fileName.toStdString().c_str());
		return -1;
	}
	if (0 == m_num_conv_layers)
	{
		PRINT_ERR(true, "m_num_conv_layers == 0");
		return -1;
	}

	// Read m_in_conv_size
	for (uint32_t i = 0; i < m_num_conv_layers; ++i)
	{
		if (readVarFromFile(f, var) != 0)
		{
			PRINT_ERR(true, "Can't read m_in_conv_size from file %s",
			          fileName.toStdString().c_str());
			return -1;
		}
		m_in_conv_size.push_back(var);
	}

	// Get m_out_pooling_size from m_in_conv_size
	std::copy(m_in_conv_size.begin() + 1, m_in_conv_size.end(),
	          std::back_insert_iterator<std::vector<uint32_t>>(m_out_pooling_size));

	// Read m_out_pooling_size
	if (readVarFromFile(f, var) != 0)
	{
		PRINT_ERR(true, "Can't read a m_out_pooling_size from file %s",
		          fileName.toStdString().c_str());
		return -1;
	}
	m_out_pooling_size.push_back(var);

	// Read m_num_of_kernels
	for (uint32_t i = 0; i < m_num_conv_layers; ++i)
	{
		if (readVarFromFile(f, var) != 0)
		{
			PRINT_ERR(true, "Can't read m_in_conv_size from file %s",
			          fileName.toStdString().c_str());
			return -1;
		}
		m_num_of_kernels.push_back(var);
	}

	// Get m_depth_of_kernels
	//  Only for RGB
	m_depth_of_kernels.push_back(3);

	//  Get m_depth_of_kernels from m_num_of_kernels
	std::copy(m_num_of_kernels.begin(), m_num_of_kernels.end() - 1,
	          std::back_insert_iterator<std::vector<uint32_t>>(m_depth_of_kernels));

	// Read m_kernels_names
	for (uint32_t i = 0; i < m_num_conv_layers; ++i)
	{
		QString str;
		if (readVarFromFile(f, str) != 0)
		{
			PRINT_ERR(true, "Can't read m_kernels_names from file %s",
			          fileName.toStdString().c_str());
			return -1;
		}
		m_kernels_names.push_back(str);
	}

	// Only kernels 3x3
	m_kernels_rows = 3;
	m_kernels_cols = 3;

	m_conv_in = std::vector<std::vector<Eigen::MatrixXd>>(m_num_conv_layers);
	m_conv_out = std::vector<Eigen::MatrixXd>(m_num_conv_layers);
	m_kernels = std::vector<std::vector<Eigen::MatrixXd>>(m_num_conv_layers);
	m_conv_biases = std::vector<std::vector<double>>(m_num_conv_layers);

	// --- Get dense layers ---

	// Read a number of dense layers
	if (readVarFromFile(f, m_num_dense_layers) != 0)
	{
		PRINT_ERR(true, "Can't read a m_num_dense_layers from file %s",
		          fileName.toStdString().c_str());
		return -1;
	}
	if (0 == m_num_dense_layers)
	{
		PRINT_ERR(true, "m_num_dense_layers == 0");
		return -1;
	}

	// Get first element of m_in_dense_size
	m_in_dense_size.push_back(m_out_pooling_size.back() * m_out_pooling_size.back() *
	                          m_num_of_kernels.back());

	// Read m_in_dense_size
	for (uint32_t i = 1; i < m_num_dense_layers; ++i)
	{
		if (readVarFromFile(f, var) != 0)
		{
			PRINT_ERR(true, "Can't read a m_in_dense_size from file %s",
			          fileName.toStdString().c_str());
			return -1;
		}
		m_in_dense_size.push_back(var);
	}

	// Get m_out_dense_size
	std::copy(m_in_dense_size.begin() + 1, m_in_dense_size.end(),
	          std::back_insert_iterator<std::vector<uint32_t>>(m_out_dense_size));

	// Read m_out_dense_size last element
	if (readVarFromFile(f, var) != 0)
	{
		PRINT_ERR(true, "Can't read a m_out_dense_size from file %s",
		          fileName.toStdString().c_str());
		return -1;
	}
	m_out_dense_size.push_back(var);

	// Read m_denses_names
	for (uint32_t i = 0; i < m_num_dense_layers; ++i)
	{
		QString str;
		if (readVarFromFile(f, str) != 0)
		{
			PRINT_ERR(true, "Can't read m_denses_names from file %s",
			          fileName.toStdString().c_str());
			return -1;
		}
		m_denses_names.push_back(str);
	}

	m_dense_in = std::vector<Eigen::MatrixXd>(m_num_dense_layers);
	m_dense_out = std::vector<Eigen::MatrixXd>(m_num_dense_layers);
	m_denses = std::vector<Eigen::MatrixXd>(m_num_dense_layers);
	m_dense_biases = std::vector<std::vector<double>>(m_num_dense_layers);

//	m_num_conv_layers = 4;
//	m_in_conv_size = {152, 75, 36, 17};
//	m_out_pooling_size = {75, 36, 17, 7};
//	m_kernels_rows = 3;
//	m_kernels_cols = 3;
//	m_num_of_kernels = {32, 64, 128, 128};
//	m_depth_of_kernels = {3, 32, 64, 128};
//	m_kernels_names = {"conv2d_12", "conv2d_13", "conv2d_14", "conv2d_15"};

//	m_num_dense_layers = 2;
//	m_in_dense_size = {7 * 7 * 128, 512};
//	m_out_dense_size = {512, 3};
//	m_denses_names = {"dense_6", "dense_7"};

//	m_num_conv_layers = 2;
//	m_in_conv_size = {70, 34};
//	m_out_pooling_size = {34, 16};
//	m_kernels_rows = 3;
//	m_kernels_cols = 3;
//	m_num_of_kernels = {32, 64};
//	m_depth_of_kernels = {3, 32};
//	m_kernels_names = {"conv2d_21", "conv2d_22"};

//	m_num_dense_layers = 2;
//	m_in_dense_size = {16 * 16 * 64, 64};
//	m_out_dense_size = {64, 1};
//	m_denses_names = {"dense_24", "dense_25"};

	// Close file
	f.close();

	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
