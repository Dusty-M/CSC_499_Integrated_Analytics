// main.cpp
// Used to write exploratory/behavioural tests for CSVParser

#include "CSVParser.hpp"
#include <iostream>

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
	uint64_t header_index {std::stoull(argv[3])};
	uint64_t  first_data_row_index {std::stoull(argv[4])};
	constexpr unsigned char DELIM {','};

	CSVParser t1{ filename, DELIM};
	ColumnData<uint64_t> results;
	try {
		results = t1.getData<uint64_t> (header, header_index, first_data_row_index);

	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}

	// demonstrating function of << operator on ColumnData
	std::cout << "sum of " << results << std::endl;
	
	// Demonstrating == operator on ColumnData
	constexpr uint64_t CORRECT_SUM {3199912};
	ColumnData<uint64_t> t1_cd;
	t1_cd.header = header;
	t1_cd.data_actual = CORRECT_SUM;

	std::cout << "Test 1: " ;
	if(t1_cd == results) {
		std::cout << "PASSED" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
	}

	// TODO: put call to getDataSegment() in try/catch block
	// test getDataSegment()
	ColumnData<uint64_t> cd_s0;
	cd_s0.header = header;
	cd_s0.num_segments = 5;
	cd_s0.cur_segment = 0;
	ColumnData<uint64_t> cd_s1 {cd_s0};
	cd_s1.cur_segment = 1;
	ColumnData<uint64_t> cd_s2 {cd_s0};
	cd_s2.cur_segment = 2;
	ColumnData<uint64_t> cd_s3 {cd_s0};
	cd_s3.cur_segment = 3;
	ColumnData<uint64_t> cd_s4 {cd_s0};
	cd_s4.cur_segment = 4;

	t1.getDataSegment(header, header_index, first_data_row_index, cd_s0, 5, 0);
	t1.getDataSegment(header, header_index, first_data_row_index, cd_s1, 5, 1);
	t1.getDataSegment(header, header_index, first_data_row_index, cd_s2, 5, 2);
	t1.getDataSegment(header, header_index, first_data_row_index, cd_s3, 5, 3);
	t1.getDataSegment(header, header_index, first_data_row_index, cd_s4, 5, 4);

	std::cout << "cd_s0: " << cd_s0 << std::endl;
	std::cout << "cd_s1: " << cd_s1 << std::endl;
	std::cout << "cd_s2: " << cd_s2 << std::endl;
	std::cout << "cd_s3: " << cd_s3 << std::endl;
	std::cout << "cd_s4: " << cd_s4 << std::endl;

	auto sum = 
		cd_s0.data_actual +
		cd_s1.data_actual +
		cd_s2.data_actual +
		cd_s3.data_actual +
		cd_s4.data_actual;

		std::cout << "final result: " << sum << std::endl;
	return 0;
}
