
#ifndef SEC_145_ALGORITHMS_ALGORITHMS_H
#define SEC_145_ALGORITHMS_ALGORITHMS_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Functions with algoritms
TODO: * test class (logic() method)
FIXME:
DANGER:
NOTE: * "const" in lambda function definition: "const" makes the linkage internal by default,
		 which means it is only accessible in one translation unit, thereby avoiding the problem.
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
	# Import a module
	import matplotlib.pyplot as plt

	# Read a file content
	with open("C:/Users/ekd/Documents/deep_learning/models/inspire_e.txt", 'r') as f:
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
#include <vector>   // std::vector template class

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
// Converts pixel limits from [0, 255] to [0, 1]
const auto convertPixelLimits = [](double pixel){ return pixel /= 255; };

//-------------------------------------------------------------------------------------------------
// Learning type enumeration
enum class LearningType {
	Simple,
	Neural
};

//-------------------------------------------------------------------------------------------------
// Learning a model
int32_t modelLearning(const QString& pathCopterImages,
                      const QString& resultPath,
                      const QString& resultName,
                      const LearningType type,
                      const bool scale = false,
                      const bool brightness = false,
                      const uint32_t imagesWidth = 20,
                      const uint32_t imagesHeight = 20);

//-------------------------------------------------------------------------------------------------
// Calculates a dot product
double dotProduct(const double* const d_1, const double* const d_2, const uint32_t n);
double dotProduct(const std::vector<double>& d_1, const std::vector<double>& d_2);

//-------------------------------------------------------------------------------------------------
// Calculates a correlation coefficient
double correlationCoefficient(const uint8_t* const d_1, const uint8_t* const d_2,
                              const uint32_t n);
double correlationCoefficient(const std::vector<uint8_t>& d_1, const std::vector<uint8_t>& d_2);

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_ALGORITHMS_ALGORITHMS_H
