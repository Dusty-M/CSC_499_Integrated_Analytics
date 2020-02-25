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
	uint32_t header_row_index;
	uint32_t  first_data_row_index;

	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
		("help", "Print help messages")
		("filename", po::value<std::string>(&filename)->required(), 
			"CSV filename including file extension")
		("header", po::value<std::string>(&header)->required(), 
			"unique header of column")
		("header_row_index", po::value<uint32_t>(&header_row_index)->required(), 
			"index of row header is located on")
		("first_data_row_index", po::value<uint32_t>(&first_data_row_index)->required(), 
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


/*****************************************
	if(argc < 5) {
		usage();
		return 1;
	}

	// store arguments in readable vars
	std::string 	filename {argv[1]};
	std::string 	header	 {argv[2]};
	uint64_t header_row_index {std::stoull(argv[3])};
	uint64_t  first_data_row_index {std::stoull(argv[4])};
*****************************************/

	constexpr unsigned char DELIM {','};
	std::shared_ptr<CSVParser> t1;
	try {
		 t1 = std::make_shared<CSVParser>( filename, DELIM);
	} catch(std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}
	ColumnData<uint64_t> results;

	// Set up values to perform a one-hit analysis
	{
		uint32_t num_segments {1};
		std::vector<ColumnData<uint64_t>> result_cds;
		try{
			result_cds = t1->makeSegments<uint64_t>(
				header, header_row_index, first_data_row_index, num_segments);
		}catch (std::runtime_error &e) {
			std::cout << e.what() << std::endl;
			return 3;
		}
		t1->getDataSegment(result_cds.at(0));

		std::cout << "result of one-hit analysis: " << result_cds.at(0).data_summary_actual << '\n' << std::endl;
	}

	// Set up values to perform a progressive analysis
	{
		uint32_t num_segments {5};
		std::vector<ColumnData<uint64_t>> result_cds;
		try{
			result_cds = t1->makeSegments<uint64_t>(
				header, header_row_index, first_data_row_index, num_segments);
		}catch (std::runtime_error &e) {
			std::cout << e.what() << std::endl;
			return 3;
		}

		std::cout << "******************************************" << std::endl;
		std::cout << "printing contents of progressive analysis:" << std::endl;
		std::cout << "******************************************" << std::endl;

		uint64_t sum{};
		for(auto &cur_res : result_cds) {
			t1->getDataSegment(cur_res);
			std::cout << cur_res << '\n' << std::endl;
			sum += cur_res.data_summary_actual;
		}
		std::cout << "Final sum: " << sum << std::endl;
	}
	return 0;
}
