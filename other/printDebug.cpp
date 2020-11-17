
#include "printDebug.h"

#include <cstdio>     // std::vsprintf(), std::sprintf()
#include <cstdarg>    // va_list macro, std::va_start(), va_end macro
#include <cstdint>    // integer types
#include <mutex>      // std::mutex class; std::lock_guard<> class
#include <exception>  // std::exception

#ifdef SEC_145_QT_EN
#include <QTime>    // currentTime()
#endif  // SEC_145_QT_EN

//-------------------------------------------------------------------------------------------------
// Application preface in debug message
static const char* const PREF = "[Sec_145]: ";

//-------------------------------------------------------------------------------------------------
bool CHECK_PRINTF_ERROR(const int ret, 
                        const char* const pref,
                        const char* const func, 
                        const char* const format) noexcept
{
	if (ret < 0) 
	{
		const uint32_t BUF_SIZE = 500;
		char out[BUF_SIZE];
		if (std::sprintf(out, "%s%s%s(): %s(): "
		                      "format encoding error occurred; format: \n\t\"%s\"",
		                      PREF, pref, func, __FUNCTION__, format) < 0)
		{
            #ifdef SEC_145_QT_EN
			qDebug() << PREF << __FUNCTION__ << ": std::sprintf() error";
            #else
			std::cout << PREF << __FUNCTION__ << ": sprintf() error" << std::endl;
            #endif  // SEC_145_QT_EN
			return false;
		}
        #ifdef SEC_145_QT_EN
		qDebug() << QTime::currentTime().toString("hh:mm:ss") << out;
        #else
		std::cout << out << std::endl;
        #endif  // SEC_145_QT_EN
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
                         ...) noexcept
{
	const uint32_t BUF_SIZE = 500;

	char buf[BUF_SIZE], userMsg[BUF_SIZE], out[BUF_SIZE + BUF_SIZE];
	va_list argList;
	int ret;
	static std::mutex mutex;

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
	ret = std::vsprintf(userMsg, format, argList);
	va_end(argList);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	ret = std::sprintf(buf, "%s%s%s%s(): ", PREF, pref, err ? "ERROR: " : "", func);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	// ret = std::fprintf(stdout, "%s%s\n", buf, userMsg);
	ret = std::sprintf(out, "%s%s", buf, userMsg);
	if (CHECK_PRINTF_ERROR(ret, pref, func, format) == false)
		return;

	try {
	std::lock_guard<std::mutex> lock(mutex);

    #ifdef SEC_145_QT_EN
	qDebug() << QTime::currentTime().toString("hh:mm:ss") << out;
    #else
	std::cout << out << std::endl;
    #endif  // SEC_145_QT_EN

	}
	catch (std::exception& obj)
	{
        #ifdef SEC_145_QT_EN
		qDebug() << PREF << pref << func << "(): " << __FUNCTION__ << "(): "
		         << "Exception from mutex.lock() or from Qt functions has been occured: "
		         << obj.what();
        #else
		std::cout << PREF << pref << func << "(): " << __FUNCTION__ << "(): "
		          << "Exception from mutex.lock() or from Qt functions has been occured: "
		          << obj.what() << std::endl;
        #endif  // SEC_145_QT_EN
	}
}
