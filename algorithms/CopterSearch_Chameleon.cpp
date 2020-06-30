
#include "CopterSearch_Chameleon.h"

#include "algorithms/Algorithms.h"  // correlationCoefficient(), dotProduct()
#include <QFile>                    // QFile class
#include <QByteArray>               // QByteArray class
#include <cmath>                    // std::fabs()

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
#define PREF  "[CopterSearch_Chameleon]: "

//-------------------------------------------------------------------------------------------------
CopterSearch_Chameleon::CopterSearch_Chameleon(const std::vector<QString>& pathModels,
                                               const LearningType modelsType,
                                               const uint32_t modelWidth,
                                               const uint32_t modelHeight) :
    m_modelsType(modelsType),
    m_modelWidth(modelWidth),
    m_modelHeight(modelHeight)
{
	m_number = static_cast<uint32_t>(pathModels.size());
	m_modelsName.resize(m_number);

	switch (m_modelsType) {
	case LearningType::Simple:
		m_modelsData = uint8_Data();

		break;
	case LearningType::Neural:
		m_modelsData = double_Data();
		break;
	default:
		m_number = 0;
		m_modelsName.resize(m_number);
		return;
		break;
	}

	// Loop for all model
	for (uint32_t i = 0; i < m_number; ++i) {

		// Get a model name
		m_modelsName[i] = QString("c_") + QString::number(i) + ".txt";

		// Simple learning type
		if (LearningType::Simple == m_modelsType) {

			// Get a model image
			QImage modelImage = QImage(pathModels[i]);

			// Check a loaded model image
			if (modelImage.isNull() == true) {
				PRINT_ERR(true, PREF, "pathModel[%lu] content is incorrect",
				          static_cast<unsigned long>(i));
				continue;
			} else if (   static_cast<uint32_t>(modelImage.width()) != m_modelWidth
			           || static_cast<uint32_t>(modelImage.height()) != m_modelHeight) {
				PRINT_ERR(true, PREF, "Size of model image %lu is incorrect",
				          static_cast<unsigned long>(i));
				continue;
			} else {
				// Get an image data
				std::get<uint8_Data>(m_modelsData).push_back(
				            std::vector<uint8_t>(modelImage.bits()[0],
				                                 modelImage.bits()[modelImage.sizeInBytes()]));
			}

		// Neural learning type
		} else if (LearningType::Simple == m_modelsType) {
			QFile f_m(pathModels[i]);
			if (f_m.open(QIODevice::ReadOnly) == false) {
				PRINT_ERR(true, PREF, "Can't open %lu files in ReadOnly mode",
				          static_cast<unsigned long>(i));
				continue;
			} else {
				std::vector<double> modelData;
				for (uint32_t j = 0; j < m_modelWidth * m_modelHeight; ++j) {
					QByteArray qba = f_m.readLine();
					if (qba.isEmpty() == true) {
						modelData = std::vector<double>();
						break;
					}
					modelData.push_back(qba.toDouble());
				}
				std::get<double_Data>(m_modelsData).push_back(modelData);
			}
		}
	}

	PRINT_DBG(m_debug, PREF, "Load from pathModels was successfull");
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearch_Chameleon::getRecognitionPercents(const QImage& image,
                                                       std::vector<double>& percents,
                                                       const QString& pathForSave) const
{
	// Initialization a vector with models images
	percents.resize(m_number, 0);

	// Check the incoming parameter
	if (image.isNull() == true) {
		PRINT_ERR(true, PREF, "Incomed image is null");
		return -1;
	}

	// Check the models data
	/*for (auto modelData : m_modelsData) {
		if (nullptr == modelData) {
			PRINT_ERR(true, PREF, "Model images was not loaded");
			return -1;
		}
	}*/

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

	// Cut a area with copter
	QImage imageCopter = image.copy(x, y, w, h);

	// Get data of this area and convert it
	uint8_t* data = imageCopter.bits();
	for (uint32_t i = 0; i < imageCopter.sizeInBytes(); ++i)
		if (data[i] < 255)
			data[i] = 0;

	// Scale a cuted area
	QImage imageCopterScale = imageCopter.scaled(m_modelWidth, m_modelHeight);
	data = imageCopterScale.bits();

	std::vector<double> ol(&data[0], &data[20*20]);
	for (auto & el : ol)
		el /= 255;
	///double hui = dotProduct(m_d.data(), ol.data(), m_modelWidth * m_modelHeight);
	//qDebug() << hui = dotProduct(m_d.data(), ol.data(), m_modelWidth * m_modelHeight);
	// Open file
	QFile f_p("C:/Users/ekd/Desktop/ppp.txt");
	if (f_p.open(QIODevice::WriteOnly | QIODevice::Append) == false) {
		PRINT_ERR(true, PREF, "Can't open one of the files in WriteOnly mode");
		return -1;
	} else {
		// Write data
		////f_p.write(QByteArray::number(hui) + QByteArray("\n"));
		// Close file
		f_p.close();
	}

	// Get percents
	for (uint32_t i = 0; i < m_number; ++i) {
		////percents[i] = correlationCoefficient(data, m_modelsData[i],
		////                                     m_modelWidth * m_modelHeight) * 100;
	}

	// Save result (if need)
	if (pathForSave.isNull() == false) {

		// For all model
		for (uint32_t i = 0; i < m_number; ++i) {

			// Open file
			QFile f_p(pathForSave + "/" + m_modelsName[i]);
			if (f_p.open(QIODevice::WriteOnly | QIODevice::Append) == false) {
				PRINT_ERR(true, PREF, "Can't open one of the files in WriteOnly mode");
				return -1;
			} else {
				// Write data
				f_p.write(QByteArray::number(static_cast<int>(percents[i])) + QByteArray("\n"));
				// Close file
				f_p.close();
			}

		}
	}

	return 0;
}
