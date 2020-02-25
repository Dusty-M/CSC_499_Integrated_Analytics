// CSVParser.hpp

#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP
#include <vector>
#include <string>
#include <ostream>

using int_data_type = uint64_t; 
using index_type = uint32_t; 
class CSVParser;
// Each ColumnData struct is associated with a particular
// CSVParser object.  Upon the first call to runAnalysis() with a
// particular ColumnData, the segment_indices are populated
// based on the amount of data found in the CSVParser
template <typename T>
struct ColumnData {
	std::string header;
	index_type first_data_row_index;
	index_type header_row_index;
	index_type header_col_index;
	index_type num_rows; // indicates total number of data values
	T data_summary_actual;
	T data_summary_projected; // guess based on partial data
	std::vector<T> data_raw;
	index_type num_segments;
	index_type cur_segment;
	
	// segment_indices stores the index holding the first
	// element of data in the ith segment.  
	// ie the first element of the 4th segment can be found
	// NOTE: i goes from 0 to (num_segments - 1)
	// in _rows.at(segment_indices[2]
	std::vector<index_type> segment_indices;
	bool operator==(const ColumnData<T> &cd) const;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const ColumnData<T> &cd);
CSVParser makeCSVParser(const std::string& filename, const char delim);

class CSVParser {
public:
	CSVParser(const std::string& filename, const char delim);
	~CSVParser() {}
	bool operator==(CSVParser csvp);

	template <typename T>
	std::vector<ColumnData<T>> makeSegments(
			const std::string &target_header,
			const index_type header_row_index,
			const index_type first_data_row_index,
			const index_type num_segments) const;

	template <typename T>
	void preprocess(ColumnData<T> &cd) const;

	template <typename T>
	void runAnalysis(	
		ColumnData<T> &cd,
		const std::string &target_header, 
		const index_type header_index, 
		const index_type first_data_row_index) const;

	template <typename T>
	void runAnalysisSegment(
		ColumnData<T> &cd) const;

	CSVParser &readData();
private:
	std::vector<std::vector<std::string>> _rows;
	char _delim;
	std::string _filename;
};

#endif
