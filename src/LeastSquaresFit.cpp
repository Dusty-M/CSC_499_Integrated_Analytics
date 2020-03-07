// LeastSquaresFit.cpp

#include "LeastSquaresFit.hpp"
#include <iostream>
#include <utility> //std::pair
#include <numeric> // std::inner_product

using vec_col_float = std::vector<ColumnData<float_data_type>>;
using vec_col_int = std::vector<ColumnData<int_data_type>>;

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
	float_data_type init_SS_xx = -(X_cur_seg.size() * _x_bar * _x_bar);
	float_data_type init_SS_xy = -(Y_cur_seg.size() * _x_bar * _y_bar);
	float_data_type cur_SS_xx = std::inner_product(X_cur_seg.begin(), X_cur_seg.end(), 
		X_cur_seg.begin(), init_SS_xx);
	float_data_type cur_SS_xy = std::inner_product(X_cur_seg.begin(), X_cur_seg.end(), 
		Y_cur_seg.begin(), init_SS_xy);

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
