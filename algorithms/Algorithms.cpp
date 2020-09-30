
#include "Algorithms.h"

#include <cmath>      // std::sqrt(), std::pow()
#include <cstdlib>    // std::abs()
#include <vector>     // std::vector template class
#include <QImage>     // QImage class
#include <QDir>       // QDir class
#include <QFile>      // QFile class
#include <algorithm>  // std::transform() template function

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// Preface in debug message
static const char* const PREF = "[Algorithms]: ";

//-------------------------------------------------------------------------------------------------
double dotProduct(const double* const d_1, const double* const d_2, const uint32_t n)
{
	// Check the incoming parameters
	if (nullptr == d_1 || nullptr == d_2) {
		PRINT_ERR(true, PREF, "nullptr == d_1 or nullptr == d_2");
		return 0;
	}

	// Calculate a dot product
	double output = 0;
	for (uint32_t i = 0; i < n; ++i) {
		output += d_1[i] * d_2[i];
	}
	return output;
}

//-------------------------------------------------------------------------------------------------
double dotProduct(const std::vector<double>& d_1, const std::vector<double>& d_2)
{
	if (d_1.size() != d_2.size()) {
		PRINT_ERR(true, PREF, "Container sizes do not match");
		return 0;
	} else {
		return dotProduct(d_1.data(), d_2.data(), static_cast<uint32_t>(d_1.size()));
	}
}

//-------------------------------------------------------------------------------------------------
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n)
{
	double sum_d_1 = 0, sum_d_2 = 0, sum_d_1_d_2 = 0;
	double squareSum_d_1 = 0, squareSum_d_2 = 0;

	// Check the incoming parameters
	if (nullptr == d_1 || nullptr == d_2) {
		PRINT_ERR(true, PREF, "nullptr == d_1 or nullptr == d_2");
		return 0;
	}

	for (uint32_t i = 0; i < n; ++i)
	{
		// Sum of elements of array d_1
		sum_d_1 += d_1[i];

		// Sum of elements of array d_2
		sum_d_2 += d_2[i];

		// Sum of d_1[i] * d_2[i]
		sum_d_1_d_2 += d_1[i] * d_2[i];

		// Sum of square of array elements
		squareSum_d_1 += d_1[i] * d_1[i];
		squareSum_d_2 += d_2[i] * d_2[i];
	}

	// Use a formula for calculating correlation coefficient
	double res = (n * sum_d_1_d_2 - sum_d_1 * sum_d_2) /
	              std::sqrt((n * squareSum_d_1 - sum_d_1 * sum_d_1) *
	                        (n * squareSum_d_2 - sum_d_2 * sum_d_2));

	// Return an absolute value
	return std::abs(res);
}

//-------------------------------------------------------------------------------------------------
double correlationCoefficient(const std::vector<uint8_t>& d_1, const std::vector<uint8_t>& d_2)
{
	if (d_1.size() != d_2.size()) {
		PRINT_ERR(true, PREF, "Container sizes do not match");
		return 0;
	} else {
		return correlationCoefficient(d_1.data(), d_2.data(), static_cast<uint32_t>(d_1.size()));
	}
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
