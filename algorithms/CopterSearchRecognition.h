
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class for copter recognition
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE:

Sec_145::CopterSearchRecognition
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |    YES     |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "CopterSearch.h"         // CopterSearch class (for inheritance)
#include <QImage>                 // QImage class
#include <QString>                // QString class
#include "Sec_145/algorithms/Learning.h"  // LearningType enum
#include <QFile>                  // QFile class
#include "Eigen/Dense"            // Eigen::MatrixXd class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class CopterSearchRecognition : public CopterSearch
{

public:
	CopterSearchRecognition(const QString& pathToNetworkParams,
	                        const QString& resultName,
	                        const uint32_t imagesWidth,
	                        const uint32_t imagesHeight,
	                        uint32_t incrCutArea);
	virtual ~CopterSearchRecognition();

	// Gets recognition percents
	int32_t getRecognitionResult(const QImage& image, uint32_t& copterIndex);

private:

	// Number of copters for recognition
	uint32_t m_numCopters;

	// File for result
	QFile m_f;

	// Width and height of input image
	uint32_t m_imagesWidth;
	uint32_t m_imagesHeight;

	// Weights
	Eigen::MatrixXd m_w_0_1;
	Eigen::MatrixXd m_w_1_2;

	// Layers
	Eigen::MatrixXd m_l_0;
	Eigen::MatrixXd m_l_1;
	Eigen::MatrixXd m_l_2;

	// Initialization flag
	bool m_init;

	// Number of intermediate layers
	uint32_t m_hidden_size;

	// For increase a cut area
	uint32_t m_incrCutArea;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
