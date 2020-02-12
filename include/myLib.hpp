#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

class CSVParser {
public:
	CSVParser(const std::string& filename, char delim);
	~CSVParser() {}
	void getData();
private:
	std::vector<std::vector<std::string>> rows;
	char _delim;
	std::string _filename;
};
