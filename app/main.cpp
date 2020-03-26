// main.cpp
// Used to write exploratory/behavioural tests for CSVParser

#include "CSVParser.hpp"
#include "LeastSquaresFit.hpp"
#include <iostream>
#include <memory> // shared pointers
#include <unistd.h> // usleep
#include <boost/program_options.hpp>
#include <chrono>

void usage() {
	std::cout << "Example usage:\n"
		<< "CSVParser_tester --filename census_2000_all_places_sample_dimensions.csv --header SE_T001_001 --header_row_index 1 --first_data_row_index 2\n" << std::endl;
}

int main( int argc, char **argv ) {
	// program arg variables
	std::string 	filename;
	std::string 	header;
	index_type header_row_index;
	index_type  first_data_row_index;

	// ********* start of boost::program_options code ****************
	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
		("help", "Print help messages")
		("filename", po::value<std::string>(&filename)->required(), 
			"CSV filename including file extension")
		("header", po::value<std::string>(&header)->required(), 
			"unique header of column")
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

	constexpr unsigned char DELIM {','};
	std::shared_ptr<CSVParser> t1;
	try {
		 t1 = std::make_shared<CSVParser>(makeCSVParser(filename, DELIM));
	} catch(const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}
	ColumnData<int_data_type> results;

	// Set up values to perform a one-hit analysis
	{
		index_type num_segments {1};
		std::vector<ColumnData<int_data_type>> result_cds;
		try{
			result_cds = t1->makeSegments<int_data_type>(
				header, header_row_index, first_data_row_index, num_segments);
		}catch (std::runtime_error &e) {
			std::cout << e.what() << std::endl;
			return 3;
		}
		t1->runAnalysisSegment(result_cds.at(0));

		std::cout << "result of one-hit analysis: " << result_cds.at(0).data_summary_actual << '\n' << std::endl;
	}

	// Set up values to perform a progressive analysis
	// Note: This only creates ColumnData segments, it
	// does not perform any calculations
	{
		index_type num_segments {5};
		std::vector<ColumnData<int_data_type>> result_cds;
		try{
			result_cds = t1->makeSegments<int_data_type>(
				header, header_row_index, first_data_row_index, num_segments);
		}catch (std::runtime_error &e) {
			std::cout << e.what() << std::endl;
			return 3;
		}

		std::cout << "******************************************" << std::endl;
		std::cout << "printing contents of progressive analysis:" << std::endl;
		std::cout << "******************************************" << std::endl;

		int_data_type sum{};
		for(auto &cur_res : result_cds) {
			t1->runAnalysisSegment(cur_res);
			std::cout << cur_res << '\n' << std::endl;
			sum += cur_res.data_summary_actual;
		}
		std::cout << "Final sum: " << sum << std::endl;
	}


	// Performing a progressive analysis using LeastSquaresFit class
	std::string X_header {"SE_T003_001"};
	std::string Y_header {"SE_T013_001"};
	

	auto Xs = t1->makeSegments<float_data_type>(X_header, header_row_index, first_data_row_index, 5);
	auto Ys = t1->makeSegments<float_data_type>(Y_header, header_row_index, first_data_row_index, 5);
	std::cout << "Xs size: " << Xs.size() << std::endl;
	std::cout << "Ys size: " << Ys.size() << std::endl;
	std::cout << "X_bar: " << calcAvg(Xs) << std::endl;
	std::cout << "Y_bar: " << calcAvg(Ys) << std::endl;
	std::cout << "\n" << std::endl;

	using vec_cols_float = std::vector<ColumnData<float_data_type>>;
	LeastSquaresFit<vec_cols_float, vec_cols_float> lsf = 
		makeLeastSquaresFit<vec_cols_float, vec_cols_float>(Xs, Ys);

	// Display results/projections after each segment is processed
	std::cout << lsf << "\n\n" << std::endl;
	constexpr int_data_type usecs {2000000}; // microseconds
	std::cout << "Displaying projections during progressive analysis:" << std::endl; 
	while(lsf.calcNextProjection()) {
		auto start = std::chrono::system_clock::now();
		std::cout << '\r' << "a: " << lsf.getProja() << " b: " << lsf.getProjb();
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

		std::cout << " elapsed: " << elapsed.count() << " ns" << std::flush;
		usleep(usecs);
	}
	std::cout << "\nProgram closing" << std::endl;
	return 0;
}
