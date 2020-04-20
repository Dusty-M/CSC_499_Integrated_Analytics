#include <iostream>
#include "CSVParser.hpp"
#include "LeastSquaresFit.hpp"
#include <vector>
#include <math.h>
#include "gnuplot-iostream.h"
#include <boost/program_options.hpp>
#include <chrono>
#include "Plot.hpp"


void usage() {
    std::cout << "Example usage:\n"
        << "make_plot --filename census_2000_all_places_sample_dimensions.csv --header SE_T001_001 --header_row_index 1 --first_data_row_index 2\n" << std::endl;
}

int main(int argc, char **argv) {

    // program arg variables
    std::string     filename;
    std::string     X_header;
    std::string     Y_header;
	index_type		num_segments;
    index_type header_row_index;
    index_type  first_data_row_index;

    // ********* start of boost::program_options code ****************
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help", "Print help messages")
        ("filename", po::value<std::string>(&filename)->required(),
            "CSV filename including file extension")
        ("X_header", po::value<std::string>(&X_header)->required(),
            "unique header of column")
        ("Y_header", po::value<std::string>(&Y_header)->required(),
            "unique header of column")
        ("num_segs", po::value<index_type>(&num_segments)->required(),
            "number of segments in analysis")
        ("header_row_index", po::value<index_type>(&header_row_index)->required(),
            "index of row header is located on")
        ("first_data_row_index", po::value<index_type>(&first_data_row_index)->required(),
            "index of first row of actual data");

    po::variables_map vm;
    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if(argc == 1) {
            usage();
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("help") )
        {
            std::cout << "Progressive Analytics Application" << std::endl
                      << desc << std::endl;
            return 0;
        }

      po::notify(vm); // throws on error, so do after help in case 
                      // there are any problems 
    }
    catch(po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return 1;
    }
    // ********* end of boost::program_options code ****************

	// Create CSVParser
		constexpr unsigned char DELIM {','};
	std::shared_ptr<CSVParser> csvp;
	try {
		csvp = std::make_shared<CSVParser> (makeCSVParser(filename, DELIM));
	} catch(const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}
	// Create LeastSquaresFit object
	using vec_cols_tX = std::vector<ColumnData<float_data_type>>;
	using vec_cols_tY = std::vector<ColumnData<float_data_type>>;

	auto Xs {csvp->makeSegments<float_data_type>(X_header,
		header_row_index, first_data_row_index, num_segments)};
	auto Ys {csvp->makeSegments<float_data_type>(Y_header,
		header_row_index, first_data_row_index, num_segments)};
	auto lsf {makeLeastSquaresFit<vec_cols_tX, vec_cols_tY>(Xs, Ys)};

	// for each segment, process ColumnData into a summary
	// and combine with previous summary, then plot summary info

	auto cur_seg {lsf.getNextSegment()};
	auto summary_aggregated {create_summary(cur_seg)};

	using flt_summary_type = summary<float_data_type, float_data_type>;
	flt_summary_type s1 {
		projection {1.0, 1.0},
		{{0.5, 0.5}, {0.7, 0.7}, {0.9, 0.9}}, // representative_points
		1.0, // SS_xx
		2.0, // SS_xy
		3.0, // x_bar
		4.0, // y_bar
		1};  // count

	flt_summary_type tmp_summary =
//		create_summary(flt_summary_type{}, flt_summary_type{});
	create_summary(s1, s1);
	std::cout << "AA" << std::endl;

	tmp_summary = create_summary(tmp_summary, s1);
	std::cout << "BB" << std::endl;

	// For plotting, summary members projection and representative_pts are needed
	std::cout << tmp_summary << std::endl;
	// ********* Start of plotting code ****************************

	SummaryPlotter plotter{};
	std::string settings {"set style fill solid 0.1\n"};
	plotter.set_settings(settings);
//	plotter(tmp_summary);
	plotter(summary_aggregated);
	/*
	Gnuplot gp;

	std::vector<double> y_pts;
	std::vector<double> x_pts;
	std::vector<double> z_pts;
	for(int i=0; i<1000; i+= 25) {
		double y = (i/500.0-1) * (i/500.0-1);
		x_pts.push_back(i);
		y_pts.push_back(y);
		z_pts.push_back(-y);
	}

	//gp << "set xrange [-5:1000]\nset yrange [-5:5]\n";

	// ******** Some samples for future reference ********* 
	//gp << "plot '-' with lines, sin(x/200) with lines\n";
	//gp << "plot 'test.dat' with points\n";

//	gp << "plot '-' with points title 'point data', '-' with lines title 'line data'\n";
//	gp.send(std::make_tuple(x_pts, y_pts));

	// create string out of summary data

	std::string str = "plot ";
	str += std::to_string(tmp_summary.proj.a_proj);
	str += "+" + std::to_string(tmp_summary.proj.b_proj) + "*x ";
	str += "with lines title 'projection', '-' with points title 'point data'\n";
	std::vector<float_data_type> Xs {};
	std::vector<float_data_type> Ys {};
	for(auto pt : tmp_summary.representative_pts) {
		Xs.push_back(pt.x);
		Ys.push_back(pt.y);
	}
//	gp << "plot 2+0.01*x with lines title 'projection', '-' with points title 'point data'\n";
	gp << str;
//	gp.send(std::make_tuple(x_pts, z_pts));
	gp.send(std::make_tuple(Xs, Ys));

	std::cout << "press enter to update plot" << std::endl;
	std::cin.get();

	for(auto &val : y_pts) {
		val = val + .1;
	}

	// although an update command exists for gnuplot it seems to rely on the data being
	// stored in a file.  If the data is modified, update will scan the file and
	// update the image.  In this case since the data is being sent straight from this program,
	// replotting is the only way I can get an effective update.
//	gp << "plot '-' with points title 'point data', '-' with lines title 'line data'\n";
//	gp.send(std::make_tuple(x_pts, y_pts));
//	gp.send(std::make_tuple(x_pts, z_pts));

//	std::vector<float_data_type> X1s = {1.0, 2.0, 3.0, 4.0};
//	std::vector<float_data_type> Y1s = {1.0, 2.0, 3.0, 4.0};
//	std::vector<float_data_type> Y2s = {0.5, 1.0, 1.5, 2.0};
	std::vector<float_data_type> X1s = {0.0, 1.0, 4.0};
 	std::vector<float_data_type> Y1s = {0.0, 1.0, 2.0};
	std::vector<float_data_type> Y2s = {0.0, 0.25, 1.0};

//	gp << "set style fill pattern 5 lc 'yellow'\n";
	std::string plot_command {"plot '-' with filledcurves closed lt rgb 'red', -0.5+x with lines title 'projection'\n"};
	gp << "set style fill solid 0.1\n";
//	gp << "plot '-' with filledcurves closed lt rgb 'red', -0.5+x with lines title 'projection'\n";
	gp << plot_command;
	gp.send(std::make_tuple(X1s, Y1s, Y2s));

	*/
	std::cout << "press enter to plot from summary" << std::endl;
	std::cin.get();

	return 0;
}
