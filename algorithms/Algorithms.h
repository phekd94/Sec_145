
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions with algoritms
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE: * "const" in lambda function definition: "const" makes the linkage internal by default,
		 which means it is only accessible in one translation unit, thereby avoiding the problem.

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <QString>  // QString class
#include <vector>   // std::vector template class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Converts pixel limits from [0, 255] to [0, 1]
const auto convertPixelLimits = [](double pixel){ return pixel /= 255; };

//-------------------------------------------------------------------------------------------------
// Calculates a dot product
double dotProduct(const double* const d_1, const double* const d_2, const uint32_t n);
double dotProduct(const std::vector<double>& d_1, const std::vector<double>& d_2);

//-------------------------------------------------------------------------------------------------
// Calculates a correlation coefficient
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n);
double correlationCoefficient(const std::vector<uint8_t>& d_1, const std::vector<uint8_t>& d_2);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
