// Mylib.hpp

#include <vector>
#include <string>
//#include <boost/algorithm/string.hpp>
//#include <iostream>
//#include <fstream>

//template <class T>
struct ColumnData {
	std::string header;
	unsigned long long data;
};

class CSVParser {
public:
	CSVParser(const std::string& filename, char delim);
	~CSVParser() {}
	ColumnData getData(std::string target_header);
private:
	std::vector<std::vector<std::string>> rows;
	char _delim;
	std::string _filename;
};
