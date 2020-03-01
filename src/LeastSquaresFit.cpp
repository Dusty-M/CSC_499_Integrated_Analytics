// LeastSquaresFit.cpp

#include "LeastSquaresFit.hpp"
#include <iostream>
#include <utility> //std::pair

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type>::LeastSquaresFit(
		std::vector<ColumnData<X_type>> X, 
		std::vector<ColumnData<Y_type>> Y): 
    	_x_bar {0}, _y_bar {0}, _X {X}, _Y {Y}, 
		_SS_xx {0}, _SS_xy {0}, _a {0}, _b {0}, 
		_cur_projection {0} {} 
template class LeastSquaresFit<int_data_type, int_data_type>; 
template class LeastSquaresFit<float_data_type, float_data_type>; 
 
template <typename X_type, typename Y_type> 
void LeastSquaresFit<X_type, Y_type>::init() { 
    _x_bar = calcAvg(_X); 
    _y_bar = calcAvg(_Y); 
}

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type> makeLeastSquaresFit(
		std::vector<ColumnData<X_type>> X, std::vector<ColumnData<Y_type>> Y) {
	LeastSquaresFit<X_type, Y_type> lsf{X, Y};
	lsf.init();
	return lsf;
}
template LeastSquaresFit<float_data_type, float_data_type> makeLeastSquaresFit(std::vector<ColumnData<float_data_type>> X, std::vector<ColumnData<float_data_type>> Y);

template <typename X_type, typename Y_type>
bool LeastSquaresFit<X_type, Y_type>::calcNextProjection() {
	if(_cur_projection == _X.size()) { // indicates no update to projection was made
		return false;
	}
	float_data_type cur_SS_xx {0};
	float_data_type cur_SS_xy {0};
	
	const auto &X_cur_seg {_X.at(_cur_projection).data_raw};
	const auto &Y_cur_seg {_Y.at(_cur_projection).data_raw};

	// iterate through the appropriate segments of _X and _Y
	// calculate SS_xx and SS_yy of the new segment
	for(index_type i {0}; i < X_cur_seg.size(); ++i) {
		const auto &x_val {X_cur_seg.at(i)};
		const auto &y_val {Y_cur_seg.at(i)};
		cur_SS_xx += x_val * x_val;
		cur_SS_xy += x_val * y_val;
	}
	cur_SS_xx -= (X_cur_seg.size() * _x_bar * _x_bar);
	cur_SS_xy -= (Y_cur_seg.size() * _x_bar * _y_bar);

	// now add cur_SS_xx and cur_SS_xy to persistent
	// values, _SS_xx and _SS_xy
	_SS_xx += cur_SS_xx;
	_SS_xy += cur_SS_xy;

	// update values of _a, _b based on new sums of squares
	_b = _SS_xy / _SS_xx;
	_a = _y_bar - _b * _x_bar;

	++_cur_projection;
	return true;
}
template bool LeastSquaresFit<float_data_type, float_data_type>::calcNextProjection();
template bool LeastSquaresFit<int_data_type, int_data_type>::calcNextProjection();

template <typename X_type, typename Y_type>
float_data_type LeastSquaresFit<X_type, Y_type>::getProja(){ return _a; }
template float_data_type LeastSquaresFit<int_data_type, int_data_type>::getProja();
template float_data_type LeastSquaresFit<float_data_type, float_data_type>::getProja();

template <typename X_type, typename Y_type>
float_data_type LeastSquaresFit<X_type, Y_type>::getProjb(){ return _b; }
template float_data_type LeastSquaresFit<int_data_type, int_data_type>::getProjb();
template float_data_type LeastSquaresFit<float_data_type, float_data_type>::getProjb();

template <typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<X_type, Y_type> lsf) {
	std::cout 	<< "_x_bar: " << lsf._x_bar  << "\n"
				<< "_y_bar: " << lsf._y_bar << "\n"
				<< "_a: " << lsf._a << "\n"
				<< "_b: " << lsf._b << "\n"
				<< "_cur_projection: " << lsf._cur_projection << std::endl;
	return os;
}
template std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<int_data_type, int_data_type> lsf);
template std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<float_data_type, float_data_type> lsf);
