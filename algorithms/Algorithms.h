
#ifndef SEC_145_ALGORITHMS_ALGORITHMS_H
#define SEC_145_ALGORITHMS_ALGORITHMS_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions with algoritms
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE:

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <QString>  // QString class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Learning type enumeration
enum class LearningType {
	Simple,
	Neural
};

//-------------------------------------------------------------------------------------------------
// Learning a model
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPathAndName,
                      const LearningType type,
                      const bool scale = false,
                      const bool brightness = false,
                      const uint32_t imagesWidth = 20,
                      const uint32_t imagesHeight = 20);

//-------------------------------------------------------------------------------------------------
// Calculates a correlation coefficient
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_ALGORITHMS_H
