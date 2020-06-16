
#include "Algorithms.h"

#include <cmath>  // std::sqrt(), std::abs()

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n)
{
	uint64_t sum_d_1 = 0, sum_d_2 = 0, sum_d_1_d_2 = 0;
	uint64_t squareSum_d_1 = 0, squareSum_d_2 = 0;

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
	double corr = static_cast<double>((n * sum_d_1_d_2 - sum_d_1 * sum_d_2)) /
	              std::sqrt((n * squareSum_d_1 - sum_d_1 * sum_d_1) *
	                        (n * squareSum_d_2 - sum_d_2 * sum_d_2));


	// Return an absolute value
	return std::abs(corr);
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
