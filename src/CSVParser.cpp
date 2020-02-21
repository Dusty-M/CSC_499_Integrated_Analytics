// CSVParser.cpp

#include "CSVParser.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>

template <typename T>
ColumnData<T>::ColumnData() :
	header {""},
	header_index {0},
	num_rows {0},
	data_actual {0},
	data_projected {0},
	num_segments {0},
	cur_segment {0},
	segment_indices {std::vector<uint32_t>{}}
{std::cout << "In ColumnData constructor" << std::endl;}
template struct ColumnData<uint64_t>;
//template struct ColumnData<double>;

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd) {
	os << "header: " << cd.header << 
		"\nheader_index: " << cd.header_index <<
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

// getData() will do the complete aggregation in one session
// In contrast, getDataSegment() is used to
// aggregate just one segment of the data
template <typename T>
ColumnData<T> CSVParser::getData(
		const std::string &target_header, 
		const uint64_t header_row_index,
		const uint64_t first_data_row_index) {
	uint32_t index {0};
	bool index_found {false};
	for(auto word : _rows.at(header_row_index)) {
		if(word == target_header) {
			index_found = true;
			break;
		}
		++index;
	}
	if(!index_found) {
		std::cout << "Error: target header not found.  "
			<< "program closing..." << std::endl;
		throw std::runtime_error("Target header not found");
	}
	// Setup complete.  
	uint64_t sum {0};
	for(uint64_t cur_row {first_data_row_index}; cur_row < _rows.size(); ++cur_row) {

		// NOTE: This line requires that the data being aggregated is an integral type
		sum += std::stoull(_rows.at(cur_row).at(index));
	}
	ColumnData<T> result;
	result.data_actual = sum;
	result.header = target_header;
	return result;
}

template ColumnData<uint64_t> CSVParser::getData(
		const std::string &target_header,
		const uint64_t header_row_index,
		const uint64_t first_data_row_index);

template <typename T>
void CSVParser::getDataSegment(
		const std::string &target_header, 
		const uint32_t header_row_index,
		const uint32_t first_data_row_index,
		ColumnData<T> &cd,
		const uint32_t num_segments,
		const uint32_t cur_segment)
{
	cd.header_index = 0;
	bool index_found {false};
	for(auto word : _rows.at(header_row_index)) {
		if(word == target_header) {
			index_found = true;
			break;
		}
		++cd.header_index;
	}
	if(!index_found) {
		std::cout << "Error: target header not found.  "
			<< "program closing..." << std::endl;
		throw std::runtime_error("Target header not found");
	}
	// setup complete

	std::cout << "in getDataSegment" << std::endl;
	// work out segment indices
	cd.num_rows = _rows.size() - first_data_row_index;
	for(uint32_t seg {0}; seg < num_segments; ++seg) {
		auto first_index_of_seg = (seg * cd.num_rows)/num_segments + first_data_row_index;
		cd.segment_indices.push_back(first_index_of_seg);
	}
	uint32_t end_index;
	if(cd.cur_segment == num_segments - 1) {
		// last segment, so end_index = last row index
		end_index = _rows.size() - 1;
	} else {
		end_index = cd.segment_indices.at(cd.cur_segment + 1) - 1;
	}
	for(uint32_t cur_row {cd.segment_indices.at(cur_segment)}; cur_row <= end_index; ++cur_row) {
		cd.data_actual += stoull(_rows.at(cur_row).at(cd.header_index));
	}
}

template void CSVParser::getDataSegment(
		const std::string &target_header, 
		const uint32_t header_row_index,
		const uint32_t first_data_row_index,
		ColumnData<uint64_t> &cd,
		const uint32_t num_segments,
		const uint32_t cur_segment);
