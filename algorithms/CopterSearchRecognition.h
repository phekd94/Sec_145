
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class for copter search for chameleon project
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
#include "algorithms/Learning.h"  // LearningType enum
#include <QFile>                  // QFile class
#include <vector>                 // std::vector template class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class CopterSearchRecognition : public CopterSearch
{

public:
	CopterSearchRecognition(const std::vector<QString>& pathModels,
	                        const LearningType modelsType,
	                        const QString& resultPath = QString(),
	                        const uint32_t modelWidth = 20,
	                        const uint32_t modelHeight = 20);
	virtual ~CopterSearchRecognition();

	// Gets recognition percents
	int32_t getRecognitionResult(const QImage& image, std::vector<double>& results);

private:

	// Preface in debug message
	static const char* const PREF;

	// Type of models
	LearningType m_modelsType;

	// Number of copters models
	uint32_t m_number;

	// Files for results
	std::vector<QFile> m_f;

	// Width and height of models
	uint32_t m_modelWidth;
	uint32_t m_modelHeight;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
