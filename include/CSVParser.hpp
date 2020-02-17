// CSVParser.hpp

#include <vector>
#include <string>
#include <ostream>

#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP

template <typename T>
struct ColumnData {
	std::string header;
	T data;
	bool operator==(const ColumnData<T> &cd) const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd);

class CSVParser {
public:
	CSVParser(const std::string& filename, const char delim);
	~CSVParser() {}
	bool operator==(CSVParser csvp);
	void readData();

	template <typename T>
	ColumnData<T> getData(	std::string target_header, 
						uint64_t header_index, 
						uint64_t first_data_row_index);
private:
	std::vector<std::vector<std::string>> _rows;
	char _delim;
	std::string _filename;
};

#endif
