
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions for learning
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE: * Python code for print a histogram:
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
	# Import a module
	import matplotlib.pyplot as plt

	# Read a file content
	with open("C:/Users/ekd/Documents/deep_learning/models/neural/inspire_e.txt", 'r') as f:
		content = f.readlines()
	content = [float(x.strip()) for x in content]

	# Assign data to plot
	plt.plot(content)

	# Show a plot
	plt.show()
"

+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      YES      |     YES    |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <cstdint>  // integer types
#include <QString>  // QString class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Learning type enumeration
enum class LearningType {
	Simple [[deprecated("This learning method gives inaccurate results")]],
	Neural_1_layer,
	Neural_2_layer,
};

//-------------------------------------------------------------------------------------------------
// Learning a model
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPath,
                      const QString& resultName,
                      const LearningType type,
                      const bool scale = false,
                      const uint32_t numCycle = 1,
                      const uint32_t numCopters = 1,
                      const bool brightness = false,
                      const uint32_t imagesWidth = 20,
                      const uint32_t imagesHeight = 20);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
