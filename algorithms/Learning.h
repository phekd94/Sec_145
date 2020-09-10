
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO:
 * test class (logic() method)
 * Eigen exceprions
 * std::chrono instead QDateTime
FIXME:
DANGER:
NOTE: * Access only across '*' or '[]' to data received from Eigen::MatrixXd::data() method

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>      // integer types
#include <QString>      // QString class
#include <QImage>       // QImage class
#include "Eigen/Dense"  // Eigen::MatrixXd class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Get a recognition label from neural network
int32_t getRecognitionLabel(const QImage& image);
int32_t getRecognitionLabel(const QString& fileName);

//-------------------------------------------------------------------------------------------------
// Load a neural network
int32_t loadModel(const QString& pathToModel);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
