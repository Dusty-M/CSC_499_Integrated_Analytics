#include "myLib.hpp"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>


CSVParser::CSVParser(const std::string& filename, char const delim) : 
	_delim {delim}, _filename {filename} {}

ColumnData CSVParser::getData(std::string target_header) {
	std::ifstream file {_filename};
	std::vector<std::string> parsed_line{};
	std::string raw_line{};
	std::vector<std::string> parsed_row{};
	std::vector<std::string> raw_lines{};

	// Headers are found in the 2nd row, so getline is called twice
	getline(file, raw_line);
	getline(file, raw_line);

	boost::split(parsed_row, raw_line, boost::is_any_of(std::string{1, _delim}));
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
	// ************** Temporary workaround: there is a quote in a data field ******
	// To deal with it for now, I'm just adding 1 to index, this has to be addressed
	// The tactic: split only on commas not between double quotes
	++index;

	// At this stage, a simple aggregation is be done:
	// A simple total sum
	unsigned long long sum = 0;
	while(getline(file, raw_line)){
		boost::split(parsed_row, raw_line, boost::is_any_of(std::string{1, _delim}));
		sum += std::stoull(parsed_row.at(index));
	}
	ColumnData cd;
	cd.header = target_header;
	cd.data = sum;
	return cd;
}
