
#include "CopterSearch_Chameleon.h"

#include "algorithms/Algorithms.h"  // correlationCoefficient()
#include <QDir>                     // QDir class

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
#define PREF  "[CopterSearch_Chameleon]: "

//-------------------------------------------------------------------------------------------------
CopterSearch_Chameleon::CopterSearch_Chameleon(const QString& pathModel_1,
                                              const QString& pathModel_2,
                                              const uint32_t modelWidth,
                                              const uint32_t modelHeight) :
    m_modelWidth(modelWidth),
    m_modelHeight(modelHeight)
{
	// Load a model image
	m_modelImage_1 = QImage(pathModel_1);
	if (m_modelImage_1.isNull() == true) {
		m_modelData_1 = nullptr;
		PRINT_ERR(true, PREF, "pathModel_1 is incorrect");
	} else {
		// Get an image data
		m_modelData_1 = m_modelImage_1.bits();
		PRINT_DBG(m_debug, PREF, "Load from pathModel_1 was successfull");
	}

	// Load a model image
	m_modelImage_2 = QImage(pathModel_2);
	if (m_modelImage_2.isNull() == true) {
		m_modelData_2 = nullptr;
		PRINT_ERR(true, PREF, "pathModel_2 is incorrect");
	} else {
		// Get an image data
		m_modelData_2 = m_modelImage_2.bits();
		PRINT_DBG(m_debug, PREF, "Load from pathModel_2 was successfull");
	}
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearch_Chameleon::getRecognitionPercents(const QImage& image,
                                                       double& p_1, double& p_2) const
{
	p_1 = p_2 = 0;

	// Check the incoming parameter
	if (image.isNull() == true) {
		PRINT_ERR(true, PREF, "Incomed image is null");
		return -1;
	}

	// Check the models data
	if (nullptr == m_modelData_1 && nullptr == m_modelData_2) {
		PRINT_ERR(true, PREF, "Images was not loaded");
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

	// Get percents
	p_1 = correlationCoefficient(data, m_modelData_1, m_modelWidth * m_modelHeight) * 100;
	p_2 = correlationCoefficient(data, m_modelData_2, m_modelWidth * m_modelHeight) * 100;

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t CopterSearch_Chameleon::modelLearning(const QString& pathCopterImages,
                                              const QString& resultPathAndName,
                                              const bool scale,
                                              const bool brightness,
                                              const uint32_t imagesWidth,
                                              const uint32_t imagesHeight)
{
	// Create a directory class object and check it exist
	QDir dir(pathCopterImages);
	if (dir.exists() == false) {
		PRINT_ERR(true, PREF, "Directory is not exist");
		return -1;
	}

	// Get an images list
	QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

	// Get images
	std::vector<QImage> images;
	for (auto file : entries) {

		// Scale an inmage (if need)
		if (true == scale) {
			// Get an image
			QImage image = QImage(file.filePath());

			// Leave only 0 and 255 pixels
			uint8_t* data = image.bits();
			for (uint32_t i = 0; i < image.sizeInBytes(); ++i)
				if (data[i] < 255)
					data[i] = 0;

			// Add an image to vector
			images.push_back(image.scaled(imagesWidth, imagesHeight));
		} else {

			// Add an image to vector
			images.push_back(QImage(file.filePath()));
		}

		// Check the size of image and entrance size
		if (   static_cast<uint32_t>(images.back().width()) != imagesWidth
		    || static_cast<uint32_t>(images.back().height()) != imagesHeight) {
			PRINT_ERR(true, PREF, "Size of the %s image is not correct",
			                      file.filePath().toStdString().c_str());
			return -1;
		}
	}

	// Get a brightness
	double b = 255.0 / (imagesWidth * imagesHeight);

	// Result (double)
	std::vector<double> res_double(imagesWidth * imagesHeight, 0);

	// Get a result (double)
	// Images loop
	for (auto image : images) {

		// Pixels loop
		for (uint32_t i = 0; i < imagesWidth * imagesHeight; ++i) {

			// Get data and process it
			uint8_t* data = image.bits();
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
	QImage(res_uint8.data(),
	       imagesWidth, imagesHeight,
	       QImage::Format_Grayscale8).save(resultPathAndName);

	return 0;
}
