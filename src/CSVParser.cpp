// CSVParser.cpp

#include "CSVParser.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sstream>

template struct ColumnData<int_data_type>;

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd) {
	os << "header: " << cd.header << 
		"\nfirst_data_row_index: " << cd.first_data_row_index <<
		"\nheader_row_index: " << cd.header_row_index <<
		"\nheader_col_index: " << cd.header_col_index <<
		"\nnum_rows: " << cd.num_rows <<
		"\ndata_summary_actual: " << cd.data_summary_actual <<
		"\ndata_summary_projected: " << cd.data_summary_projected <<
		"\nnum_segments: " << cd.num_segments <<
		"\ncur_segment: " << cd.cur_segment <<
		"\nsegment_indices: [ ";
	for(auto index : cd.segment_indices) {
		std::cout << index << " ";
	}
	std::cout << "]" << std::flush;
	return os;
}
template std::ostream &operator<<(std::ostream &os, const ColumnData<int_data_type> &cd);
template std::ostream &operator<<(std::ostream &os, const ColumnData<float_data_type> &cd);

template <typename T>
bool operator==(const ColumnData<T> &cd1, const ColumnData<T> &cd2){
	return(	cd1.header == cd2.header && 
			cd1.first_data_row_index == cd2.first_data_row_index &&
			cd1.header_row_index == cd2.header_row_index &&
			cd1.header_col_index == cd2.header_col_index &&
			cd1.num_rows == cd2.num_rows &&
			cd1.data_summary_actual == cd2.data_summary_actual &&
			cd1.data_summary_projected == cd2.data_summary_projected &&
			cd1.num_segments == cd2.num_segments &&
			cd1.cur_segment == cd2.cur_segment &&
			cd1.segment_indices == cd2.segment_indices);
}
template bool operator==(
	const ColumnData<int_data_type> &cd1, 
	const ColumnData<int_data_type> &cd2);
template bool operator==(
	const ColumnData<float_data_type> &cd1, 
	const ColumnData<float_data_type> &cd2);

template <typename T>
float_data_type calcAvg(std::vector<ColumnData<T>> &cd_vec) {
	float_data_type avg {0};
	int_data_type n {0};
	for(auto const &cd : cd_vec) {
		for(auto const &val : cd.data_raw) {
			avg+= val;
		}
		n += cd.data_raw.size();
	}
	avg /= n;
	return avg;
}
template float_data_type calcAvg(std::vector<ColumnData<int_data_type>> &cd);
template float_data_type calcAvg(std::vector<ColumnData<float_data_type>> &cd);


CSVParser makeCSVParser(const std::string &filename, const char delim) {
	return readData(CSVParser{filename, delim});
}

CSVParser::CSVParser(const std::string& filename, const char delim) : 
	_delim {delim}, _filename {filename} {}

CSVParser readData(CSVParser csvp) {
	std::ifstream file {csvp._filename};
	if(file.fail()) {
		throw std::runtime_error("Filename does not exist");
	}
	std::string raw_line{};
	std::vector<std::string> parsed_row{};
	std::vector<std::vector<std::string>> rows{};
	while(getline(file, raw_line)){

		// Reference for the following code, dealing with delimiters inside quotes:
		// https://stackoverflow.com/questions/35639083/c-csv-line-with-commas-and-strings-within-double-quotes/35639947
		const char *row_ptr = raw_line.c_str();
		bool in_quotes {false};
		const char *start = row_ptr;
		for(const char *cur_char = start; *cur_char; ++cur_char) {
			if(*cur_char == '"') {
				in_quotes = !in_quotes;
			} else if (*cur_char == csvp._delim && !in_quotes) {
				parsed_row.push_back(std::string(start, cur_char - start)); // add word to list
				start = cur_char + 1; // set start to beginning of next word
			}
		}
		parsed_row.push_back(std::string(start)); // push last word, trailing final delim

		csvp._rows.push_back(parsed_row);
		parsed_row.clear();
	}
	return csvp;
}

// CSVParser::makeSegments() is used to create a vector of ColumnData structs
// It is expected that the user calls this function to obtain the vector of ColumnData
// structs, then iterates through each vector calling CSVParser::runAnalysisSegment.
template <typename T>
std::vector<ColumnData<T>> CSVParser::makeSegments(
		const std::string &target_header, 
		const index_type header_row_index, 
		const index_type first_data_row_index, 
		const index_type num_segments) const 
{
	std::vector<ColumnData<T>> results;
	for(index_type cur_seg {0}; cur_seg < num_segments; ++cur_seg) {
		ColumnData<T> cur_cd{};
		cur_cd.num_segments = num_segments;
		cur_cd.cur_segment = cur_seg;
		cur_cd.header = target_header;
		cur_cd.first_data_row_index = first_data_row_index;
		cur_cd.header_row_index = header_row_index;
		results.push_back(cur_cd);
	}
	// Work out indices for all segments
	std::vector<index_type> segment_indices{};
	index_type num_rows = _rows.size() - first_data_row_index;
	for(index_type seg {0}; seg < num_segments; ++seg) {
		index_type first_index_of_seg = (seg * num_rows)/num_segments + first_data_row_index;
		segment_indices.push_back(first_index_of_seg);
	}

	// Find header_col_index
	index_type header_col_index;
	{
		auto header_col {_rows.at(header_row_index)};
		auto header {std::find(header_col.begin(), header_col.end(), target_header)};
		if(header == header_col.end()) {
			throw std::runtime_error("Target header not found");
		} else {
			header_col_index = std::distance(header_col.begin(), header);	
		}
	}

	// now set segment_indices and num_rows for all ColumnData objects in results
	for(auto &cur_cd : results) {
		cur_cd.segment_indices = segment_indices;
		cur_cd.num_rows = num_rows;
		cur_cd.header_col_index = header_col_index;
		preprocess(cur_cd);
	}

	return results;
}
template std::vector<ColumnData<int_data_type>> CSVParser::makeSegments(
		const std::string &target_header, 
		const index_type header_row_index, 
		const index_type first_data_row_index, 
		const index_type num_segments) const;
template std::vector<ColumnData<float_data_type>> CSVParser::makeSegments(
		const std::string &target_header, 
		const index_type header_row_index, 
		const index_type first_data_row_index, 
		const index_type num_segments) const;

// CSVParser::preprocess() ensures that target_header exists, records the column
// target_header is found, and creates segment_indices
template <typename T>
void CSVParser::preprocess( ColumnData<T> &cd ) const {
	cd.header_col_index = 0;
	bool index_found {false};
	for(auto word : _rows.at(cd.header_row_index)) {
		if(word == cd.header) {
			index_found = true;
			// indicates cd.header_col_index is set correctly
			break;
		}
		++cd.header_col_index;
	}
	if(!index_found) {
		throw std::runtime_error("Target header not found");
	}

	index_type end_index;
	if(cd.cur_segment == cd.num_segments - 1) {
		// last segment, so end_index = last row index
		end_index = _rows.size() - 1;
	} else {
		end_index = cd.segment_indices.at(cd.cur_segment + 1) - 1;
	}
	for(index_type cur_row {cd.segment_indices.at(cd.cur_segment)}; cur_row <= end_index; ++cur_row) {
		// storing each value into a string stream, because the >> operator
		// can convert into a variety of numeric types.
		T val;
		std::stringstream converter_hack {_rows.at(cur_row).at(cd.header_col_index)};
		converter_hack >> val;
		cd.data_raw.push_back(val);
	}
}
template void CSVParser::preprocess(
		ColumnData<int_data_type> &cd) const;
template void CSVParser::preprocess(
		ColumnData<float_data_type> &cd) const;


// runAnalysis() will do the complete aggregation in one session
// In contrast, runAnalysisSegment() is used to
// aggregate just one segment of the data
template <typename T>
void CSVParser::runAnalysis(
		ColumnData<T> &cd,
		const std::string &target_header, 
		const index_type header_row_index,
		const index_type first_data_row_index) const {
	int_data_type sum {0};
	for(index_type cur_row {first_data_row_index}; cur_row < _rows.size(); ++cur_row) {
		sum += std::stoull(_rows.at(cur_row).at(cd.header_col_index));
	}
	cd.data_summary_actual = sum;
	cd.data_summary_projected = sum;
}
// Currently depricated, will remove soon...
template void CSVParser::runAnalysis(
		ColumnData<int_data_type> &cd,
		const std::string &target_header,
		const index_type header_row_index,
		const index_type first_data_row_index) const;
template void CSVParser::runAnalysis(
		ColumnData<float_data_type> &cd,
		const std::string &target_header,
		const index_type header_row_index,
		const index_type first_data_row_index) const;


template <typename T>
void CSVParser::runAnalysisSegment(	ColumnData<T> &cd ) const
{
	for(const auto &val : cd.data_raw) {
		cd.data_summary_actual += val;
	}
}
template void CSVParser::runAnalysisSegment(
		ColumnData<int_data_type> &cd) const;
template void CSVParser::runAnalysisSegment(
		ColumnData<float_data_type> &cd) const;
