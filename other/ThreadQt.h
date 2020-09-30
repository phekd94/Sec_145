
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Template class which inherited from specify class and running in a new thread
TODO:
FIXME:
DANGER:
NOTE:

Sec_145::ThreadQT
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <QThread>  // QThread class (for inheritance)

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Template class which inherited from specify class and running it in a new thread
template <typename ClassInNewThread> class ThreadQt : public QThread, public ClassInNewThread
{
public:

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

	// Stops the thread
	void stop() noexcept
	{
		m_stop = true;
	}

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[ThreadQT]: "};

	// Flag; for stop an execution thread
	volatile bool m_stop {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
