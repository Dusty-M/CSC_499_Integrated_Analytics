// main.cpp
// Used to write exploratory/behavioural tests for CSVParser

#include "CSVParser.hpp"
#include <iostream>
#include <memory> // shared pointers
#include <unistd.h> // usleep
#include <boost/program_options.hpp>

int main( int argc, char **argv ) {

	// program arg variables
	std::string 	filename;
	std::string 	header;
	index_type header_row_index;
	index_type  first_data_row_index;

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

	constexpr unsigned char DELIM {','};
	std::shared_ptr<CSVParser> t1;
	try {
		 t1 = std::make_shared<CSVParser>(makeCSVParser(filename, DELIM));
	} catch(std::runtime_error &e) {
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
	return 0;
}
