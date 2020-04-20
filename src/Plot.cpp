// Plot.cpp

#include "Plot.hpp"

SummaryPlotter::SummaryPlotter() : _gp()  {}

void SummaryPlotter::set_settings(std::string &settings_string) {
	_settings = settings_string;
}

void SummaryPlotter::send_settings() {
	_gp << _settings;
}

void SummaryPlotter::operator()(summary<float_data_type, float_data_type> plot_data) {

	// ****create plot string****

	//TODO: Confidence interval (shaded region)

	// projection
	std::string plot_string = "plot ";
    plot_string += std::to_string(plot_data.proj.a_proj); // y-intercept
    plot_string += "+" + std::to_string(plot_data.proj.b_proj) + "*x "; // + (slope * x)
    plot_string += "with lines title 'projection', '-' with points title 'point data'\n";

	// representative points
    std::vector<float_data_type> Xs {};
    std::vector<float_data_type> Ys {};
    for(auto pt : plot_data.representative_pts) {
        Xs.push_back(pt.x);
        Ys.push_back(pt.y);
    }

    _gp << plot_string;
    _gp.send(std::make_tuple(Xs, Ys));
}
