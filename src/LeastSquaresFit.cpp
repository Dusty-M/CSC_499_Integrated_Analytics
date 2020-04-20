// LeastSquaresFit.cpp

#include "LeastSquaresFit.hpp"
#include <iostream>
#include <utility> //std::pair
#include <numeric> // std::inner_product
#include <chrono>
#include <memory>
#include <algorithm> // std::sort
using vec_col_float = std::vector<ColumnData<float_data_type>>;
using vec_col_int = std::vector<ColumnData<int_data_type>>;

template<typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const point<X_type, Y_type> &pt){
	os << "(" << pt.x << ", " << pt.y << ")";
	return os;
}
template std::ostream &operator<<(std::ostream &os, 
		const point<float_data_type, float_data_type> &pt);

template<typename X_type, typename Y_type>
std::ostream &operator<<(std::ostream &os, const summary<X_type, Y_type> &sum){
	os << "proj: " << sum.proj << "\n"
		<< "representative_pts: " << std::endl;
		for(auto val : sum.representative_pts) {
			os << "(" << val.x << ", " << val.y << "), ";
		}
		os << "SS_xx: " << sum.SS_xx << "\nSS_xy: " << sum.SS_xy << "\n"
		<< "x_bar: " << sum.x_bar << "\ny_bar: " << sum.y_bar << "\n"
		<< "count: " << sum.count << std::endl;
		return os;
}
template std::ostream &operator<<(
	std::ostream &os, const summary<float_data_type, float_data_type> &sum);

projection error( projection const p1, projection const p2) {
	return projection {	std::abs(p1.a_proj - p2.a_proj), 
						std::abs(p1.b_proj - p2.b_proj)};
}

std::ostream &operator << (std::ostream &s, projection const proj) 
{
	return s << "(" << proj.a_proj << ", " << proj.b_proj << ")";
}

// Provide a vector of points and a lambda function representing the project line of best fit
// and get_points returns a set of n points closest to dy
// Ie. if dy is set to -1.0, in points as close to a value of 1.0 below the line will be returned
template <typename Point_type>
std::vector<Point_type> get_points(const index_type n, const float_data_type dy, 
		const std::vector<Point_type> &all_points,
		const std::function<float_data_type(float_data_type)> &&f_x) {

	// testing code
	for(auto pt : all_points) {
		std::cout << "point: " << pt << ", error: " << (f_x(pt.x) - pt.y) << std::endl;
	}

	std::vector< Point_type > cur_rep_points {};

	// Find point with error that is closest to the value of dy
	auto iter {all_points.begin()};
	float_data_type closest_error {std::abs((f_x(iter->x)) - iter->y)};
	std::cout << "first element's error: " << closest_error << std::endl;
	iter++;
	while(iter != cur_rep_points.end()) {
		float_data_type cur_error{std::abs(f_x(iter->x) - iter->y)};
		std::cout << "cur_error :" << cur_error << std::endl;
		if (std::abs(cur_error - dy) < std::abs(closest_error - dy)) {
			closest_error = cur_error;
			iter++;
		} else { 
			// error must be equal or larger, therefore previous element contains min
			// remove element from all_points, add it to cur_rep_points
			// set up iterators on either side
			// recall: vector::erase() returns an iterator to the element following
			// the member being removed
			cur_rep_points.push_back(*(--iter));
			std::cout << "Choosing point with error: " << closest_error << std::endl;
			break;
		}
	} // end of setup
	// iter is now pointing to the closest point in all_points to the point with 
	// closest error to target error
	
	// testing - checking contents of cur_rep_points
	std::cout << "contents of cur_rep_points: " << cur_rep_points.at(0);
	std::cout << ", size: " << cur_rep_points.size() << std::endl;
	// two iterators will now be used:
	// - one iterator (iter_back) will start one element past/behind iter.  Each time an element
	// pointed to by this iterator is added to cur_rep_points, the iterator
	// will be incremented towards the end of the vector
	// - one iterator (iter_front) will start one element before/in front of iter.  
	// This will be a reverse iterator.  Each time and element pointed to by this iterator is 
	// added to cur_rep_points, the iterator will be incremented towards
	// the front of the vector (still a ++ since it's a reverse iterator)
	auto iter_back {iter + 1}; // iter_back now points to element after iter

	// offset = number to increment iter_front.  
	// iter_front begins by pointing at back-most element.  
	// It needs to be moved so that it points at the element in front of iter
	// iter - begin() is distance from front to iter
	// therefore size() - (iter - begin()) = distance from back element to target element
	auto offset = all_points.size() - (iter - all_points.begin());
	auto iter_front {all_points.rbegin()};
	iter_front += offset; // iter_front now points to element before iter
	std::cout << "is iter_front at rend()?: " << (iter_front == all_points.rend()) << std::endl;
	std::cout << "iter_front: " << *iter_front << ", iter_back" << *iter_back << std::endl;
	auto tmp_iter {all_points.rbegin()};

	// iter_front and iter_back are now pointing to elements on either side of iter
	while(cur_rep_points.size() < all_points.size() && cur_rep_points.size() < n) {
		// sanity check, this should never run, thus no exception is being thrown
		if(iter_front == all_points.rend() && iter_back == all_points.end()) {
			std::cout << "Error, trying to add representative points after all available points have been added already!" << std::endl;
			exit(1);
		}
		if(iter_front == all_points.rend()) { // out of points to left of target point
			// just add back point
			cur_rep_points.push_back(*iter_back++);
		} else if (iter_back == all_points.end()) { // out of points to right of target point
			cur_rep_points.push_back(*iter_front++);
		} else { // points exist on both sides, do comparison to choose which to add
			auto error_front {f_x(iter_front->x) - iter_front->y};
			auto error_back {f_x(iter_back->x) - iter_back->y};
			if(std::abs(error_front - dy) < std::abs(error_back - dy)) { // front points is closer
				cur_rep_points.push_back(*iter_front++);
			} else { // back put is closer
				cur_rep_points.push_back(*iter_back++);
			}
		}
	} // end of while loop, all points added to cur_rep_points
	return cur_rep_points;
}
template std::vector < point < float_data_type, float_data_type > > 
		get_points( const index_type n, const float_data_type dy,
		const std::vector < point < float_data_type, float_data_type > > &all_points,
		const std::function < float_data_type(float_data_type) > &&f_x);

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
//	float_data_type a_actual, b_actual;
	std::shared_ptr <projection> actual;

	{ // scoping lsf
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
//		a_actual = lsf.getProja();
//		b_actual = lsf.getProjb();

		actual = std::make_shared<projection> 
			(projection {lsf.getProja(), lsf.getProjb()});
	}

	// Setup for progressive calculation
	auto Xs {csv.makeSegments<X_type>(X_header, 
		header_row_index, first_data_row_index, num_segments)};
	auto Ys {csv.makeSegments<Y_type>(Y_header, 
		header_row_index, first_data_row_index, num_segments)};
	auto lsf {makeLeastSquaresFit<vec_cols_tX , vec_cols_tY>(Xs, Ys)};

	index_type cur_seg {0};
	auto start {std::chrono::system_clock::now()};
	while(lsf.calcNextProjection()) {
		projection cur_proj {lsf.getProja(), lsf.getProjb()};
		std::cout << "Error: " << error(*actual, cur_proj);
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
    	_X {X}, _Y {Y},_x_bar {0}, _y_bar {0},
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

template <typename X_type, typename Y_type> // X_type/Y_type are std::vector<Columndata<T>>
segment<typename X_type::value_type, typename Y_type::value_type> LeastSquaresFit<X_type, Y_type>::getNextSegment() 
{
	
	auto X = _X.at(_count);
	auto Y = _Y.at(_count);
	++_count;
	return segment {X, Y};
};
template segment <typename vec_col_float::value_type, typename vec_col_float::value_type >
	LeastSquaresFit<vec_col_float, vec_col_float>::getNextSegment();

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

template <typename X_type, typename Y_type>
summary<X_type, Y_type> create_summary(std::vector<X_type> X, std::vector<Y_type> Y)
{
	/***************************************
	return summary<X_type, Y_type {
		// projection
		// representative_pts
		// SS_xx
		// SS_xy
		// x_bar
		// y_bar
		// index_type count
	};
	***************************************/
	return summary<X_type, Y_type> {}; // temp, replace with real thing
}
template summary<float_data_type, float_data_type> create_summary
	(std::vector<float_data_type> X, std::vector<float_data_type> Y);

template <typename X_type, typename Y_type>
summary<X_type, Y_type> create_summary(
		summary<X_type, Y_type> s1, summary<X_type, Y_type> s2)
{

	std::cout << "printing summaries" << std::endl;
	std::cout << s1 << "\n\n" << s2 << std::endl;
	// calculate slope and y intercept
	float_data_type cur_SS_xx = s1.SS_xx + s2.SS_xx;
	float_data_type cur_SS_xy = s1.SS_xy + s2.SS_xy;
	float_data_type b = cur_SS_xy / cur_SS_xx;
	float_data_type a = s1.y_bar - b * s1.x_bar;
	projection cur_proj {a, b};

	std::cout << "projection: " << cur_proj << std::endl;
	// Calculate x_bar,y_bar
	float_data_type cur_x_bar = ((s1.x_bar * s1.count) + (s2.x_bar * s2.count)) 
		/ (s1.count + s2.count);
	float_data_type cur_y_bar = ((s1.y_bar * s1.count) + (s2.y_bar * s2.count)) 
		/ (s1.count + s2.count);

	// create lambda function to evaluate current projection,
	// useful for evaluating error
	auto f_proj {[&cur_proj](X_type x_i)
		{return cur_proj.a_proj + x_i * cur_proj.b_proj;}};

	// select representative points from {s1.representative_pts UNION s2.representative_pts}

	// sort points based on error. 
	// Note: when comparing a point to f(x) using the projection calculated
	// above, f(x_i) = a + b * x_i, this is contained in f_proj lambda
	// then error = f_proj(x_i) - y_i
	// 
	// NOTE: abs value NOT used here intentionally
	// By being sensitive to sign points can be gathered in equal
	// quantity from above/below the line.
	// calculate error for the ith representative point in each summary
	// and add pair of error,point for each rep. point in both s1 and s2

	// Add all representative points from both summaries to all_points
	std::vector < point< X_type, Y_type > > all_points;
		all_points.insert(	all_points.begin(), 
							s1.representative_pts.begin(), 
							s1.representative_pts.end());
		all_points.insert(	all_points.begin(), 
							s2.representative_pts.begin(), 
							s2.representative_pts.end());

	// lambda function used to compare points based on error
	auto compare {[&f_proj](point<X_type, Y_type> p1, point<X_type, Y_type> p2) {
		float_data_type error_p1, error_p2;
		error_p1 = std::abs(f_proj(p1.x) - p1.y);
		error_p2 = std::abs(f_proj(p2.x) - p2.y);
		return error_p1 < error_p2;}
	};

	/*****************************************************
	std::cout << "printing points before sorting" << std::endl;
	for(auto pt : all_points) {
		std::cout << pt << ", ";
	}
	std::cout << std::endl;
	*****************************************************/

	// sort points;
	std::sort(all_points.begin(), all_points.end(), compare);
	/*****************************************************
	std::cout << "printing points after sorting" << std::endl;
	for(auto pt : all_points) {
		std::cout << pt << ", ";
	}
	std::cout << std::endl;
	*****************************************************/

	// pick most representative points
	if(all_points.empty()) {
		std::cout << "ERROR, all_points is empty, "
			<< "neither summary contained rep points!" << std::endl;
	}

	// For now (temporary measure) representative points will consist of the 20
	// points closest to the line.  Later, this will be modified such that
	// there will be some number of points taken nearest the line, as well
	// as values located +/- 0.5 std dev from line, +/- 1.0 std dev, etc.
	const float_data_type TARGET_ERROR {0.0};

	std::vector < point <float_data_type, float_data_type > > cur_rep_points = 
		get_points(10, TARGET_ERROR, all_points, f_proj);

	return 
		summary<float_data_type, float_data_type> {
				cur_proj,				// projection 
				cur_rep_points,			// representative_pts
				s1.SS_xx + s2.SS_xx, 	// SS_xx
				s1.SS_xy + s2.SS_xy, 	// SS_xy
				cur_x_bar,				// x_bar
				cur_y_bar,				// y_bar
				s1.count + s2.count};	// count
/*
	std::cout << "A" << std::endl;
	std::vector< point< X_type, Y_type> > tmp_pts {
		point<X_type, Y_type>{1.0, 1.0},
		point<X_type, Y_type>{2.0, 3.0},
		point<X_type, Y_type>{4.0, 1.0}};

	std::cout << "B" << std::endl;

	summary<X_type, Y_type> tmp_sum{
		projection{2.0, 0.5}, // y = 2.0 + 0.5x
		tmp_pts,
		0.0, // SS_xx
		0.0, // SS_xy
		0.0, // x_bar
		0.0, // y_bar
		0};  // count
	tmp_sum.SS_xx = s1.SS_xx + s2.SS_xx;
	tmp_sum.SS_xy = s1.SS_xy + s2.SS_xy;
	tmp_sum.count = s1.count + s2.count;
	std::cout << "C" << std::endl;
	return tmp_sum;
*/
}
using float_summary_type = summary<float_data_type, float_data_type>;
template float_summary_type create_summary<float_data_type, float_data_type>
	(float_summary_type s1, float_summary_type s2);
