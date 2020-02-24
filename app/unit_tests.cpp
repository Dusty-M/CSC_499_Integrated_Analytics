/* ************************
unit_tests.cpp
CSC499 Spring 2020
Dusty Miller - V00160288
Supervisor: Dr. Sean Chester
************************ */
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include <iostream>
#include "CSVParser.hpp"

TEST_CASE("basic ingestion - sample data"){
	constexpr unsigned char DELIM {','};
	std::string filename {"census_2000_sample_places_sample_dimensions.csv"};
	std::string header {"SE_T001_001"};
	constexpr uint32_t header_row_index {1};
	constexpr uint32_t first_data_row_index {2};
	CSVParser t1 {filename, DELIM};
	SECTION("Errors opening files or finding headers throw exceptions") {

		// If file does not exist, exception should be thrown
		std::cout << "In first test" << std::endl;
		std::string bad_filename {"not_a_filename.aaa"};
		CHECK_THROWS_AS((CSVParser {bad_filename, DELIM}), std::runtime_error);

		// If header is not found in the row specified, 
		// an exception should be thrown
		std::string bad_header {"not_a_header"};
		ColumnData<uint64_t> temp_cd{};
		CHECK_THROWS_AS(t1.preprocess(temp_cd, bad_header, 1, 2), 
		std::runtime_error);
	}
}
