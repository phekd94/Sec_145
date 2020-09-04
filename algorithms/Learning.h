
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO: * test class (logic() method)
FIXME: * batch_gradientDescent(), batch_gradientDescent_conv():
		 l_2_delta[batch](0, i) /= (batch_size * num_labels); // num_labels ???
	   * batch_gradientDescent_conv(): w_0_1 (+ or -)= alpha * ...
DANGER:
NOTE: * Access only across '*' or '[]' to data received from Eigen::MatrixXd::data() method
	  * Python code for print a histogram:
"
	# Import modules
	import matplotlib.pyplot as plt
	import seaborn as sns

	# Read a file content
	with open("C:/Users/ekd/Desktop/p_1.txt", 'r') as f:
		content_1 = f.readlines()
	content_1 = [int(x.strip()) for x in content_1]

	# Read a file content
	with open("C:/Users/ekd/Desktop/p_6.txt", 'r') as f:
		content_2 = f.readlines()
	content_2 = [int(x.strip()) for x in content_2]

	# Median
	print(median(content))

	# One histogram
	ax = sns.distplot(content_1, bins=20) #, rug=False, hist=False)
	# Two histograms
	#plt.hist([content_1, content_2], color=['r','y'], histtype = 'bar') #alpha=0.5)
	# Percents
	ax = sns.distplot(content, kde=False)
	ax.yaxis.set_major_formatter(PercentFormatter(xmax=len(content)))

	# Show histogram
	plt.show()
"

	  * Python code for print a plot:
"
	import matplotlib.pyplot as plt

	with open(PATH_TO_FILE_WITH_TEST, 'r') as f:
		content = f.readlines()
	content = [float(x.strip()) for x in content]

	with open(PATH_TO_FILE_WITH_TRAIN, 'r') as f:
		content_2 = f.readlines()
	content_2 = [float(x.strip()) for x in content_2]

	plt.plot(content, label='test')
	plt.plot(content_2, label='train')
	plt.legend()
	plt.title('Success')
	plt.xlabel('Iteration')
	plt.ylabel('Probability of success')
	plt.grid(True)

	plt.show()
"

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
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
// Learning type enumeration
enum class LearningType {
	Simple [[deprecated("This learning method gives inaccurate results")]],
	Neural_1_layer [[deprecated("This learning method gives inaccurate results")]],
	Neural_2_layer,
};

//-------------------------------------------------------------------------------------------------
// Get recognition label from neural network
int32_t getRecognitionLabel(const QImage& image,
                            const Eigen::MatrixXd& w_0_1, const Eigen::MatrixXd& w_1_2,
                            Eigen::MatrixXd& l_0, Eigen::MatrixXd& l_1, Eigen::MatrixXd& l_2,
                            const uint32_t imagesWidth, const uint32_t imagesHeight);

//-------------------------------------------------------------------------------------------------
// Learning a model
int32_t modelLearning(const QString& pathToImages,
                      const QString& pathToImagesTest,
                      const QString& resultPath,
                      const QString& resultName,
                      const LearningType type,
                      const uint32_t numIterations,
                      const uint32_t numCopters,
                      const uint32_t imagesWidth,
                      const uint32_t imagesHeight,
                      const bool test = false);

//-------------------------------------------------------------------------------------------------
// Load neural network for copters (version 1)
int32_t loadModel_copters_v1(const QString& pathToModel);

int32_t getRecognitionLayer_copters_v1();

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
