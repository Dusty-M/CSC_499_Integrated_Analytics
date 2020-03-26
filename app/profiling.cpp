// profiling.cpp
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
		<< "profiling --filename census_2000_all_places_sample_dimensions.csv --X_header SE_T003_001 --Y_header SE_T013_001 --num_segs 5 --header_row_index 1 --first_data_row_index 2\n" << std::endl;
}

int main( int argc, char **argv ) {
	// program arg variables
	std::string 	filename;
	std::string 	X_header;
	std::string 	Y_header;
	index_type 		num_segs;
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
		("num_segs", po::value<index_type>(&num_segs)->required(), 
			"number of segments")
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

	// Create CSVParser object
	constexpr unsigned char DELIM {','};
	std::shared_ptr<CSVParser> t1;
	try {
		 t1 = std::make_shared<CSVParser>(makeCSVParser(filename, DELIM));
	} catch(const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}

	// call profiler function
	runProfile<float_data_type, float_data_type>(
		num_segs, *t1, X_header, Y_header, header_row_index, first_data_row_index);

	return 0;
}
