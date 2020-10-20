
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions with algoritms
TODO:
 * test class (logic() method)
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
#include <vector>   // std::vector template class
#include <cmath>    // std::exp(); DBL_MIN

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Maximum array element
template <typename Array>
double maxArrayElement(Array* const array, const uint32_t size)
{
	double res = DBL_MIN;
	for (uint32_t i = 0; i < size; ++i) {
		if (array[i] >= res)
			res = array[i];
	}
	return res;
}

//-------------------------------------------------------------------------------------------------
// Index of maximum array element
template <typename Array>
uint32_t maxArrayElementIndex(Array* const array, const uint32_t size)
{
	double max = DBL_MIN;
	uint32_t res = 0;
	for (uint32_t i = 0; i < size; ++i) {
		if (array[i] >= max) {
			max = array[i];
			res = i;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------------
// Minimal array element
template <typename Array>
double minArrayElement(Array* const array, const uint32_t size)
{
	double res = DBL_MAX;
	for (uint32_t i = 0; i < size; ++i) {
		if (array[i] <= res)
			res = array[i];
	}
	return res;
}

//-------------------------------------------------------------------------------------------------
// Index of minimal array element
template <typename Array>
uint32_t minArrayElementIndex(Array* const array, const uint32_t size)
{
	double min = DBL_MAX;
	uint32_t res = 0;
	for (uint32_t i = 0; i < size; ++i) {
		if (array[i] <= min) {
			min = array[i];
			res = i;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------------
// Sum of array elements
template <typename DataType>
double sumArrayElements(DataType* const data, const uint32_t size)
{
	double sum = 0;
	for (uint32_t i = 0; i < size; ++i) {
		sum += data[i];
	}
	return sum;
}

//-------------------------------------------------------------------------------------------------
// Sum of exponents of array elements
template <typename DataType>
double sumArrayExpElements(DataType* const data, const uint32_t size)
{
	double sum = 0;
	for (uint32_t i = 0; i < size; ++i) {
		sum += std::exp(data[i]);
	}
	return sum;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
