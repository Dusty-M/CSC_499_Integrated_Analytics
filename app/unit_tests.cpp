/* ************************
unit_tests.cpp
CSC499 Spring 2020
Dusty Miller - V00160288
Supervisor: Dr. Sean Chester
************************ */
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <cstdlib>
#include <string>
#include <iostream>
#include "CSVParser.hpp"
#include "LeastSquaresFit.hpp"


TEST_CASE("Exception tests"){
constexpr unsigned char DELIM {','};
std::string filename1 {"census_2000_sample_places_sample_dimensions.csv"};
std::string header {"SE_T001_001"};
constexpr index_type header_row_index {1};
constexpr index_type first_data_row_index {2};
CSVParser t1 = makeCSVParser (filename1, DELIM);

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
	constexpr unsigned char DELIM {','};
	std::string filename1 {"census_2000_sample_places_sample_dimensions.csv"};
	std::string header {"SE_T001_001"};
	constexpr index_type header_row_index {1};
	constexpr index_type first_data_row_index {2};
	CSVParser t1 = makeCSVParser (filename1, DELIM);

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
	SECTION("Testing CSVParser::preprocess function") {
		// Create 5-segment set of ColumnData structs
		index_type num_segments {5};
		
		std::vector<ColumnData<int_data_type>> cd_constructed_vec = 
			t1.makeSegments<int_data_type>(
			header,
			header_row_index,
			first_data_row_index,
			num_segments);

		// expected values, used to confirm results
		int_data_type data_at_0 {2987};
		int_data_type data_at_86 {990};
		int_data_type data_at_87 {470};
		int_data_type data_at_173 {3173};
		int_data_type data_at_174 {3162};
		int_data_type data_at_260 {16543};
		int_data_type data_at_261 {310};
		int_data_type data_at_347 {462};
		int_data_type data_at_348 {464};
		int_data_type data_at_435 {19940};

		CHECK(cd_constructed_vec.at(0).data_raw.at(0) == data_at_0);
		CHECK(cd_constructed_vec.at(0).data_raw.at(86) == data_at_86);
		CHECK(cd_constructed_vec.at(1).data_raw.at(0) == data_at_87);
		CHECK(cd_constructed_vec.at(1).data_raw.at(86) == data_at_173);
		CHECK(cd_constructed_vec.at(2).data_raw.at(0) == data_at_174);
		CHECK(cd_constructed_vec.at(2).data_raw.at(86) == data_at_260);
		CHECK(cd_constructed_vec.at(3).data_raw.at(0) == data_at_261);
		CHECK(cd_constructed_vec.at(3).data_raw.at(86) == data_at_347);
		CHECK(cd_constructed_vec.at(4).data_raw.at(0) == data_at_348);
		CHECK(cd_constructed_vec.at(4).data_raw.at(87) == data_at_435);
	}
}
TEST_CASE("LeastSquaresFit class test") {
	constexpr unsigned char DELIM {','};
	std::string filename1 {"census_2000_all_places_sample_dimensions.csv"};
	std::string X_header {"SE_T003_001"};
	std::string Y_header {"SE_T013_001"};
	constexpr index_type header_row_index {1};
	constexpr index_type first_data_row_index {2};
	index_type num_segments {5};
	CSVParser t1 = makeCSVParser (filename1, DELIM);

	SECTION("Testing makeSegments ColumnData Factory Function") {
		// create segments for X and Y, then create LeastSquaresFit object
		auto X {t1.makeSegments<float_data_type>(
			X_header, header_row_index, first_data_row_index, num_segments)};
		auto Y {t1.makeSegments<float_data_type>(
			Y_header, header_row_index, first_data_row_index, num_segments)};
		auto lsf = makeLeastSquaresFit(X, Y);

		float_data_type epsilon {.01};
		float_data_type a_lower_bound {37.0};
		float_data_type a_upper_bound {38.23};
		index_type num_progressions_expected {5};
		index_type num_progressions_actual {0};
		while(lsf.calcNextProjection()) {
			auto cur_a = lsf.getProja();
			auto cur_b = lsf.getProjb();

			CHECK(cur_a < a_upper_bound);
			CHECK(cur_a > a_lower_bound);
			CHECK(std::abs(cur_b) < epsilon);
			++num_progressions_actual;
		}
		CHECK(num_progressions_actual == num_progressions_expected);
	}
}
