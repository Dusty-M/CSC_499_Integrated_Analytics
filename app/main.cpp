// main.cpp
// Used to write exploratory/behavioural tests for CSVParser

#include "CSVParser.hpp"
#include <iostream>
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

	CSVParser t1{ filename, DELIM};
	ColumnData<uint64_t> results;
	try {
		t1.preprocess(results, header, 1, 2);
		t1.getData<uint64_t> (results, header, header_row_index, first_data_row_index);
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

	// test getDataSegment()
	ColumnData<uint64_t> cd_s0;
	cd_s0.num_segments = 5;
	cd_s0.cur_segment = 0;
	t1.preprocess(cd_s0, header, header_row_index, first_data_row_index);

	ColumnData<uint64_t> cd_s1;
	cd_s1.num_segments = 5;
	cd_s1.cur_segment = 1;
	t1.preprocess(cd_s1, header, header_row_index, first_data_row_index);

	ColumnData<uint64_t> cd_s2;
	cd_s2.num_segments = 5;
	cd_s2.cur_segment = 2;
	t1.preprocess(cd_s2, header, header_row_index, first_data_row_index);

	ColumnData<uint64_t> cd_s3;
	cd_s3.num_segments = 5;
	cd_s3.cur_segment = 3;
	t1.preprocess(cd_s3, header, header_row_index, first_data_row_index);

	ColumnData<uint64_t> cd_s4;
	cd_s4.num_segments = 5;
	cd_s4.cur_segment = 4;
	t1.preprocess(cd_s4, header, header_row_index, first_data_row_index);

	std::cout << "progressive display:\n" << std::endl;
	constexpr int usecs {2000000}; // microseconds
	std::string proj_result;
	uint64_t sum {0};
	uint32_t total_segs {5};
	uint32_t current_seg {1};

	t1.getDataSegment(header, header_row_index, first_data_row_index, cd_s0, 5, 0);
	auto fraction = (float)(cd_s0.data_actual * total_segs) / current_seg;

	std::cout << "s0 data: " << cd_s0.data_actual << " fraction: " << fraction << std::endl;
	sum += cd_s0.data_actual;
	proj_result = std::to_string((sum * total_segs) / current_seg++);
	proj_result.insert(0, 15 - proj_result.length(), ' ');
	std::cout << '\r' << proj_result << std::flush;
	usleep(usecs); // sleep for 1 sec

	t1.getDataSegment(header, header_row_index, first_data_row_index, cd_s1, 5, 1);
	sum += cd_s1.data_actual;
	proj_result = std::to_string(sum * total_segs / current_seg++);
	proj_result.insert(0, 15 - proj_result.length(), ' ');
	std::cout << '\r' << proj_result << std::flush;
	usleep(usecs); // sleep for 1 sec

	t1.getDataSegment(header, header_row_index, first_data_row_index, cd_s2, 5, 2);
	sum += cd_s2.data_actual;
	proj_result = std::to_string(sum * total_segs / current_seg++);
	proj_result.insert(0, 15 - proj_result.length(), ' ');
	std::cout << '\r' << proj_result << std::flush;
	usleep(usecs); // sleep for 1 sec

	t1.getDataSegment(header, header_row_index, first_data_row_index, cd_s3, 5, 3);
	sum += cd_s3.data_actual;
	proj_result = std::to_string(sum * total_segs / current_seg++);
	proj_result.insert(0, 15 - proj_result.length(), ' ');
	std::cout << '\r' << proj_result << std::flush;
	usleep(usecs); // sleep for 1 sec

	t1.getDataSegment(header, header_row_index, first_data_row_index, cd_s4, 5, 4);
	sum += cd_s4.data_actual;
	proj_result = std::to_string(sum * total_segs / current_seg);
	proj_result.insert(0, 15 - proj_result.length(), ' ');
	std::cout << '\r' << proj_result << std::flush;
	usleep(usecs); // sleep for 1 sec

/*
	std::cout << "cd_s0: " << cd_s0 << std::endl;
	std::cout << "\ncd_s1: " << cd_s1 << std::endl;
	std::cout << "\ncd_s2: " << cd_s2 << std::endl;
	std::cout << "\ncd_s3: " << cd_s3 << std::endl;
	std::cout << "\ncd_s4: " << cd_s4 << std::endl;
	std::cout << std::endl;
	auto sum = 
		cd_s0.data_actual +
		cd_s1.data_actual +
		cd_s2.data_actual +
		cd_s3.data_actual +
		cd_s4.data_actual;

*/
		std::cout << "\nfinal result: " << sum << " cur_seg: " << current_seg << std::endl;
	return 0;
}
