
#ifndef SEC_145_ALGORITHMS_COPTER_SEARCH_RECOGNITION_H
#define SEC_145_ALGORITHMS_COPTER_SEARCH_RECOGNITION_H

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
#include "CopterSearch.h"           // CopterSearch class (for inheritance)
#include <QImage>                   // QImage class
#include <QString>                  // QString class
#include "algorithms/Algorithms.h"  // LearningType enum
#include <variant>                  // std::variant template class; std::get() function
#include <QFile>                    // QFile class
#include <vector>                   // std::vector template class

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

	// Type of models
	LearningType m_modelsType;

	// Number of copters models
	uint32_t m_number;

	// typedefs for data of copters models
	typedef std::vector<std::vector<uint8_t>>  uint8_Data;
	typedef std::vector<std::vector<double>>   double_Data;

	// Data of copters models
	std::variant<uint8_Data, double_Data> m_modelsData;

	// Files for results
	std::vector<QFile> m_f;

	// Width and height of models
	uint32_t m_modelWidth;
	uint32_t m_modelHeight;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_COPTER_SEARCH_RECOGNITION_H
