
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
bool CHECK_PRINTF_ERROR(const int ret,
                        const char* const func, 
                        const char* const format) noexcept
{
	if (ret < 0) 
	{
		const uint32_t BUF_SIZE = 500;
		char out[BUF_SIZE];
		if (std::sprintf(out, "%s(): %s(): "
		                      "format encoding error occurred; format: \n\t\"%s\"",
		                      func, __FUNCTION__, format) < 0)
		{
            #ifdef SEC_145_QT_EN
			qDebug() << __FUNCTION__ << ": std::sprintf() error";
            #else
			std::cout << __FUNCTION__ << ": sprintf() error" << std::endl;
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
void Sec_145::printDebug(const char* func,
                         const bool err, 
                         const char* format, 
                         ...) noexcept
{
	const uint32_t BUF_SIZE = 500;

	char buf[BUF_SIZE], userMsg[BUF_SIZE], out[BUF_SIZE + BUF_SIZE];
	va_list argList;
	int ret;

	// printDebug() for all threads
	static std::mutex mutex;

	if (nullptr == func || nullptr == format)
	{
		if (nullptr == func)
			func = "nullptr ";
		if (nullptr == format)
			format = "nullptr";
		CHECK_PRINTF_ERROR(-1, func, format);
		return;
	}

	va_start(argList, format);
	ret = std::vsprintf(userMsg, format, argList);
	va_end(argList);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
		return;

	ret = std::sprintf(buf, "%s%s(): ", err ? "ERROR: " : "", func);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
		return;

	// ret = std::fprintf(stdout, "%s%s\n", buf, userMsg);
	ret = std::sprintf(out, "%s%s", buf, userMsg);
	if (CHECK_PRINTF_ERROR(ret, func, format) == false)
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
		qDebug() << func << "(): " << __FUNCTION__ << "(): "
		         << "Exception from mutex.lock() or from Qt functions has been occured: "
		         << obj.what();
        #else
		std::cout << func << "(): " << __FUNCTION__ << "(): "
		          << "Exception from mutex.lock() or from Qt functions has been occured: "
		          << obj.what() << std::endl;
        #endif  // SEC_145_QT_EN
	}
}

//-------------------------------------------------------------------------------------------------
// Template print
/*
enum class eDebug : bool {
	ENABLE = true,
	DISABLE = false
};

void print()
{

}

template<typename T>
void print(const T& value)
{
	std::cout << value << std::endl;
}

template<typename T, typename... Args>
void print(const T& value, const Args&... args)
{
	std::cout << value;
	print(args...);
}

template<typename... Args>
void print(const eDebug& enable, const Args&... args)
{
	if (enable == eDebug::ENABLE)
		print(args...);
}
*/
