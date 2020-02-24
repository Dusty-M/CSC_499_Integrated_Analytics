// main.cpp
// Used to write exploratory/behavioural tests for CSVParser

#include "CSVParser.hpp"
#include <iostream>
#include <memory> // shared pointers
#include <unistd.h> // usleep
void usage() {
	std::cout << "****************************************************" << std::endl;
	std::cout << "Error: too few args.  Proper usage:\n"
		<< "./test_prog <CSV filename> "
		<< "<header name> <index of header> <index of first data>\n"
		<< "example: test_prog some_data.csv SE_T001_001 1 2\n";
	std::cout << "****************************************************" << std::endl;
}

int main( int argc, char **argv ) {
	if(argc < 5) {
		usage();
		return 1;
	}

	// store arguments in readable vars
	std::string 	filename {argv[1]};
	std::string 	header	 {argv[2]};
	uint64_t header_row_index {std::stoull(argv[3])};
	uint64_t  first_data_row_index {std::stoull(argv[4])};
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

		std::cout << "result of one-hit analysis: " << result_cds.at(0).data_actual << '\n' << std::endl;
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
		for(auto cur_res : result_cds) {
			t1->getDataSegment(cur_res);
			std::cout << cur_res << '\n' << std::endl;
			sum += cur_res.data_actual;
		}
		std::cout << "Final sum: " << sum << std::endl;
	}
	return 0;
}
