
#include "CopterSearch_Chameleon.h"

#include "algorithms/Algorithms.h"  // correlationCoefficient()
#include <QFile>                    // QFile class
#include <QByteArray>               // QByteArray class
#include <cmath>                    // std::fabs()

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
#define PREF  "[CopterSearch_Chameleon]: "

//-------------------------------------------------------------------------------------------------
CopterSearch_Chameleon::CopterSearch_Chameleon(const QString& pathModel_1,
                                               const QString& pathModel_2,
                                               const uint32_t modelWidth,
                                               const uint32_t modelHeight) :
    m_modelImage_1(pathModel_1),
    m_modelImage_2(pathModel_2),
    m_modelWidth(modelWidth),
    m_modelHeight(modelHeight)
{
	// Check a loaded model image
	if (m_modelImage_1.isNull() == true) {
		m_modelData_1 = nullptr;
		PRINT_ERR(true, PREF, "pathModel_1 is incorrect");
	} else {
		// Get an image data
		m_modelData_1 = m_modelImage_1.bits();
		PRINT_DBG(m_debug, PREF, "Load from pathModel_1 was successfull");
	}

	// Check a loaded model image
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
                                                       double& p_1, double& p_2,
                                                       const QString& pathForSave) const
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

	// Save result (if need)
	//static bool flag_open = false;
	if (pathForSave.isNull() == false) {
		QFile f_p_1(pathForSave + "/p_1.txt");
		QFile f_p_2(pathForSave + "/p_2.txt");

		// Open files
		if (   f_p_1.open(QIODevice::WriteOnly | QIODevice::Append) == false
		    || f_p_2.open(QIODevice::WriteOnly | QIODevice::Append) == false) {
			PRINT_ERR(true, PREF, "Can't open one of the files in WriteOnly mode");
		} else {
			// Write data
			f_p_1.write(QByteArray::number(static_cast<int>(p_1)) + QByteArray("\n"));
			f_p_2.write(QByteArray::number(static_cast<int>(p_2)) + QByteArray("\n"));
			// Close files
			f_p_1.close();
			f_p_2.close();
		}
	}

	return 0;
}
