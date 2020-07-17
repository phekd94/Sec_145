
#include "CopterSearchRecognition.h"

#include <QByteArray>             // QByteArray class
#include "other/Other.h"          // writeVarInFile<>()
#include "algorithms/Learning.h"  // getRecognitionLabel()

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* const CopterSearchRecognition::PREF = "[CopterSearchRecognition]: ";

//-------------------------------------------------------------------------------------------------
CopterSearchRecognition::CopterSearchRecognition(const QString& pathToNetworkParams,
                                                 const LearningType modelsType,
                                                 const QString& resultName,
                                                 const uint32_t imagesWidth,
                                                 const uint32_t imagesHeight,
                                                 uint32_t incrCutArea) :
    m_learningType(modelsType),
    m_imagesWidth(imagesWidth),
    m_imagesHeight(imagesHeight),
    m_init(false),
    m_incrCutArea(incrCutArea)
{
	switch (m_learningType) {
	case LearningType::Neural_2_layer:
	{
		// Open a file with parameters of the neural network
		QFile f(pathToNetworkParams + "/l_2_b_params.txt");
		if (f.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open a file with parameters of the neural network");
			return;
		}

		// Read an number of copters
		if (readVarFromFile(f, m_numCopters) != 0) {
			PRINT_ERR(true, PREF, "readVarFromFile(m_alpha)");
			return;
		}

		// Read a number of intermediate layers
		if (readVarFromFile(f, m_hidden_size) != 0) {
			PRINT_ERR(true, PREF, "readVarFromFile(m_hidden_size)");
			return;
		}

		// Close file with parameters of the neural network
		f.close();

		// Open a file with weight (layer 0 -> layer 1)
		f.setFileName(pathToNetworkParams + "/l_2_b_w_0_1.txt");
		if (f.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open a file with weight w_0_1");
			return;
		}

		// Read a weight w_0_1
		m_w_0_1 = Eigen::MatrixXd(m_imagesWidth * m_imagesHeight, m_hidden_size);
		if (readMatrixFromFile(f, m_w_0_1) != 0) {
			PRINT_ERR(true, PREF, "readMatrixFromFile(m_w_0_1)");
			return;
		}

		// Close file with weight w_0_1
		f.close();

		// Open a file with weight (layer 1 -> layer 2)
		f.setFileName(pathToNetworkParams + "/l_2_b_w_1_2.txt");
		if (f.open(QIODevice::ReadOnly) == false) {
			PRINT_ERR(true, PREF, "Can't open a file with weight w_1_2");
			return;
		}

		// Read a weight w_1_2
		m_w_1_2 = Eigen::MatrixXd(m_hidden_size, m_numCopters);
		if (readMatrixFromFile(f, m_w_1_2) != 0) {
			PRINT_ERR(true, PREF, "readMatrixFromFile(m_w_1_2)");
			return;
		}

		// Close file with weight w_1_2
		f.close();

		// Create layers
		m_l_0 = Eigen::MatrixXd(1, m_imagesWidth * m_imagesHeight);
		m_l_1 = Eigen::MatrixXd(1, m_hidden_size);
		m_l_2 = Eigen::MatrixXd(1, m_numCopters);

		// Open a file for result (without check)
		m_f.setFileName(pathToNetworkParams + "/" + resultName + ".txt");
		m_f.open(QIODevice::WriteOnly);

		break;
	}
	default:
		PRINT_ERR(true, PREF, "Unknown learning type");
		return;
		break;
	}

	// Set initialization flag
	m_init = true;

	PRINT_DBG(m_debug, PREF, "Load neural network was successfull");
}

//-------------------------------------------------------------------------------------------------
CopterSearchRecognition::~CopterSearchRecognition()
{
	PRINT_DBG(m_debug, PREF, "");
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearchRecognition::getRecognitionResult(const QImage& image, uint32_t& copterIndex)
{
	// Set a copter index to 0
	copterIndex = 0;

	// Check the init flag
	if (m_init == false) {
		return -1;
	}

	// Check the incoming parameter
	if (image.isNull() == true) {
		PRINT_ERR(true, PREF, "Incomed image is null");
		return -1;
	}

	// Get a rectangle of area with copter
	uint32_t x, y, w, h;
	if (get_ObjParameters(x, y, w, h) != 0) {
		return -1;
	}
	if (   x + w > static_cast<uint32_t>(image.width())
	    || y + h > static_cast<uint32_t>(image.height())) {
		PRINT_ERR(true, PREF, "x + w > image.width() or y + h > image.height()");
		return -1;
	}

	// Increase a cut area
	if (m_incrCutArea > x)
		x = 0;
	else
		x -= m_incrCutArea;

	if (m_incrCutArea > y)
		y = 0;
	else
		y -= m_incrCutArea;

	if (x + w + 2 * m_incrCutArea >= static_cast<uint32_t>(image.width()))
		w = static_cast<uint32_t>(image.width()) - x;
	else
		w += 2 * m_incrCutArea;

	if (y + h + 2 * m_incrCutArea >= static_cast<uint32_t>(image.height()))
		h = static_cast<uint32_t>(image.height()) - y;
	else
		h += 2 * m_incrCutArea;

	// Cut an area with copter
	QImage imageCopter = image.copy(x, y, w, h);
	if (imageCopter.isNull() == true) {
		PRINT_ERR(true, PREF, "Copy image is null; x = %lu, y = %lu, w = %lu, h = %lu",
		          static_cast<unsigned long>(x), static_cast<unsigned long>(y),
		          static_cast<unsigned long>(w), static_cast<unsigned long>(h));
		return -1;
	}

	// Convert to Grayscale8 format
	imageCopter.convertTo(QImage::Format_Grayscale8);

	// Scale a cuted area
	QImage imageCopterScale = imageCopter.scaled(m_imagesWidth, m_imagesHeight);
	if (imageCopterScale.isNull() == true) {
		PRINT_ERR(true, PREF, "Scaled image is null");
		return -1;
	}

	// Get a results
	switch (m_learningType) {
	case LearningType::Neural_2_layer:
	{
		int32_t recognitionLabel;
		recognitionLabel = getRecognitionLabel(imageCopterScale, m_w_0_1, m_w_1_2,
		                                       m_l_0, m_l_1, m_l_2,
		                                       m_imagesWidth, m_imagesHeight);
		if (recognitionLabel < 0) {
			PRINT_ERR(true, PREF, "getRecognitionLabel()");
			return -1;
		}

		copterIndex = static_cast<uint32_t>(recognitionLabel);

		break;
	}
	default:
		return -1;
		break;
	}

	// Save result
	if (m_f.isOpen() == true) {
		if (writeVarInFile(m_f, copterIndex) != 0) {
			PRINT_ERR(true, PREF, "Can't write a result");
		}
	}

	return 0;
}
