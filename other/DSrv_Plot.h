
#pragma once

//-----------------------------------------------------------------------------
/*
DESCRITION: class for display a plots from get data
TODO:
FIXME:
DANGER:
NOTE: * need qcustomplot.* files availability

Sec_145::DSrv_Plot class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-----------------------------------------------------------------------------
#include <QObject>        // QObject class (for inheritance)

#include "qcustomplot.h"  // QCPGraph class
#include <QVector>        // QVector class
#include <cstdint>        // integer types
#include <Qt>             // Qt::GlobalColor
#include <vector>         // std::vector class
#include <string>         // std::string class

//-----------------------------------------------------------------------------
namespace Sec_145
{

//-----------------------------------------------------------------------------
class DSrv_Plot : public QObject
{
	Q_OBJECT

public:
	DSrv_Plot();
	~DSrv_Plot();

	// Initialization
	int32_t init(const QCustomPlot* const cus_plot,
	             const bool debug = false,
	             const std::string& name = "undefined",
	             const std::string& x_label = "x",
	             const std::string& y_label = "y",
	             const uint32_t x_plot_size = 100,
	             const uint32_t y_plot_size = 10,
	             const uint32_t num_of_plots = 1,
	             const std::vector<QCheckBox*>& cb =
	                                      std::vector<QCheckBox*>(1, nullptr),
	             const std::vector<Qt::GlobalColor>& colors =
	                                std::vector<Qt::GlobalColor>(1, Qt::blue)
	             );

	// Add data to plot
	void addData(const uint32_t index, const double data);

private:
	// Struct for each plot
	struct Plot {
		// Plot
		QCPGraph* plot;
		// t for plot
		QVector<double> gr_t;
		// plot(t)
		QVector<double> gr;
		// Index for plot(t)
		uint32_t gr_i;
		// Graph color
		Qt::GlobalColor gr_color;
		// Check box
		QCheckBox* cb;
	};

	// Vector with plot structs
	std::vector<struct Plot> m_plots;

	// Custom plot from widget class
	QCustomPlot* m_cus_plot;

	// Number of plots
	uint32_t m_num_of_plots;

	// Name (label) of plots
	const char* m_name;

	// Enable/disable a debug output via printDebug.cpp/.h
	bool m_debug;
};

//-----------------------------------------------------------------------------
} // namespace Sec_145
