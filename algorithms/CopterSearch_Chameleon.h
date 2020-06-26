
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
#include "CopterSearch.h"  // CopterSearch class (for inheritance)
#include <QImage>          // QImage class
#include <QString>         // QString class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class CopterSearch_Chameleon : public CopterSearch
{

public:
	CopterSearch_Chameleon(const QString& pathModel_1, const QString& pathModel_2,
	                       const uint32_t modelWidth = 20, const uint32_t modelHeight = 20);

	// Gets recognition percents
	int32_t getRecognitionPercents(const QImage& image, double& p_1, double& p_2,
	                               const QString& pathForSave = QString()) const;

private:

	// Model of copters
	QImage m_modelImage_1;
	QImage m_modelImage_2;
	uint8_t* m_modelData_1;
	uint8_t* m_modelData_2;

	// Width and height of model
	uint32_t m_modelWidth;
	uint32_t m_modelHeight;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_COPTER_SEARCH_CHAMELEON_H
