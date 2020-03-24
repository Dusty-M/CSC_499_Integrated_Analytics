// LeastSquaresFit.cpp

#include "LeastSquaresFit.hpp"
#include <iostream>
#include <utility> //std::pair
#include <numeric> // std::inner_product
#include <chrono>

using vec_col_float = std::vector<ColumnData<float_data_type>>;
using vec_col_int = std::vector<ColumnData<int_data_type>>;

template <typename X_type, typename Y_type>
void runProfile(const index_type num_segments, const CSVParser &csv,
		const std::string &X_header, const std::string &Y_header,
		const index_type header_row_index, const index_type first_data_row_index) {

	using vec_cols_tX = std::vector<ColumnData<X_type>>;
	using vec_cols_tY = std::vector<ColumnData<Y_type>>;
	using time_nano_t = std::chrono::nanoseconds;

	// If num_segments != 1, run the analysis with a single segment (non-progressive)
	// to obtain the true y-intercept and slope.  Use these values to determine
	// the error during progressive calculations
	float_data_type a_actual, b_actual;
//	if(num_segments != 1) {
	{
		auto lsf {makeLeastSquaresFit<vec_cols_tX, vec_cols_tY>(
			csv.makeSegments<X_type>(
				X_header,
				header_row_index, 
				first_data_row_index, 
				1),
			csv.makeSegments<Y_type>(
				Y_header,
				header_row_index, 
				first_data_row_index, 
				1))};
		lsf.calcNextProjection(); // performs complete calculation
		a_actual = lsf.getProja();
		b_actual = lsf.getProjb();
	}
//	}

	// Setup for progressive calculation
	auto Xs {csv.makeSegments<X_type>(X_header, 
		header_row_index, first_data_row_index, num_segments)};
	auto Ys {csv.makeSegments<Y_type>(Y_header, 
		header_row_index, first_data_row_index, num_segments)};
	auto lsf {makeLeastSquaresFit<vec_cols_tX , vec_cols_tY>(Xs, Ys)};

	index_type cur_seg {0};
	auto start {std::chrono::system_clock::now()};
	while(lsf.calcNextProjection()) {
		auto cur_a_proj {lsf.getProja()};
		auto cur_b_proj {lsf.getProjb()};
		auto cur_a_error {std::abs(cur_a_proj - a_actual)};
		auto cur_b_error {std::abs(cur_b_proj - b_actual)};

		std::cout << "a: " << cur_a_proj << ", b: " << cur_a_proj
			<< ", a error: " << cur_a_error
			<< ", b error: " << cur_b_error;
		auto cur {std::chrono::system_clock::now()};
		std::cout << ", segment: " << cur_seg++ << ", Time elapsed" << ": "
			<< std::chrono::duration_cast<time_nano_t>(cur - start).count() << " ns" << std::endl;
	}
	auto cur {std::chrono::system_clock::now()};
	std::cout << "time elapsed [total]: " 
		<< std::chrono::duration_cast<time_nano_t>(cur - start).count() 
		<< " ns" << std::endl;
}	
template void runProfile<float_data_type, float_data_type>(
	const index_type num_segments, const CSVParser &csv,
	const std::string &X_header, const std::string &Y_header,
	const index_type header_row_index, const index_type first_row_data_index);

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type>::LeastSquaresFit(X_type X, Y_type Y): 
    	_x_bar {0}, _y_bar {0}, _X {X}, _Y {Y}, 
		_SS_xx {0}, _SS_xy {0}, _a {0}, _b {0},
		_count {0} {} 
template class LeastSquaresFit<vec_col_int, vec_col_int>; 
template class LeastSquaresFit<vec_col_float, vec_col_float>;
	 
template <typename X_type, typename Y_type> 
void LeastSquaresFit<X_type, Y_type>::init() { 
    _x_bar = calcAvg(_X); 
    _y_bar = calcAvg(_Y); 
}

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type> makeLeastSquaresFit(
		X_type X, Y_type Y) {
	LeastSquaresFit<X_type, Y_type> lsf{X, Y};
	lsf.init();
	return lsf;
}
template LeastSquaresFit<vec_col_float, vec_col_float>
	makeLeastSquaresFit(vec_col_float X, vec_col_float Y);

template <typename X_type, typename Y_type>
bool LeastSquaresFit<X_type, Y_type>::calcNextProjection() {
	if(_count == _X.size()) { // indicates no update to projection was made
		return false;
	}
	
	const auto &X_cur_seg {_X.at(_count).data_raw};
	const auto &Y_cur_seg {_Y.at(_count).data_raw};

	// use inner product to calculate SS_xx and SS_xy
	float_data_type init_SS_xx {-(X_cur_seg.size() * _x_bar * _x_bar)};
	float_data_type init_SS_xy {-(Y_cur_seg.size() * _x_bar * _y_bar)};
	float_data_type cur_SS_xx {std::inner_product(X_cur_seg.begin(), X_cur_seg.end(), 
		X_cur_seg.begin(), init_SS_xx)};
	float_data_type cur_SS_xy {std::inner_product(X_cur_seg.begin(), X_cur_seg.end(), 
		Y_cur_seg.begin(), init_SS_xy)};

	// now add cur_SS_xx and cur_SS_xy to persistent
	// values, _SS_xx and _SS_xy
	_SS_xx += cur_SS_xx;
	_SS_xy += cur_SS_xy;

	// update values of _a, _b based on new sums of squares
	_b = _SS_xy / _SS_xx;
	_a = _y_bar - _b * _x_bar;

	++_count;
	return true;
}
template bool LeastSquaresFit<vec_col_float, vec_col_float>::calcNextProjection();
template bool LeastSquaresFit<vec_col_int, vec_col_int>::calcNextProjection();

template <typename X_type, typename Y_type>
float_data_type LeastSquaresFit<X_type, Y_type>::getProja(){ return _a; }
template float_data_type LeastSquaresFit<vec_col_int, vec_col_int>::getProja();
template float_data_type LeastSquaresFit<vec_col_float, vec_col_float>::getProja();

template <typename X_type, typename Y_type>
float_data_type LeastSquaresFit<X_type, Y_type>::getProjb(){ return _b; }
template float_data_type LeastSquaresFit<vec_col_int, vec_col_int>::getProjb();
template float_data_type LeastSquaresFit<vec_col_float, vec_col_float>::getProjb();

template <typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<X_type, Y_type> &lsf) {
	std::cout 	<< "_x_bar: " << lsf._x_bar  << "\n"
				<< "_y_bar: " << lsf._y_bar << "\n"
				<< "_a: " << lsf._a << "\n"
				<< "_b: " << lsf._b << "\n"
				<< "_count: " << lsf._count << std::endl;
	return os;
}
template std::ostream &operator<<(std::ostream &os, 
	const LeastSquaresFit<vec_col_int, vec_col_int> &lsf);
template std::ostream &operator<<(std::ostream &os, 
	const LeastSquaresFit<vec_col_float, vec_col_float> &lsf);
