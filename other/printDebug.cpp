
#include "printDebug.h"

#include <cstdio>   // vsprintf(), sprintf()
#include <cstdarg>  // va_list, va_start(), va_end()

#include <QDebug>   // qDebug()
#include <QTime>    // currentTime()

//-------------------------------------------------------------------------------------------------
#define PREF  "[]: "

//-------------------------------------------------------------------------------------------------
#define CHECK_PRINTF_ERROR()                                              \
	do {                                                                  \
	    if (ret < 0)                                                      \
        {                                                                 \
	        qDebug() << PREF << pref << func << "(): "                    \
	                 << __FUNCTION__ << "(): "                            \
	                 << "format encoding error occurred; format: \n\t\""  \
	                 << format << "\"";                                   \
	        return;                                                       \
	    }                                                                 \
	} while(0)

//-------------------------------------------------------------------------------------------------
void Sec_145::printDebug(const char* pref, const char* func, bool err, const char* format, ...)
{
  #define  BUF_SIZE  100

	char buf[BUF_SIZE], userMsg[BUF_SIZE], out[BUF_SIZE + BUF_SIZE];
	va_list argList;
	int ret;

	if (nullptr == pref || nullptr == func || nullptr == format)
	{
		if (nullptr == pref)
			pref = "nullptr";
		if (nullptr == func)
			func = "nullptr";
		if (nullptr == format)
			format = "nullptr";
		ret = -1;
		CHECK_PRINTF_ERROR();
	}

	va_start(argList, format);
	ret = vsprintf(userMsg, format, argList);
	va_end(argList);
	CHECK_PRINTF_ERROR();

	ret = sprintf(buf, PREF "%s%s%s(): ", pref, err ? "ERROR: " : "", func);
	CHECK_PRINTF_ERROR();

	// ret = fprintf(stdout, "%s%s\n", buf, userMsg);
	ret = sprintf(out, "%s%s", buf, userMsg);
	CHECK_PRINTF_ERROR();

	qDebug() << QTime::currentTime().toString("hh:mm:ss") << out;

  #undef  BUF_SIZE
}
