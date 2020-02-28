// LeastSquaresFit.hpp
// Utility for calculating a least squares best fit line

#ifndef LEASTSQUARESFIT_HPP
#define LEASTSQUARESFIT_HPP
#include "CSVParser.hpp"

template <typename X_type, typename Y_type>
class LeastSquaresFit {
public:
    LeastSquaresFit(ColumnData<X_type> X, ColumnData<Y_type> Y);
    void init();
	void calcSquares();

    ColumnData<X_type> _X;
    ColumnData<Y_type> _Y;

    // Values specific to the least squares fitting calculation
    // See the following link for a description of variables used:
    // http://mathworld.wolfram.com/LeastSquaresFitting.html
    float_data_type   _x_bar,
                    _y_bar,
                    _SS_xx,
                    _SS_xy,
                    _a,
                    _b;
private:
};

template <typename X_type, typename Y_type>
LeastSquaresFit<X_type, Y_type> makeLeastSquaresFit(ColumnData<X_type> X, ColumnData<Y_type> Y);

#endif
