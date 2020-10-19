
#include "NeuralNetworkThread.h"

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
void NeuralNetworkThread::process() noexcept
{
	// Catch a lock(), unlock() xxxxxxxxxxx exceptions
	try {

	// Lock a mutex
	m_mutex.lock();

	// Check new data flag
	if (false == m_new_data)
	{
		// Unlock a mutex
		m_mutex.unlock();

		return;
	}

	// Set working flag
	m_is_working = true;

	// Unlock a mutex
	m_mutex.unlock();

	// Get a label
	if (m_image.isNull() != true)
	{
		uint32_t magic_number = 70; // 152
		m_image = m_image.scaled(magic_number, magic_number);
		if (m_image.isNull() != true)
		{
			const uint8_t* const data = m_image.bits();
			for (uint32_t i = 0; i < magic_number * magic_number; ++i)
			{
				// m_r[i] = data[i * 4 + 0];
				// m_g[i] = data[i * 4 + 1];
				// m_b[i] = data[i * 4 + 2];
				m_r[i] = data[i * 4 + 2];
				m_g[i] = data[i * 4 + 1];
				m_b[i] = data[i * 4 + 0];
			}
			m_recognitionLabel = NeuralNetwork::getRecognitionLabel(m_r, m_g, m_b);
		}
	}

	// Lock a mutex
	m_mutex.lock();

	// Clear new data flag
	m_new_data = false;

	// Clear working flag
	m_is_working = false;

	// Unlock a mutex
	m_mutex.unlock();

	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, PREF, "Exception from mutex has been occured: %s", obj.what());
		return;
	}
	catch (std::exception& obj)
	{
		PRINT_ERR(true, PREF, "Exception from processContours() has been occured: %s",
		          obj.what());

		// Unlock a mutex
		m_mutex.unlock();

		return;
	}
}

//-------------------------------------------------------------------------------------------------
bool NeuralNetworkThread::setNewData(const QString& fileName) noexcept
{
	// Catch a lock() and unlock() exceptions
	try {

	// Return value
	bool ret;

	// Lock a mutex
	m_mutex.lock();

	// Add new data
	if (false == m_is_working)
	{
		// Set new file name
		m_fileName = fileName;

		// Set new data flag
		m_new_data = true;

		// Set return value; new data have been accepted
		ret = true;
	}
	else
	{
		// Set return value; new data have not been accepted
		ret = false;
	}

	// Unlock a mutex
	m_mutex.unlock();

	return ret;

	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, PREF, "Exception from mutex has been occured: %s", obj.what());
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
bool NeuralNetworkThread::setNewData(const QImage& image) noexcept
{
	// Catch a lock() and unlock() exceptions
	try {

	// Return value
	bool ret;

	// Lock a mutex
	m_mutex.lock();

	// Add new data
	if (false == m_is_working)
	{
		// Set new file name
		m_image = image;

		// Set new data flag
		m_new_data = true;

		// Set return value; new data have been accepted
		ret = true;
	}
	else
	{
		// Set return value; new data have not been accepted
		ret = false;
	}

	// Unlock a mutex
	m_mutex.unlock();

	return ret;

	}
	catch (std::system_error& obj)
	{
		PRINT_ERR(true, PREF, "Exception from mutex has been occured: %s", obj.what());
		return false;
	}
}
