
#include "Other.h"

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
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
} // namespace Sec_145
