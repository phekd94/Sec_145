
#include "printDebug.h"

#include <cstdio>   // vsprintf(), sprintf()
#include <cstdarg>  // va_list, va_start(), va_end()
#include <cstdint>  // integer types

#include <iostream>  // std::cout; std::endl

// #include <QDebug>   // qDebug()
// #include <QTime>    // currentTime()

//-------------------------------------------------------------------------------------------------
// Application preface in debug message
const char* const PREF = "[Sec_145]: ";

//-------------------------------------------------------------------------------------------------
bool CHECK_PRINTF_ERROR(const int ret, 
                        const char* const pref,
                        const char* const func, 
                        const char* const format)
{
	if (ret < 0) 
	{
		const uint32_t BUF_SIZE = 500;
		char out[BUF_SIZE];
		if (sprintf(out, "%s%s%s(): %s(): "
		                 "format encoding error occurred; format: \n\t\"%s\"",
		                 PREF, pref, func, __FUNCTION__, format) < 0) 
		{
			// qDebug() << PREF << __FUNCTION__ << ": sprintf() error";
			std::cout << PREF << __FUNCTION__ << ": sprintf() error" << std::endl;
			return false;
		}
		// qDebug() << QTime::currentTime().toString("hh:mm:ss") << out;
		std::cout << out << std::endl;
		return false;
	} 
	else 
	{
		return true;
	}
}

//-------------------------------------------------------------------------------------------------
void Sec_145::printDebug(const char* pref, 
                         const char* func, 
                         const bool err, 
                         const char* format, 
                         ...)
{
	const uint32_t BUF_SIZE = 500;

	char buf[BUF_SIZE], userMsg[BUF_SIZE], out[BUF_SIZE + BUF_SIZE];
	va_list argList;
	int ret;

	if (nullptr == pref || nullptr == func || nullptr == format)
	{
		if (nullptr == pref)
			pref = "nullptr ";
		if (nullptr == func)
			func = "nullptr ";
		if (nullptr == format)
			format = "nullptr";
		CHECK_PRINTF_ERROR(-1, pref, func, format);
		return;
	}

	va_start(argList, format);
	ret = vsprintf(userMsg, format, argList);
	va_end(argList);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	ret = sprintf(buf, "%s%s%s%s(): ", PREF, pref, err ? "ERROR: " : "", func);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	// ret = fprintf(stdout, "%s%s\n", buf, userMsg);
	ret = sprintf(out, "%s%s", buf, userMsg);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	// qDebug() << QTime::currentTime().toString("hh:mm:ss") << out;
	std::cout << out << std::endl;
}
