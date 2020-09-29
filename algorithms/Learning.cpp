
#include "Learning.h"

#include <cmath>                  // std::tanh(); std::exp()
#include <vector>                 // std::vector<> class
#include <QImage>                 // QImage class
#include <QFile>                  // QFile class
#include <algorithm>              // std::max<>()
#include <chrono>                 // std::chrono functions
#include "Sec_145/other/Other.h"  // readVectorsFromFile<>()
#include "Algorithms.h"           // sumArrayExpElements<>();
                                  // maxArrayElement<>(); maxArrayElementIndex<>()

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// Preface in debug message
const char* const PREF = "[Learning]: ";

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
		PRINT_ERR(true, PREF, "Wrong size of input or output matrix");
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
// Convolute layers

// Number of convolute layers
const uint32_t num_conv_layers {4};

// Incoming size for convolute layers
const uint32_t in_conv_size[num_conv_layers] {152, 75, 36, 17};

// Outgoing size for max pooling layers
const uint32_t out_pooling_size[num_conv_layers] {75, 36, 17, 7};

// Rows and columns of kernels
const uint32_t kernels_rows {3};
const uint32_t kernels_cols {3};

// Number of kernels
const uint32_t num_of_kernels[num_conv_layers] {32, 64, 128, 128};

// Depth of kernels
const uint32_t depth_of_kernels[num_conv_layers] {3, 32, 64, 128};

// Names of kernels layers
const QString kernels_names[num_conv_layers]
{
	"conv2d_12", "conv2d_13", "conv2d_14", "conv2d_15"
};

// Inputs for convolute layers
static std::vector<std::vector<Eigen::MatrixXd>> conv_in(num_conv_layers);

// Outputs for convolute layers
static std::vector<Eigen::MatrixXd> conv_out(num_conv_layers);

// Kernels
static std::vector<std::vector<Eigen::MatrixXd>> kernels(num_conv_layers);

// Biases for convolute layers
static std::vector<std::vector<double>> conv_biases(num_conv_layers);

//-------------------------------------------------------------------------------------------------
// Dense layers

// Number of dense layers
const uint32_t num_dense_layers {2};

// Incoming size for dense layers
const uint32_t in_dense_size[num_dense_layers] {7 * 7 * 128, 512};

// Outgoing size for dense layers
const uint32_t out_dense_size[num_dense_layers] {512, 3};

// Names of denses layers
const QString denses_names[num_dense_layers]
{
	"dense_6", "dense_7"
};

// Inputs for dense layers
static std::vector<Eigen::MatrixXd> dense_in(num_dense_layers);

// Outputs for dense layers
static std::vector<Eigen::MatrixXd> dense_out(num_dense_layers);

// Dense matrixes with weights
static std::vector<Eigen::MatrixXd> denses(num_dense_layers);

// Biases for dense layers
static std::vector<std::vector<double>> dense_biases(num_dense_layers);

//-------------------------------------------------------------------------------------------------
int32_t getRecognitionLabel()
{	
	// Get start time
	uint64_t ms_start = std::chrono::duration_cast<std::chrono::milliseconds>(
	                        std::chrono::system_clock::now().time_since_epoch()).count();

//------------------------------
	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < num_conv_layers;
	     ++num_conv_layers_i)
	{
		// Set output matrix elements to 0
		for (uint32_t i = 0; i < conv_out[num_conv_layers_i].size(); ++i)
		{
			conv_out[num_conv_layers_i].data()[i] = 0;
		}

		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i];
		     ++depth_of_kernels_i)
		{
			// Get a convolution result
			Eigen::MatrixXd conv = conv_in[num_conv_layers_i][depth_of_kernels_i] *
			                       kernels[num_conv_layers_i][depth_of_kernels_i];

			// Summury for all (depth) convolution result
			conv_out[num_conv_layers_i] += conv;

		} // Loop for depth of convolution

		// Add biases
		for (uint32_t j = 0; j < conv_out[num_conv_layers_i].cols(); ++j)
		{
			for (uint32_t i = 0; i < conv_out[num_conv_layers_i].rows(); ++i)
			{
				conv_out[num_conv_layers_i](i, j) += conv_biases[num_conv_layers_i][j];
			}
		}

		// Apply a ReLU activation function
		ReLU(conv_out[num_conv_layers_i].data(), conv_out[num_conv_layers_i].size());

		// Matrix for result of max pooling 2D
		Eigen::MatrixXd pooling(out_pooling_size[num_conv_layers_i] *
		                        out_pooling_size[num_conv_layers_i],
		                        conv_out[num_conv_layers_i].cols());

		// Apply a max pooling 2D
		if (maxPooling2D(conv_out[num_conv_layers_i], pooling,
		                 in_conv_size[num_conv_layers_i] - kernels_rows + 1,
		                 out_pooling_size[num_conv_layers_i]) != 0)
		{
			PRINT_ERR(true, PREF, "maxPooling2D() for layer %s",
			          kernels_names[num_conv_layers_i].toStdString().c_str());
			return -1;
		}

		// Flatten last convolution layer
		if (num_conv_layers - 1 == num_conv_layers_i)
		{
			for (uint32_t i = 0; i < pooling.rows(); ++i)
			{
				for (uint32_t j = 0; j < pooling.cols(); ++j)
				{
					dense_in[0](0, i * pooling.cols() + j) = pooling(i, j);
				}
			}
			continue;
		}

		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i + 1];
		     ++depth_of_kernels_i)
		{
			// Set an input for next convolution from output from this convolution
			for (uint32_t l_row = 0;
			     l_row < in_conv_size[num_conv_layers_i + 1] - kernels_rows + 1;
			     ++l_row)
			{
				for (uint32_t l_col = 0;
				     l_col < in_conv_size[num_conv_layers_i + 1] - kernels_cols + 1;
				     ++l_col)
				{
					for (uint32_t k_row = 0; k_row < kernels_rows; ++k_row) {
						for (uint32_t k_col = 0; k_col < kernels_cols; ++k_col) {
							conv_in[num_conv_layers_i + 1][depth_of_kernels_i](
							          l_row * (in_conv_size[num_conv_layers_i + 1] -
							          kernels_cols + 1) + l_col,
							        k_row * kernels_cols + k_col) =
							    pooling((l_row + k_row) * in_conv_size[num_conv_layers_i + 1] +
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
	     num_dense_layers_i < num_dense_layers;
	     ++num_dense_layers_i)
	{
		// Get a result
		dense_out[num_dense_layers_i] = dense_in[num_dense_layers_i] * denses[num_dense_layers_i];

		// Add biases
		for (uint32_t bias_i = 0; bias_i < dense_out[num_dense_layers_i].cols(); ++bias_i)
		{
			dense_out[num_dense_layers_i](0, bias_i) += dense_biases[num_dense_layers_i][bias_i];
		}

		// Apply an activation function
		if (num_dense_layers_i < num_dense_layers - 1)
		{
			// ReLU
			ReLU(dense_out[num_dense_layers_i].data(), dense_out[num_dense_layers_i].size());

			// Set an input from output
			dense_in[num_dense_layers_i + 1] = dense_out[num_dense_layers_i];
		}
		else
		{
			// Softmax
			softmax(dense_out[num_dense_layers_i].data(), dense_out[num_dense_layers_i].size());

			PRINT_DBG(true, PREF, "Index of maximal element: %lu;  value: %f",
			          static_cast<unsigned long>(
			              maxArrayElementIndex(dense_out[num_dense_layers_i].data(),
			                                   dense_out[num_dense_layers_i].size())),
			          maxArrayElement(dense_out[num_dense_layers_i].data(),
			                          dense_out[num_dense_layers_i].size()));
		}

	} // Loop for each dense

//--------------------
	// Get finish time
	uint64_t ms_finish = std::chrono::duration_cast<std::chrono::milliseconds>(
	                        std::chrono::system_clock::now().time_since_epoch()).count();

	qDebug() << ms_finish - ms_start;

	return maxArrayElementIndex(dense_out[num_dense_layers - 1].data(),
	                            dense_out[num_dense_layers - 1].size());
}

//-------------------------------------------------------------------------------------------------
int32_t getRecognitionLabel(const QString& fileName)
{
	// For file with tensor data
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

	// Loop for depth
	for (uint32_t depth_of_kernels_i = 0;
	     depth_of_kernels_i < depth_of_kernels[0];
	     ++depth_of_kernels_i)
	{

		// Load tensor
		//  File name with tensor
		f.setFileName(fileName + QString::number(depth_of_kernels_i) + ".txt");

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read tensor
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        in_conv_size[0] * in_conv_size[0]) != 0)
		{
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get tensor
		for (uint32_t l_row = 0; l_row < in_conv_size[0] - kernels_rows + 1; ++l_row)
		{
			for (uint32_t l_col = 0; l_col < in_conv_size[0] - kernels_cols + 1; ++l_col)
			{
				for (uint32_t k_row = 0; k_row < kernels_rows; ++k_row)
				{
					for (uint32_t k_col = 0; k_col < kernels_cols; ++k_col)
					{
						conv_in[0][depth_of_kernels_i](
						        l_row * (in_conv_size[0] - kernels_cols + 1) + l_col,
						        k_row * kernels_cols + k_col) =
						    vectors[0][(l_row + k_row) * in_conv_size[0] + l_col + k_col];
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
int32_t getRecognitionLabel(const QImage& image)
{
	// / 255

	return -1;
}

//-------------------------------------------------------------------------------------------------
int32_t loadModel(const QString& pathToModel)
{
//-----------------------------
	// For each convolute layer
	for (uint32_t i = 0; i < num_conv_layers; ++i)
	{
		// Input
		for (uint32_t j = 0; j < depth_of_kernels[i]; ++j)
		{
			conv_in[i].push_back(
			            Eigen::MatrixXd((in_conv_size[i] - kernels_cols + 1) *
			                            (in_conv_size[i] - kernels_rows + 1),
			                            kernels_rows * kernels_cols));
		}

		// Output
		conv_out[i] = Eigen::MatrixXd((in_conv_size[i] - kernels_cols + 1) *
		                              (in_conv_size[i] - kernels_rows + 1),
		                              num_of_kernels[i]);

		// Kernels
		for (uint32_t j = 0; j < depth_of_kernels[i]; ++j)
		{
			kernels[i].push_back(
			            Eigen::MatrixXd(kernels_rows * kernels_cols, num_of_kernels[i]));
		}

		// Biases
		conv_biases[i] = std::vector<double>(num_of_kernels[i]);
	}

//-------------------------
	// For each dense layer
	for (uint32_t i = 0; i < num_dense_layers; ++i)
	{
		// Input
		dense_in[i] = Eigen::MatrixXd(1, in_dense_size[i]);

		// Output
		dense_out[i] = Eigen::MatrixXd(1, out_dense_size[i]);

		// Weights
		denses[i] = Eigen::MatrixXd(in_dense_size[i], out_dense_size[i]);

		// Biases
		dense_biases[i] = std::vector<double>(out_dense_size[i]);
	}

//---------------------------------------
	// For files with kernels and weights
	QFile f;

	// For files contents
	std::vector<std::vector<double>> vectors;

//------------------------------
	// Loop for each convolution
	for (uint32_t num_conv_layers_i = 0;
	     num_conv_layers_i < num_conv_layers;
	     ++num_conv_layers_i)
	{

		// Loop for depth of convolution
		for (uint32_t depth_of_kernels_i = 0;
		     depth_of_kernels_i < depth_of_kernels[num_conv_layers_i];
		     ++depth_of_kernels_i)
		{

			// Load kernels
			//  File name with kernels
			QString fileName =   pathToModel + "/"
			                   + kernels_names[num_conv_layers_i] + "/"
			                   + QString::number(depth_of_kernels_i)
			                   + ".txt";
			f.setFileName(fileName);

			//  Open file
			if (f.open(QIODevice::ReadOnly) == false)
			{
				PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
				return -1;
			}

			//  Read kernels
			if (readVectorsFromFile(f, vectors,
			                        num_of_kernels[num_conv_layers_i],
			                        kernels_rows * kernels_cols) != 0)
			{
				PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
				return -1;
			}

			//  Close file
			f.close();

			// Get matrix with kernels
			for (uint32_t i = 0; i < num_of_kernels[num_conv_layers_i]; ++i)
			{
				for (uint32_t j = 0; j < kernels_rows * kernels_cols; ++j)
				{
					kernels[num_conv_layers_i][depth_of_kernels_i](j, i) = vectors[i][j];
				}
			}

			// Clears vectors with file contents
			vectors.clear();

		} // Loop for depth of convolution

		// Load a biases
		//  File name with kernels
		QString fileName =   pathToModel + "/"
		                   + kernels_names[num_conv_layers_i] + "/"
		                   + "biases"
		                   + ".txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read biases
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        num_of_kernels[num_conv_layers_i]) != 0)
		{
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get biases
		conv_biases[num_conv_layers_i] = vectors[0];

		// Clears vectors with file contents
		vectors.clear();

	} // Loop for each convolution

//------------------------
	// Loop for each dense
	for (uint32_t num_dense_layers_i = 0;
	     num_dense_layers_i < num_dense_layers;
	     ++num_dense_layers_i)
	{

		// Load dense
		//  File name with dense
		QString fileName =   pathToModel + "/"
		                   + denses_names[num_dense_layers_i]
		                   + "/weights.txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read dense
		if (readVectorsFromFile(f, vectors,
		                        in_dense_size[num_dense_layers_i],
		                        out_dense_size[num_dense_layers_i]) != 0)
		{
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get dense matrix
		for (uint32_t row = 0; row < in_dense_size[num_dense_layers_i]; ++row)
		{
			for (uint32_t col = 0; col < out_dense_size[num_dense_layers_i]; ++col)
			{
				denses[num_dense_layers_i](row, col) = vectors[row][col];
			}
		}

		// Clears vectors with file contents
		vectors.clear();

		// Load biases
		//  File name with dense
		fileName =   pathToModel + "/"
		           + denses_names[num_dense_layers_i]
		           + "/biases.txt";
		f.setFileName(fileName);

		//  Open file
		if (f.open(QIODevice::ReadOnly) == false)
		{
			PRINT_ERR(true, PREF, "Can't open a file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Read biases
		if (readVectorsFromFile(f, vectors,
		                        1,
		                        out_dense_size[num_dense_layers_i]) != 0)
		{
			PRINT_ERR(true, PREF, "Can't read from file %s", fileName.toStdString().c_str());
			return -1;
		}

		//  Close file
		f.close();

		// Get biases
		dense_biases[num_dense_layers_i] = vectors[0];

		// Clears vectors with file contents
		vectors.clear();
	}

//-----------
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
