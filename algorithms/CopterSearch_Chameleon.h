
#ifndef SEC_145_ALGORITHMS_COPTER_SEARCH_CHAMELEON_H
#define SEC_145_ALGORITHMS_COPTER_SEARCH_CHAMELEON_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class for copter search for chameleon project
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE:

Sec_145::CopterSearch_Chameleon
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
#include <variant>                  // std::variant template class; std::get function

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class CopterSearch_Chameleon : public CopterSearch
{

public:
	CopterSearch_Chameleon(const std::vector<QString>& pathModels, const LearningType modelsType,
	                       const uint32_t modelWidth = 20, const uint32_t modelHeight = 20);

	// Gets recognition percents
	int32_t getRecognitionPercents(const QImage& image, std::vector<double>& percents,
	                               const QString& pathForSave = QString()) const;

private:

	// Type of models
	LearningType m_modelsType;

	// Name and number of copters models
	std::vector<QString> m_modelsName;
	uint32_t m_number;

	// Data of copters models
	typedef std::vector<std::vector<uint8_t>>  uint8_Data;
	typedef std::vector<std::vector<double>>   double_Data;

	std::variant<uint8_Data, double_Data> m_modelsData;

	// Width and height of models
	uint32_t m_modelWidth;
	uint32_t m_modelHeight;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_COPTER_SEARCH_CHAMELEON_H
