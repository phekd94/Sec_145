
#include "Other.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
template <>
int32_t writeVarInFile<double>(QFile& file, const double& var)
{
	const char* const PREF = "[Other]: ";
	if (file.write(QByteArray::number(var, 'f') + "\n") == -1) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, uint32_t& var)
{
	const char* const PREF = "[Other]: ";
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = static_cast<uint32_t>(ar.toUInt(&ok));
	if (false == ok) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, int32_t& var)
{
	const char* const PREF = "[Other]: ";
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = static_cast<int32_t>(ar.toInt(&ok));
	if (false == ok) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
int32_t readVarFromFile(QFile& file, double& var)
{
	const char* const PREF = "[Other]: ";
	QByteArray ar = file.readLine();
	if (ar.isEmpty() == true) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	bool ok;
	var = ar.toDouble(&ok);
	if (false == ok) {
		PRINT_ERR(true, PREF, "Bad data");
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
