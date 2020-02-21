// CSVParser.hpp

#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP
#include <vector>
#include <string>
#include <ostream>

// Each ColumnData struct is associated with a particular
// CSVParser object.  Upon the first call to getData() with a
// particular ColumnData, the segment_indices are populated
// based on the amount of data found in the CSVParser
template <typename T>
struct ColumnData {
	ColumnData<T>(); // constructor

	std::string header;
	uint32_t header_index;
	uint32_t num_rows; // indicates total number of data values
	T data_actual;
	T data_projected; // guess based on partial data
	uint32_t num_segments;
	uint32_t cur_segment;

	// segment_indices stores the index holding the first
	// element of data in the ith segment.  
	// ie the first element of the 4th segment can be found
	// NOTE: i goes from 0 to (num_segments - 1)
	// in _rows.at(segment_indices[2]
	std::vector<uint32_t> segment_indices;
	bool operator==(const ColumnData<T> &cd) const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd);

class CSVParser {
public:
	CSVParser(const std::string& filename, const char delim);
	~CSVParser() {}
	bool operator==(CSVParser csvp);

	template <typename T>
	ColumnData<T> getData(	
		const std::string &target_header, 
		const uint64_t header_index, 
		const uint64_t first_data_row_index);

	template <typename T>
	void getDataSegment(
       const std::string &target_header,
        const uint32_t header_row_index,
        const uint32_t first_data_row_index,
        ColumnData<T> &cd,
        const uint32_t num_segments,
        const uint32_t cur_segment);
private:
	void readData();
	std::vector<std::vector<std::string>> _rows;
	char _delim;
	std::string _filename;
};

#endif
