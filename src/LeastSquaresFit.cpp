// LeastSquaresFit.cpp

#include "LeastSquaresFit.hpp"
#include <iostream>
#include <utility> //std::pair

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type>::LeastSquaresFit(ColumnData<X_type> X, ColumnData<Y_type> Y): 
    _x_bar {0}, _y_bar {0}, _X {X}, _Y {Y}, _SS_xx {0}, _SS_xy {0}, _a {0}, _b {0} {} 
template class LeastSquaresFit<int_data_type, int_data_type>; 
template class LeastSquaresFit<float_data_type, float_data_type>; 
 
template <typename X_type, typename Y_type> 
void LeastSquaresFit<X_type, Y_type>::init() { 
    _x_bar = calc_avg(_X); 
    _y_bar = calc_avg(_Y); 
}

template <typename X_type, typename Y_type> 
LeastSquaresFit<X_type, Y_type> makeLeastSquaresFit(
		ColumnData<X_type> X, ColumnData<Y_type> Y) {
	LeastSquaresFit<X_type, Y_type> lsf{X, Y};
	lsf.init();
	return lsf;
}
template LeastSquaresFit<float_data_type, float_data_type> makeLeastSquaresFit(ColumnData<float_data_type> X, ColumnData<float_data_type> Y);

template <typename X_type, typename Y_type>
void LeastSquaresFit<X_type, Y_type>::calcSquares() {
	for(index_type i {0}; i < _X.data_raw.size(); ++i) {
		_SS_xx += _X.data_raw.at(i) * _X.data_raw.at(i);
		_SS_xy += _X.data_raw.at(i) * _Y.data_raw.at(i);
	}

	std::cout << "_SS_xy: " << _SS_xy << std::endl;
	_SS_xx -= (_X.data_raw.size() * (_x_bar * _x_bar));
	_SS_xy -= (_Y.data_raw.size() * (_x_bar * _y_bar));
	std::cout << "_SS_xy: " << _SS_xy << std::endl;

	_b = _SS_xy / _SS_xx;
	_a = _y_bar - _b * _x_bar;
}
//template void calcSquares();

