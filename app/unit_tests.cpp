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

constexpr unsigned char DELIM {','};
std::string filename1 {"census_2000_sample_places_sample_dimensions.csv"};
std::string header {"SE_T001_001"};
constexpr index_type header_row_index {1};
constexpr index_type first_data_row_index {2};
CSVParser t1 = makeCSVParser (filename1, DELIM);

TEST_CASE("Exception tests"){
	SECTION("Errors opening files or finding headers throw exceptions") {
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
	SECTION("Ensure Files open properly and existing headers are found") {
		index_type num_segments {5};
		CHECK_NOTHROW(makeCSVParser (filename1, DELIM));
		CSVParser csvp = makeCSVParser (filename1, DELIM);
		CHECK_NOTHROW(csvp.makeSegments<int_data_type> (
			header, header_row_index, first_data_row_index, num_segments));
	}
}

TEST_CASE("ColumnData content tests") {
	std::string header {"SE_T001_001"};
	constexpr index_type header_row_index {1};
	constexpr index_type first_data_row_index {2};

	SECTION("Testing makeSegments ColumnData Factory Function") {
		// create some variables used to create a target ColumnData.
		index_type 
			header_col_index {16},
			num_rows {436},
			num_segments {1},
			cur_segment {0};
		float_data_type
			data_summary_actual {0},
			data_summary_projected {0};
		std::vector<float_data_type> data_raw {};
		std::vector<index_type> segment_indices {2};

		// Check construction of a single segment
		ColumnData<float_data_type> cd_target {
				header,
				first_data_row_index,
				header_row_index,
				header_col_index,
				num_rows,
				data_summary_actual,
				data_summary_projected,
				data_raw,
				num_segments,
				cur_segment,
				segment_indices};
		std::vector<ColumnData<float_data_type>> cd_constructed_vec = 
			t1.makeSegments<float_data_type>(
			header,
			header_row_index,
			first_data_row_index,
			num_segments);

			CHECK(cd_target == cd_constructed_vec.at(0));

		// Check construction of a multi-segment call
		num_segments = 5;
		segment_indices = std::vector<index_type> {2, 89, 176, 263, 350};

		// modify cd_target to reflect the expected contents
		// of the new cd_constructed
		cd_target.num_segments = num_segments;
		cd_target.segment_indices = segment_indices;

		// Create 5-segment set of ColumnData structs
		cd_constructed_vec = 
			t1.makeSegments<float_data_type>(
			header,
			header_row_index,
			first_data_row_index,
			num_segments);
		
		// Confirm contents of each of the 5 segments
		for(index_type i {0}; i < num_segments; ++i) {
			cd_target.cur_segment = i;
			CHECK(cd_target == cd_constructed_vec.at(i));
		}
	}
}
