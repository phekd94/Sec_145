
#include "CopterSearchRecognition.h"

#include "algorithms/Algorithms.h"  // correlationCoefficient(), dotProduct()
#include <QByteArray>               // QByteArray class
#include <cmath>                    // std::fabs()
#include <algorithm>                // std::transform() template function

#include "other/printDebug.h"       // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* const CopterSearchRecognition::PREF = "[CopterSearchRecognition]: ";

//-------------------------------------------------------------------------------------------------
CopterSearchRecognition::CopterSearchRecognition(const std::vector<QString>& pathModels,
                                                 const LearningType modelsType,
                                                 const QString& resultPath,
                                                 const uint32_t modelWidth,
                                                 const uint32_t modelHeight) :
    m_modelsType(modelsType),
    m_modelWidth(modelWidth),
    m_modelHeight(modelHeight)
{
	m_number = static_cast<uint32_t>(pathModels.size());
	m_f = std::vector<QFile>(m_number);

	switch (m_modelsType) {
	default:
		m_number = 0;
		PRINT_ERR(true, PREF, "Unknown learning type");
		return;
		break;
	}

	// Loop for all model
	for (uint32_t i = 0; i < m_number; ++i) {

		// Open a file for write a results (if need)
		if (resultPath.isNull() == false) {
			m_f[i].setFileName(resultPath + "/c_" + QString::number(i) + ".txt");
			if (m_f[i].open(QIODevice::WriteOnly) == false) {
				PRINT_ERR(true, PREF, "Can't open %lu file in WriteOnly mode",
				          static_cast<unsigned long>(i));
			}
		}

		// Get a model
		switch (m_modelsType) {
		case LearningType::Neural_2_layer:
		{
			break;
		}
		}
	}

	PRINT_DBG(m_debug, PREF, "Load models was successfull");
}

//-------------------------------------------------------------------------------------------------
CopterSearchRecognition::~CopterSearchRecognition()
{
	// Close files for write results
	for (auto & file : m_f) {
		file.close();
	}
	PRINT_DBG(m_debug, PREF, "Files were closed");
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearchRecognition::getRecognitionResult(const QImage& image,
                                                      std::vector<double>& results)
{
	// Initialization a vector with models images
	results.resize(m_number, 0);

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

	// Cut an area with copter
	QImage imageCopter = image.copy(x, y, w, h);
	if (imageCopter.isNull() == true) {
		PRINT_ERR(true, PREF, "Copy image is null; x = %lu, y = %lu, w = %lu, h = %lu",
		          static_cast<unsigned long>(x), static_cast<unsigned long>(y),
		          static_cast<unsigned long>(w), static_cast<unsigned long>(h));
		return -1;
	}

	// Get data of this area and convert it
	uint8_t* data = imageCopter.bits();
	for (uint32_t i = 0; i < imageCopter.sizeInBytes(); ++i) {
		if (data[i] < 255)
			data[i] = 0;
	}

	// Scale a cuted area
	QImage imageCopterScale = imageCopter.scaled(m_modelWidth, m_modelHeight);
	if (imageCopterScale.isNull() == true) {
		PRINT_ERR(true, PREF, "Scaled image is null");
		return -1;
	}
	data = imageCopterScale.bits();

	// Get a results
	for (uint32_t i = 0; i < m_number; ++i) {
		switch (m_modelsType) {
		case LearningType::Neural_2_layer:
		{

			break;
		}
		default:
			return -1;
			break;
		}
	}

	// Save results
	for (uint32_t i = 0; i < m_number; ++i) {

		// Write data
		if (m_f[i].isOpen() == true) {
			m_f[i].write(QByteArray::number(results[i]) + QByteArray("\n"));
		}
	}

	return 0;
}
