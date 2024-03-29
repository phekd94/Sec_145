
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: macroses and function for print debug messages
TODO:
 * print via ostream
FIXME:
DANGER:
NOTE:
 * __FUNCTION__ is a __func__ synonym

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |    YES     |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#ifdef SEC_145_QT_EN
#include <QDebug>    // qDebug()
#else
#include <iostream>  // std::cout; std::endl
#endif  // SEC_145_QT_EN

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
#define PRINT_DBG(enable, ...)                                      \
	do {                                                            \
	    if (enable)                                                 \
	        Sec_145::printDebug(__FUNCTION__, false, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
#define PRINT_ERR(...)                                         \
	do {                                                       \
	    Sec_145::printDebug(__FUNCTION__, true, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
void printDebug(const char* func,
                const bool err,
                const char* format,
                ...) noexcept;

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
