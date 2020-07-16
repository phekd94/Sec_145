
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Helper functions
TODO: * PREF
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
#include <cstdint>     // integer types
#include <QFile>       // QFile class
#include <QByteArray>  // QByteArray class

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Write variable in file
template <typename T>
int32_t writeVarInFile(QFile& file, const T& data)
{
	const char* const PREF = "[Other]: ";
	QByteArray ar;
	ar = ar.setNum(data);
	if (file.write(ar + "\n") == -1) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Write variable in file (explicit specialization for double data type)
template <>
int32_t writeVarInFile<double>(QFile& file, const double& data)
{
	const char* const PREF = "[Other]: ";
	QByteArray ar;
	ar = ar.setNum(data, 'f');
	if (file.write(ar + "\n") == -1) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Write array in file
template <typename Array>
int32_t writeArrayInFile(QFile& file, const Array* const data, const uint32_t size)
{
	const char* const PREF = "[Other]: ";
	for (uint32_t i = 0; i < size; ++i) {
		if (writeVarInFile(file, data[i]) != 0) {
			PRINT_ERR(true, PREF, "writeVarInFile()");
			return -1;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Write matrix in file
template <typename Matrix>
int32_t writeMatrixInFile(QFile& file, const Matrix& m)
{
	const char* const PREF = "[Other]: ";
	for (uint32_t i = 0; i < m.rows(); ++i) {
		for (uint32_t j = 0; j < m.cols(); ++j) {
			if (writeVarInFile(file, m(i, j)) != 0) {
				PRINT_ERR(true, PREF, "writeVarInFile()");
				return -1;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
