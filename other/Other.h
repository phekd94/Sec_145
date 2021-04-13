
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Helper functions
TODO:
 * One readVarFromFile() function. Use type_id
 * test functions
 * handle exceptions
 * property(): try use <type_traits> for type identification
FIXME:
DANGER:
NOTE:
 * reflection() and property() need a Q_DECLARE_METATYPE(type), where type is a type of
   user-defined type (or Qt-defined type)

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
#include <QString>     // QString class
#include <QObject>     // QObjects classes and macroses

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Write variable in file
template <typename T>
int32_t writeVarInFile(QFile& file, const T& var)
{
	if (file.write(QByteArray::number(var) + "\n") == -1)
	{
		PRINT_ERR("Bad data");
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
	for (uint32_t i = 0; i < size; ++i)
	{
		if (writeVarInFile(file, data[i]) != 0)
		{
			PRINT_ERR("writeVarInFile()");
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
	for (uint32_t i = 0; i < m.rows(); ++i)
	{
		for (uint32_t j = 0; j < m.cols(); ++j)
		{
			if (writeVarInFile(file, m(i, j)) != 0)
			{
				PRINT_ERR("writeVarInFile()");
				return -1;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Write binary variable in file
int32_t writeBinVarInFile(QFile& file, const uint8_t& var);

//-------------------------------------------------------------------------------------------------
// Write binary array in file
int32_t writeBinArrayInFile(QFile& file, const uint8_t* const data, const uint32_t size);

//-------------------------------------------------------------------------------------------------
// Read variable from file
int32_t readVarFromFile(QFile& file, uint32_t& var);
int32_t readVarFromFile(QFile& file, int32_t& var);
int32_t readVarFromFile(QFile& file, double& var);
int32_t readVarFromFile(QFile& file, QString& var);

//-------------------------------------------------------------------------------------------------
// Read matrix from file
template <typename Matrix>
int32_t readMatrixFromFile(QFile& file, Matrix& m)
{
	for (uint32_t i = 0; i < m.rows(); ++i)
	{
		for (uint32_t j = 0; j < m.cols(); ++j)
		{
			if (readVarFromFile(file, m(i, j)) != 0)
			{
				PRINT_ERR("readVarFromFile()");
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
// Calls a method by name from given class
int32_t reflection(QObject *obj,
                   const QString methodName,
                   QGenericReturnArgument returnValue,
                   QGenericArgument val0 = QGenericArgument(),
                   QGenericArgument val1 = QGenericArgument(),
                   QGenericArgument val2 = QGenericArgument(),
                   QGenericArgument val3 = QGenericArgument(),
                   QGenericArgument val4 = QGenericArgument(),
                   QGenericArgument val5 = QGenericArgument(),
                   QGenericArgument val6 = QGenericArgument(),
                   QGenericArgument val7 = QGenericArgument(),
                   QGenericArgument val8 = QGenericArgument(),
                   QGenericArgument val9 = QGenericArgument());

//-------------------------------------------------------------------------------------------------
// Gets a property by name from given class
/*template<typename T>
int32_t property(QObject *obj,
                 const QString propertyName,
                 T** property)
{
	// Check the incoming parameter
	if (nullptr == obj)
	{
		PRINT_ERR("nullptr == obj");
		return -1;
	}

	// Get a QVariant class object
	QVariant v = obj->property(propertyName.toStdString().c_str());

	// Get a property itself from QVariant class object
	if (v.isValid() == false)
	{
		PRINT_ERR("%s does not exist in the given class",
		          propertyName.toStdString().c_str());
		return -1;
	}
	else
	{
		// Check the match of the property and parameter types
		if (v.userType() == QMetaType::type((std::string(typeid(T).name()) + "*").c_str()))
		{
			if (nullptr != property)
			{
				*property = v.value<T*>();
				return 0;
			}
			else
			{
				PRINT_ERR("nullptr == property");
				return -1;
			}
		}
		else
		{
			PRINT_ERR("Property and parameter types do not match");
			return -1;
		}
	}
}

//-------------------------------------------------------------------------------------------------
// Gets a property by name from given class
template<typename T>
int32_t property(QObject *obj,
                 const QString propertyName,
                 T& property)
{
	// Check the incoming parameter
	if (nullptr == obj)
	{
		PRINT_ERR("nullptr == obj");
		return -1;
	}

	// Get a QVariant class object
	QVariant v = obj->property(propertyName.toStdString().c_str());

	// Get a property itself from QVariant class object
	if (v.isValid() == false)
	{
		PRINT_ERR("%s does not exist in the given class",
		          propertyName.toStdString().c_str());
		return -1;
	}
	else
	{
		// Check the match of the property and parameter types
		if (v.userType() == QMetaType::type(typeid(T).name()))
		{
			property = v.value<T>();
			return 0;
		}
		else
		{
			PRINT_ERR("Property and parameter types do not match");
			return -1;
		}
	}
}*/


//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
