
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class which inherited from specify class and running in a new thread
TODO:
 * test class
FIXME:
DANGER:
NOTE:
 * Recommendation: 
   ThreadQt_object.start(); // for start a run() method 
   while (ThreadQt_object.isRunning() != true);
   ...
   Sec_145::ThreadQt<Class_of_ThreadQt_object>::stop(ThreadQt_object);

Sec_145::ThreadQT
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <QThread>    // QThread class (for inheritance)
#include <exception>  // std::exception

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class which inherited from specify class and running it in a new thread
template <typename ClassInNewThread, bool useQtExecMethod>
class ThreadQt : public QThread, public ClassInNewThread
{
public:

	ThreadQt() = default;

	// Uses move constructor for ClassInNewThread class
	ThreadQt(ClassInNewThread && ClassInNewThread_obj) :
	    ClassInNewThread(std::move(ClassInNewThread_obj))
	{
		PRINT_DBG(true, PREF, "ClassInNewThread move constructor");
	}

	// Stops the thread
	void stop() noexcept
	{
		m_stop = true;
	}

	// Stops the thread (with waiting for thread to stop)
	static void stop(ThreadQt& thread) noexcept
	{
		// Stop a thread
		if constexpr (useQtExecMethod)
		{
			// QThread::quit() is not mark as "noexcept", but it is slot therefore it should not
			// throw exceptions
			thread.quit();
		}
		else
		{
			thread.stop();
		}

		// Catch an exceptions from QThread::isRunning()
		try {
			// Waiting
			while (thread.isRunning() == true);
		}
		catch (std::exception& obj)
		{
			PRINT_ERR(true, PREF,
			          "Exception (%s) during call QThread::isRunning() has been occured",
			          obj.what());
			return;
		}
	}

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[ThreadQt]: "};

	// Flag; for stop an execution thread
	volatile bool m_stop {false};

	// Overrides a run() method from QThread class
	virtual void run() override
	{
		PRINT_DBG(true, PREF, "Thread with id = %llu is started",
		          reinterpret_cast<unsigned long long>(QThread::currentThreadId()));

		// Catch an exceptions from QThread::exec() or from ClassInNewThread::process()
		try {
			// Use exec() Qt method or user defined process() method
			if constexpr (useQtExecMethod)
			{
				exec();
			}
			else
			{
				while (false == m_stop)
					ClassInNewThread::process();
			}
		}
		catch (std::exception& obj)
		{
			PRINT_ERR(true, PREF,
			          "Exception (%s) during call %s has been occured",
			          useQtExecMethod == true ? "QThread::exec()" : "ClassInNewThread::process()",
			          obj.what());
			return;
		}

		PRINT_DBG(true, PREF, "Thread with id = %llu is stoped",
		          reinterpret_cast<unsigned long long>(QThread::currentThreadId()));
	}
};

//-------------------------------------------------------------------------------------------------
// ThreadQt type for use with exec()
template<typename ClassInNewThread>
using ThreadQt_exec = ThreadQt<ClassInNewThread, true>;

// ThreadQt type for use with process()
template<typename ClassInNewThread>
using ThreadQt_process = ThreadQt<ClassInNewThread, false>;

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
