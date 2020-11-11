
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO:
 * test class (logic() method)
 * skip: uint64_t + exception over
 * whole: uint64_t + exception over
 * mutex + flag -> mutex + try_lock
 * base class
 * replace 152 magic number
 * xxxxxxxxxxx
 * int32_t image_size -> try {} catch
FIXME:
DANGER:
NOTE:

Sec_145::NeuralNetworkThread class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "NeuralNetwork.h"  // NeuralNetwork class (for inheritance)
#include <mutex>            // std::mutex
#include <QString>          // QString class
#include <QImage>           // QImage class
#include <system_error>     // std::system_error
#include <exception>        // std::exception

#include "Sec_145/other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
class NeuralNetworkThread : public NeuralNetwork
{
public:

	virtual ~NeuralNetworkThread()
	{
		// Delete memory for RGB data
		if (m_r != nullptr)
			delete [] m_r;
		if (m_g != nullptr)
			delete [] m_g;
		if (m_b != nullptr)
			delete [] m_b;
	}

	// Processes image
	void process() noexcept;

	// Sets new data (image)
	bool setNewData(const QString& fileName) noexcept;
	bool setNewData(const QImage& image) noexcept;

	// ***********************
	// ******* Getters *******
	// ***********************

	// Get a saved recognition label from neural network
	int32_t getRecognitionLabel() noexcept
	{
		return m_recognitionLabel;
	}

private:

	// Preface in debug message
	constexpr static const char* const PREF {"[NeuralNetworkThread]: "};

	// File name for process
	QString m_fileName;

	// Image
	QImage m_image;
	uint32_t m_image_size {0};
	uint8_t* m_r {nullptr};
	uint8_t* m_g {nullptr};
	uint8_t* m_b {nullptr};

	// Recognition label
	volatile int32_t m_recognitionLabel {-1};

	// Mutex
	std::mutex m_mutex;

	// Flag; new data have been added
	volatile bool m_new_data {false};

	// Flag; process() method is working
	volatile bool m_is_working {false};
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
