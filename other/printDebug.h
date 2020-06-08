
#ifndef SEC_145_PRINTDEBUG_H
#define SEC_145_PRINTDEBUG_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: macroses and function for print debug messages
TODO:
FIXME:
DANGER:
NOTE: * __FUNCTION__ is a __func__ synonym
*/

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
#define PRINT_DBG(enable, pref, ...)                             \
	do {                                                         \
	    if (enable)                                              \
	        printDebug(pref, __FUNCTION__, false, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
#define PRINT_ERR(enable, pref, ...)                            \
	do {                                                        \
	    if (enable)                                             \
	        printDebug(pref, __FUNCTION__, true, __VA_ARGS__);  \
	} while(0)

//-------------------------------------------------------------------------------------------------
void printDebug(const char* pref, const char* func, bool err, const char* format, ...);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_PRINTDEBUG_H
