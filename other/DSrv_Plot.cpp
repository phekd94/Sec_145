
#include "dsrv_plot.h"

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-----------------------------------------------------------------------------
#define PREF  "[DSrv_Plot]: "

//-----------------------------------------------------------------------------
using namespace Sec_145;

//-----------------------------------------------------------------------------
DSrv_Plot::DSrv_Plot() : m_cus_plot(nullptr)
{
	PRINT_DBG(m_debug, PREF, "");
}

//-----------------------------------------------------------------------------
DSrv_Plot::~DSrv_Plot()
{
	PRINT_DBG(m_debug, PREF, "");
}

//-----------------------------------------------------------------------------
int32_t DSrv_Plot::init(const QCustomPlot* const cus_plot,
                        const bool debug,
                        const std::string& name,
                        const std::string& x_label,
                        const std::string& y_label,
                        const uint32_t x_plot_size,
                        const uint32_t y_plot_size,
                        const uint32_t num_of_plots,
                        const std::vector<QCheckBox*>& cb,
                        const std::vector<Qt::GlobalColor>& colors)
{
	// Check the incoming parameters
	if (cb.size() != num_of_plots || colors.size() != num_of_plots) {
		PRINT_ERR(true, PREF,
		 "Size of colors or check box containers is not equal num_of_motors");
		return -1;
	}

	// Check the incoming parameter and set a custom plot member
	if (nullptr == cus_plot) {
		PRINT_ERR(true, PREF, "nullptr == cus_plot");
		return -1;
	} else {
		m_cus_plot = const_cast<QCustomPlot*>(cus_plot);
	}

	// Set a debug member
	m_debug = debug;

	// Set a name member
	if (name.empty() == true) {
		m_name = "undefined";
	} else {
		static std::string name_stat = name;
		m_name = name_stat.c_str();
	}

	// Set labels for custom plot
	m_cus_plot->xAxis->setLabel(x_label.c_str());
	m_cus_plot->yAxis->setLabel(y_label.c_str());

	// Set ranges for custom plot
	m_cus_plot->xAxis->setRange(0, x_plot_size);
	m_cus_plot->yAxis->setRange(0, y_plot_size);

	// Set a number of plots member
	m_num_of_plots = num_of_plots;

	// Set a number of plots for vector with plots
	m_plots.resize(num_of_plots);

	//
	for (uint32_t i = 0; i < num_of_plots; ++i) {
		//
		m_plots[i].cb = cb[i];
		m_plots[i].gr_color = colors[i];
		// connect()

		//
		m_plots[i].gr = QVector<double>(0);
		m_plots[i].gr_t = QVector<double>(0);
		m_plots[i].gr_i = 0;

		//
		if ((m_plots[i].plot = m_cus_plot->addGraph()) == nullptr) {
			PRINT_ERR(true, PREF, "[%s]: addGraph(), i = %lu",
			          name.c_str(),
			          static_cast<unsigned long>(i));
			m_cus_plot->clearGraphs();
			return -1;
		} else {
			m_plots[i].plot->setData(m_plots[i].gr_t, m_plots[i].gr, true);
		}
	}

	// replot

	return 0;
}
