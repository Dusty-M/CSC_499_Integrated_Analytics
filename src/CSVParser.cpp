// CSVParser.cpp

#include "CSVParser.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>

template <typename T>
ColumnData<T>::ColumnData() :
	header {""},
	header_col_index {0},
	num_rows {0},
	data_actual {0},
	data_projected {0},
	num_segments {0},
	cur_segment {0},
	segment_indices {std::vector<uint32_t>{}}{}
template struct ColumnData<uint64_t>;

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd) {
	os << "header: " << cd.header << 
		"\nfirst_data_row_index: " << cd.first_data_row_index <<
		"\nheader_row_index: " << cd.header_row_index <<
		"\nheader_col_index: " << cd.header_col_index <<
		"\nnum_rows: " << cd.num_rows <<
		"\ndata_actual: " << cd.data_actual <<
		"\ndata_projected: " << cd.data_projected <<
		"\nnum_segments: " << cd.num_segments <<
		"\ncur_segment: " << cd.cur_segment <<
		"\nsegment_indices: [ ";
	for(auto index : cd.segment_indices) {
		std::cout << index << " ";
	}
	std::cout << "]" << std::flush;
	return os;
}
template std::ostream &operator<<(std::ostream &os, const ColumnData<uint64_t> &cd);

template <typename T>
bool ColumnData<T>::operator==(const ColumnData<T> &cd) const {
	return(	header == cd.header && 
			first_data_row_index == cd.first_data_row_index &&
			header_row_index == cd.header_row_index &&
			header_col_index == cd.header_col_index &&
			num_rows == cd.num_rows &&
			data_actual == cd.data_actual &&
			data_projected == cd.data_projected &&
			num_segments == cd.num_segments &&
			cur_segment == cd.cur_segment &&
			segment_indices == cd.segment_indices);
}
template bool ColumnData<uint64_t>::operator==(const ColumnData<uint64_t> &cd) const;


CSVParser::CSVParser(const std::string& filename, const char delim) : 
	_delim {delim}, _filename {filename} 
	{
		readData();
	}

void CSVParser::readData() {
	std::ifstream file {_filename};
	if(file.fail()) {
		throw std::runtime_error("Filename does not exist");
	}
	std::string raw_line{};
	std::vector<std::string> parsed_row{};
	while(getline(file, raw_line)){

		// Reference for the following code, dealing with delimiters inside quotes:
		// https://stackoverflow.com/questions/35639083/c-csv-line-with-commas-and-strings-within-double-quotes/35639947
		const char *row_ptr = raw_line.c_str();
		bool in_quotes {false};
		const char *start = row_ptr;
		for(const char *cur_char = start; *cur_char; ++cur_char) {
			if(*cur_char == '"') {
				in_quotes = !in_quotes;
			} else if (*cur_char == _delim && !in_quotes) {
				parsed_row.push_back(std::string(start, cur_char - start)); // add word to list
				start = cur_char + 1; // set start to beginning of next word
			}
		}
		parsed_row.push_back(std::string(start)); // push last word, trailing final delim

		_rows.push_back(parsed_row);
		parsed_row.clear();
	}
}

// CSVParser::makeSegments() is used to create a vector of ColumnData structs
// It is expected that the user calls this function to obtain the vector of ColumnData
// structs, then iterates through each vector calling CSVParser::getDataSegment.
template <typename T>
std::vector<ColumnData<T>> CSVParser::makeSegments(
		const std::string &target_header, 
		const uint32_t header_row_index, 
		const uint32_t first_data_row_index, 
		const uint32_t num_segments) const 
{
	std::vector<ColumnData<T>> results;
	for(uint32_t cur_seg {0}; cur_seg < num_segments; ++cur_seg) {
		ColumnData<T> cur_cd{};
		cur_cd.num_segments = num_segments;
		cur_cd.cur_segment = cur_seg;
		cur_cd.header = target_header;
		cur_cd.first_data_row_index = first_data_row_index;
		cur_cd.header_row_index = header_row_index;
		results.push_back(cur_cd);
	}
	// Work out indices for all segments
	std::vector<uint32_t> segment_indices{};
	uint32_t num_rows = _rows.size() - first_data_row_index;
	for(uint32_t seg {0}; seg < num_segments; ++seg) {
		uint32_t first_index_of_seg = (seg * num_rows)/num_segments + first_data_row_index;
		segment_indices.push_back(first_index_of_seg);
	}

	// Find header_col_index
	uint32_t header_col_index {0};
	bool index_found {false};
	for(auto word : _rows.at(header_row_index)) {
		if(word == target_header) {
			index_found = true;
			// indicates header_col_index is set correctly
			break;
		}
		++header_col_index;
	}
	if(!index_found) {
		throw std::runtime_error("Target header not found");
	}

	// now set segment_indices and num_rows for all ColumnData objects in results
	for(auto &cur_cd : results) {
		cur_cd.segment_indices = segment_indices;
		cur_cd.num_rows = num_rows;
		cur_cd.header_col_index = header_col_index;
	}

	return results;
}
template std::vector<ColumnData<uint64_t>> CSVParser::makeSegments(
		const std::string &target_header, 
		const uint32_t header_row_index, 
		const uint32_t first_data_row_index, 
		const uint32_t num_segments) const;

// CSVParser::preprocess() ensures that target_header exists, records the column
// target_header is found, and creates segment_indices
template <typename T>
void CSVParser::preprocess(
		ColumnData<T> &cd) const {
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
}
template void CSVParser::preprocess(
		ColumnData<uint64_t> &cd) const;


// getData() will do the complete aggregation in one session
// In contrast, getDataSegment() is used to
// aggregate just one segment of the data
template <typename T>
void CSVParser::getData(
		ColumnData<T> &cd,
		const std::string &target_header, 
		const uint64_t header_row_index,
		const uint64_t first_data_row_index) const {
	uint64_t sum {0};
	for(uint64_t cur_row {first_data_row_index}; cur_row < _rows.size(); ++cur_row) {

		// NOTE: This line requires that the data being aggregated is an integral type
		sum += std::stoull(_rows.at(cur_row).at(cd.header_col_index));
	}
	cd.data_actual = sum;
	cd.data_projected = sum;
}
template void CSVParser::getData(
		ColumnData<uint64_t> &cd,
		const std::string &target_header,
		const uint64_t header_row_index,
		const uint64_t first_data_row_index) const;

template <typename T>
void CSVParser::getDataSegment(
		ColumnData<T> &cd)
{
	uint32_t end_index;
	if(cd.cur_segment == cd.num_segments - 1) {
		// last segment, so end_index = last row index
		end_index = _rows.size() - 1;
	} else {
		end_index = cd.segment_indices.at(cd.cur_segment + 1) - 1;
	}
	for(uint32_t cur_row {cd.segment_indices.at(cd.cur_segment)}; cur_row <= end_index; ++cur_row) {
		cd.data_actual += stoull(_rows.at(cur_row).at(cd.header_col_index));
	}
}
template void CSVParser::getDataSegment(
		ColumnData<uint64_t> &cd);
