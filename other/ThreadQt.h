
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class which inherited from specify class and running in a new thread
TODO:
FIXME:
DANGER:
NOTE:
 * Recommendation: 
   ThreadQt_object.start(); // for start a run() method 
   while (ThreadQt_object.isRunning() != true);

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
template <typename ClassInNewThread> class ThreadQt : public QThread, public ClassInNewThread
{
public:

	// Stops the thread
	void stop() noexcept
	{
		m_stop = true;
	}

	// Stops the thread (with waiting for thread to stop)
	static void stop(ThreadQt& thread) noexcept
	{
		// Stop a thread
		thread.stop();

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

		while (false == m_stop)
			ClassInNewThread::process();

		PRINT_DBG(true, PREF, "Thread with id = %llu is stoped",
		          reinterpret_cast<unsigned long long>(QThread::currentThreadId()));
	}
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
