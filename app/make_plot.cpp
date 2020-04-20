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

	SummaryPlotter plotter{};
	std::string settings {"set style fill solid 0.1\n"};
	plotter.set_settings(settings);

	plotter(summary_aggregated); // plotting first segment's summary

	std::cout << "press enter to plot summary #2" << std::endl;
	std::cin.get();

	// for each iteration, get another summary of a segment of data,
	// then aggregate it with summary_aggregate
	for(int i {1}; i < num_segments; ++i) {
		auto cur_summary {create_summary(lsf.getNextSegment())};
		summary_aggregated = create_summary(summary_aggregated, cur_summary);
		plotter(cur_summary);
		std::cout << "press enter to plot summary #" << (i + 2) << std::endl;
		std::cin.get();

	}
	return 0;
}
