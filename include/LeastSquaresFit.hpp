// LeastSquaresFit.hpp
// Utility for calculating a least squares best fit line

#ifndef LEASTSQUARESFIT_HPP
#define LEASTSQUARESFIT_HPP
#include "CSVParser.hpp"

template <typename X_type, typename Y_type>
class LeastSquaresFit;

// Note: X_type and Y_type are expected to be std::vector<ColumnData<T>>
// where T is some numeric type such as float, double, unsigned int, etc.
template <typename X_type, typename Y_type>
class LeastSquaresFit {
public:
	template <typename T1, typename T2>
	friend std::ostream &operator<<(std::ostream &os, const LeastSquaresFit<T1, T2> lsf);

    LeastSquaresFit(X_type X, Y_type Y);
    void init();
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

#endif
