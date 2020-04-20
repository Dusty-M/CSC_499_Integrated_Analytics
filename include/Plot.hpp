// Plot.hpp
// Definition of API for gnuplot-iostream
// Used to set up gnuplot-iostream objects and print summaries
// produced by LeastSquaresFit objects

#ifndef PLOT_HPP
#define PLOT_HPP

#include "gnuplot-iostream.h"
#include "LeastSquaresFit.hpp"
#include <vector>
#include <string>
#include <tuple>
#include <iostream>

class SummaryPlotter {
public:
	SummaryPlotter();
	void set_settings(std::string &settings_string);
	void send_settings();
	void operator()(summary<float_data_type, float_data_type> plot_data);
private:
	std::string _settings;
	Gnuplot _gp;
};

#endif
