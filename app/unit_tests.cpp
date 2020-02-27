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
	constexpr index_type header_row_index {1};
	constexpr index_type first_data_row_index {2};
	CSVParser t1 = makeCSVParser (filename, DELIM);
	SECTION("Errors opening files or finding headers throw exceptions") {
		CHECK(true);
		// If file does not exist, exception should be thrown
		std::string bad_filename {"not_a_filename.aaa"};
		CHECK_THROWS_AS(makeCSVParser (bad_filename, DELIM), std::runtime_error);

		// If header is not found in the row specified, 
		// an exception should be thrown
		std::string bad_header {"not_a_header"};
		index_type num_segments {5};
		CHECK_THROWS_AS(t1.makeSegments<int_data_type>(
			bad_header, header_row_index, first_data_row_index, num_segments),
			std::runtime_error);
	}
}
