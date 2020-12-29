
#include "Other.h"

#include <algorithm>    // std::equal()
#include <QMetaMethod>  // QMetaMethod class

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
// Application preface in debug message
static const char* const PREF = "[Sec_145]: ";

//-------------------------------------------------------------------------------------------------
template <>
int32_t writeVarInFile<double>(QFile& file, const double& var)
{
	if (file.write(QByteArray::number(var, 'f', 3) + "\n") == -1)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t writeBinVarInFile(QFile& file, const uint8_t& var)
{
	if (file.write(reinterpret_cast<const char*>(&var), 1) == -1)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t writeBinArrayInFile(QFile& file, const uint8_t* const data, const uint32_t size)
{
	if (file.write(reinterpret_cast<const char*>(data), size) == -1)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, uint32_t& var)
{
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = static_cast<uint32_t>(ar.toUInt(&ok));
	if (false == ok)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, int32_t& var)
{
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = static_cast<int32_t>(ar.toInt(&ok));
	if (false == ok)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, double& var)
{
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = ar.toDouble(&ok);
	if (false == ok)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, QString& var)
{
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	var = QString(ar);
	if (var.isEmpty() == true)
	{
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}

	// Remove "\r\n" from result string
	var.remove(var.size() - 2, 2);

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVectorsFromFile(QFile& file, std::vector<std::vector<double>>& vectors,
                            const uint32_t vectors_num, const uint32_t vector_size)
{
	for (uint32_t n = 0; n < vectors_num; ++n)
	{
		vectors.push_back(std::vector<double>(vector_size));
		for (uint32_t i = 0; i < vector_size; ++i)
		{
			if (readVarFromFile(file, vectors.back()[i]) != 0)
			{
				PRINT_ERR(true, PREF, "n = %lu, i = %lu",
				          static_cast<unsigned long>(n),
				          static_cast<unsigned long>(i));
				return -1;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t compareFiles(QFile& file_1, QFile& file_2, const uint32_t num, bool& res)
{
	res = false;

	// Open files
	if (file_1.open(QIODevice::ReadOnly) == false)
	{
		PRINT_ERR(true, PREF, "Can't open a file 1; name: %s",
		          file_1.fileName().toStdString().c_str());
		return -1;
	}
	if (file_2.open(QIODevice::ReadOnly) == false)
	{
		PRINT_ERR(true, PREF, "Can't open a file 2; name: %s",
		          file_2.fileName().toStdString().c_str());
		file_1.close();
		return -1;
	}

	// Read files contents
	std::vector<std::vector<double>> vectors_1;
	if (readVectorsFromFile(file_1, vectors_1, 1, num) != 0)
	{
		PRINT_ERR(true, PREF, "Can't read data from file 1");
		file_1.close();
		file_2.close();
		return -1;
	}
	std::vector<std::vector<double>> vectors_2;
	if (readVectorsFromFile(file_2, vectors_2, 1, num) != 0)
	{
		PRINT_ERR(true, PREF, "Can't read data from file 2");
		file_1.close();
		file_2.close();
		return -1;
	}

	// Compare contents
	res = std::equal(vectors_1[0].begin(), vectors_1[0].end(), vectors_2[0].begin());

	// Check the rest data in files
	if (file_1.readLine().isEmpty() != true || file_2.readLine().isEmpty() != true)
	{
		PRINT_DBG(true, PREF, "Files have extra data");
	}

	// Close files
	file_1.close();
	file_2.close();

	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t reflection(QObject *obj,
                   const QString methodName,
                   QGenericReturnArgument returnValue,
                   QGenericArgument val0,
                   QGenericArgument val1,
                   QGenericArgument val2,
                   QGenericArgument val3,
                   QGenericArgument val4,
                   QGenericArgument val5,
                   QGenericArgument val6,
                   QGenericArgument val7,
                   QGenericArgument val8,
                   QGenericArgument val9)
{
	QMetaMethod method;

	// Check the incoming parameter
	if (nullptr == obj)
	{
		PRINT_ERR(true, PREF, "nullptr == obj");
		return -1;
	}

	// Look for a method
	for (int32_t i = obj->metaObject()->methodOffset();
	     i < obj->metaObject()->methodCount();
	     ++i)
	{
		if (obj->metaObject()->method(i).name() == methodName)
		{
			method = obj->metaObject()->method(i);
			break;
		}
	}

	// Call found method
	if (method.isValid() == false)
	{
		PRINT_ERR(true, PREF, "Method was not found");
		return -1;
	}
	else
	{
		bool ret = method.invoke(obj,
		                         Qt::AutoConnection,
		                         returnValue,
		                         val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
		if (ret == false)
		{
			PRINT_ERR(true, PREF, "One of the parameters for method call is not correct");
			return -1;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
