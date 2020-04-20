// LeastSquaresFit.hpp
// Utility for calculating a least squares best fit line

#ifndef LEASTSQUARESFIT_HPP
#define LEASTSQUARESFIT_HPP
#include "CSVParser.hpp"
#include <functional>

template <typename X_type, typename Y_type>
class LeastSquaresFit;

// a projection is the best guess with current data of the line of best fit
struct projection{
    float_data_type a_proj; // represents y-intercept
    float_data_type b_proj; // represents slope
};

template<typename X_type, typename Y_type>
struct point {
	X_type x;
	Y_type y;
};

template<typename X_DataCol_type, typename Y_DataCol_type>
struct segment {
	segment(X_DataCol_type X_arg, Y_DataCol_type Y_arg, 
		float_data_type x_bar_arg, float_data_type y_bar_arg): 
		X {X_arg}, Y {Y_arg}, x_bar {x_bar_arg}, y_bar {y_bar_arg}{}
	X_DataCol_type X;
	Y_DataCol_type Y;
	float_data_type x_bar;
	float_data_type y_bar;
};


template<typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const point<X_type, Y_type> &pt);

// a summary is a complete accounting of a progressive
// analysis, including a projection, a set of representative
// data points, the number of segments processed so far,
// and intermediate values SS_xx, SS_xy, x_bar, y_bar
template <typename X_type, typename Y_type>
struct summary{
	projection proj;
	std::vector< point< X_type, Y_type > > representative_pts;
	float_data_type 	SS_xx, 
						SS_xy, 
						x_bar, 
						y_bar;
	index_type count; // represents number of segments aggregated thus far
};

template <typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const summary<X_type, Y_type> &sum);

projection error(projection const p1, projection const p2);

template <typename T>
std::ostream &operator << (std::ostream &s, projection const proj);

template <typename Point_type>
std::vector<Point_type> get_points(const index_type n, const float_data_type dy,
		const std::vector<Point_type> &all_points,
		const std::function<float_data_type(float_data_type)> &&f_x);

template <typename X_type, typename Y_type>
void runProfile(const index_type num_segments, const CSVParser &csv,
	const std::string &X_header, const std::string &Y_header,
	const index_type header_row_index, const index_type first_row_data_index);

// Note: X_type and Y_type are expected to be std::vector<ColumnData<T>>
// where T is some numeric type such as float, double, unsigned int, etc.
template <typename X_type, typename Y_type>
class LeastSquaresFit {
public:
	template <typename T1, typename T2>
	friend std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<T1, T2> &lsf);

    LeastSquaresFit(X_type X, Y_type Y);
    void init();
	// NOTE: either use calcNextProjection or use getNextSegment.  
	// DO NOT MIX USAGE for a given calculation
	// TODO: remove calcNextProjection interface.  Rely only on getNextSegment.
	// Rationale: 	- calcNextProjection was designed to be used in sequential calculation
	// 				- getNextSegment can be used for concurrent calculation
	segment<typename X_type::value_type, typename Y_type::value_type> getNextSegment();
	bool calcNextProjection(); 
	float_data_type getProja();
	float_data_type getProjb();

private:
	// In the cast of a progressive analysis, there will be many segmented
	// ColumnData structs, therefore _X and _Y are vectors of ColumnData structs.
    X_type _X;
    Y_type _Y;

    // Values specific to the least squares fitting calculation
    // See the following link for a description of variables used:
    // http://mathworld.wolfram.com/LeastSquaresFitting.html
    float_data_type _x_bar,
                    _y_bar,
                    _SS_xx,
                    _SS_xy,
                    _a,
                    _b;
	index_type _count; // indicates how many projections have been made
};

// Factory function, as above, X_type and Y_type are expected to be
// of type std::vector<ColumnData<T>> where T is a numeric type
template <typename X_type, typename Y_type>
LeastSquaresFit<X_type, Y_type> makeLeastSquaresFit(X_type X, Y_type Y);

// Overloaded function create_summary. 
// - provide 2 summaries as arguments and it will return a summary OR
// - provide raw data as an argument and it will return a summary

template <typename X_type, typename Y_type>
//summary<X_type, Y_type> create_summary(std::vector<X_type> X, std::vector<Y_type> Y);
summary<X_type, Y_type> create_summary(segment<ColumnData<X_type>, ColumnData<Y_type>> seg);


template <typename X_type, typename Y_type>
summary<X_type, Y_type> create_summary(summary<X_type, Y_type> s1, summary<X_type, Y_type> s2);

#endif
