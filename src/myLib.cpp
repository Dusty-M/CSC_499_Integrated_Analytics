#include "myLib.hpp"

CSVParser::CSVParser(const std::string& filename, char const delim) : 
	_delim {delim}, _filename {filename} {}

void CSVParser::getData() {

	/* Raw test code - making use of boost::split
	*********************************************
	std::string line("test\ttest2\ttest3");
	std::vector<std::string> strs;
	boost::split(strs,line,boost::is_any_of("\t"));

	std::cout << "* size of the vector: " << strs.size() << std::endl;    
	for (size_t i = 0; i < strs.size(); i++) {
		std::cout << strs[i] << std::endl;
	}
	*********************************************/
	std::ifstream file {_filename};
	std::vector<std::string> parsed_line{};
	std::string raw_line{};
	std::vector<std::string> raw_lines{};
	while(getline(file, raw_line)){
//		boost::split(parsed_line, raw_line, boost::is_any_of(","));
		raw_lines.push_back(raw_line);
	}
	std::cout << "resulting strings:" << std::endl;
	for(auto line : raw_lines) {
		std::cout << line << std::endl;
	}
}
