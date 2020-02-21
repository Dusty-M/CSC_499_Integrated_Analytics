// myLib.cpp

#include "myLib.hpp"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

std::ostream &operator<<(std::ostream &os, const ColumnData &cd) {
	os << cd.header << ": " << cd.data;
	return os;
}

bool ColumnData::operator==(ColumnData &cd) {
	return(header == cd.header && data == cd.data);
}

CSVParser::CSVParser(const std::string& filename, char const delim) : 
	_delim {delim}, _filename {filename} {}

ColumnData CSVParser::getData(std::string target_header) {
	std::ifstream file {_filename};
	std::string raw_line{};
	std::vector<std::string> parsed_row{};

	// First row contains non-unique column header names
	getline(file, raw_line);
	boost::split(parsed_row, raw_line, boost::is_any_of(std::string{1, _delim}));
	_rows.push_back(parsed_row);
	parsed_row.clear();

	// Second row contains unique column header indices, this is what is
	// currently being used to identify target columns
	getline(file, raw_line);
	boost::split(parsed_row, raw_line, boost::is_any_of(std::string{1, _delim}));
	_rows.push_back(parsed_row);
	uint index {0};
	bool index_found {false};
	for(auto word : parsed_row) {
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

	parsed_row.clear(); // remove headers from previous operations
	unsigned long long sum = 0;
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

		// raw line has been completely parsed at delim 
		sum += std::stoull(parsed_row.at(index));
		_rows.push_back(parsed_row);
		parsed_row.clear();
	}
	ColumnData cd;
	cd.header = target_header;
	cd.data = sum;
	return cd;
}
