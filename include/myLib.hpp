// Mylib.hpp

#include <vector>
#include <string>
#include <ostream>

#ifndef MYLIB_HPP
#define MYLIB_HPP

struct ColumnData {
	std::string header;
	unsigned long long data;
	bool operator==(ColumnData &cd);
};

std::ostream &operator<<(std::ostream &os, const ColumnData &cd);

class CSVParser {
public:
	CSVParser(const std::string& filename, char delim);
	~CSVParser() {}
	bool operator==(CSVParser csvp);
	ColumnData getData(std::string target_header);
private:
	std::vector<std::vector<std::string>> _rows;
	char _delim;
	std::string _filename;
};

#endif
