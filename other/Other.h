
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Helper functions
TODO: * PREF
	  * One readVarFromFile() function. Use type_id
	  * test functions
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

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Write variable in file
template <typename T>
int32_t writeVarInFile(QFile& file, const T& var)
{
	const char* const PREF = "[Other]: ";
	if (file.write(QByteArray::number(var) + "\n") == -1) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Write variable in file (explicit specialization for double data type)
template <>
int32_t writeVarInFile<double>(QFile& file, const double& var);

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
// Read variable from file
int32_t readVarFromFile(QFile& file, uint32_t& var);
int32_t readVarFromFile(QFile& file, int32_t& var);
int32_t readVarFromFile(QFile& file, double& var);

//-------------------------------------------------------------------------------------------------
// Read matrix from file
template <typename Matrix>
int32_t readMatrixFromFile(QFile& file, Matrix& m)
{
	const char* const PREF = "[Other]: ";
	for (uint32_t i = 0; i < m.rows(); ++i) {
		for (uint32_t j = 0; j < m.cols(); ++j) {
			if (readVarFromFile(file, m(i, j)) != 0) {
				PRINT_ERR(true, PREF, "readVarFromFile()");
				return -1;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Read vectors with double numbers from file
int32_t readVectorsFromFile(QFile& file, std::vector<std::vector<double>>& vectors,
                            const uint32_t vectors_num, const uint32_t vector_size);

//-------------------------------------------------------------------------------------------------
// Compare two files with double numbers
int32_t compareFiles(QFile& file_1, QFile& file_2, const uint32_t num, bool& res);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
