// CSVParser.cpp

#include "CSVParser.hpp"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd) {
	os << cd.header << ": " << cd.data;
	return os;
}

template std::ostream &operator<<(std::ostream &os, const ColumnData<uint64_t> &cd);

template <typename T>
bool ColumnData<T>::operator==(const ColumnData<T> &cd) const {
	return(header == cd.header && data == cd.data);
}

template bool ColumnData<uint64_t>::operator==(const ColumnData<uint64_t> &cd) const;

template struct ColumnData<uint64_t>;
//template struct ColumnData<double>;

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

template <typename T>
ColumnData<T> CSVParser::getData(std::string target_header, 
		uint64_t header_row_index,
		uint64_t first_data_row_index) {
	// Find target header relevant row
	uint index {0};
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
	uint64_t sum {0};
	for(uint64_t cur_row {first_data_row_index}; cur_row < _rows.size(); ++cur_row) {

		// NOTE: This line requires that the data being aggregated is an integral type
		sum += std::stoull(_rows.at(cur_row).at(index));
	}
	ColumnData<T> result;
	result.data = sum;
	result.header = target_header;
	return result;
}

template ColumnData<uint64_t> CSVParser::getData(std::string target_header,
		uint64_t header_row_index,
		uint64_t first_data_row_index);
