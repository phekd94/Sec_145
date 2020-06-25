
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

	// Learning type enumeration
	enum class LearningType {
		Simple,
		Neural
	};

	CopterSearch_Chameleon(const QString& pathModel_1, const QString& pathModel_2,
	                       const uint32_t modelWidth = 20, const uint32_t modelHeight = 20);

	// Gets recognition percents
	int32_t getRecognitionPercents(const QImage& image, double& p_1, double& p_2) const;

	// Learning; gets a model
	static int32_t modelLearning(const QString& pathCopterImages,
	                             const QString& resultPathAndName,
	                             const LearningType type,
	                             const bool scale = false,
	                             const bool brightness = false,
	                             const uint32_t imagesWidth = 20,
	                             const uint32_t imagesHeight = 20);

private:

	// Model of copters
	QImage m_modelImage_1;
	QImage m_modelImage_2;
	uint8_t* m_modelData_1;
	uint8_t* m_modelData_2;

	// Width and height of model
	uint32_t m_modelWidth;
	uint32_t m_modelHeight;

	// Simple learning
	static const std::vector<uint8_t> simpleLearning(const std::vector<QImage>& images,
	                                                 const bool brightness,
	                                                 const uint32_t imagesWidth,
	                                                 const uint32_t imagesHeight);

	// Neural learning
	static const std::vector<uint8_t> neuralLearning(const std::vector<QImage>& images,
	                                                 const uint32_t imagesWidth,
	                                                 const uint32_t imagesHeight);

};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_COPTER_SEARCH_CHAMELEON_H
