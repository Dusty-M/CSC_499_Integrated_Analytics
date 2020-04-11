#include <iostream>
#include "CSVParser.hpp"
#include "LeastSquaresFit.hpp"
#include <vector>
#include <math.h>
#include "gnuplot-iostream.h"
int main() {
Gnuplot gp;

std::vector<double> y_pts;
std::vector<double> x_pts;
std::vector<double> z_pts;
for(int i=0; i<1000; i+= 25) {
    double y = (i/500.0-1) * (i/500.0-1);
	x_pts.push_back(i);
    y_pts.push_back(y);
	z_pts.push_back(-y);
}

//gp << "set xrange [-5:1000]\nset yrange [-5:5]\n";

/********* Some samples for future reference *********/
//gp << "plot '-' with lines, sin(x/200) with lines\n";
//gp << "plot 'test.dat' with points\n";

gp << "plot '-' with points title 'point data', '-' with lines title 'line data'\n";
gp.send(std::make_tuple(x_pts, y_pts));
gp.send(std::make_tuple(x_pts, z_pts));

std::cout << "press enter to update plot" << std::endl;
std::cin.get();

for(auto &val : y_pts) {
    val = val + .1;
}

// although an update command exists for gnuplot it seems to rely on the data being
// stored in a file.  If the data is modified, update will scan the file and
// update the image.  In this case since the data is being sent straight from this program,
// replotting is the only way I can get an effective update.
gp << "plot '-' with points title 'point data', '-' with lines title 'line data'\n";
gp.send(std::make_tuple(x_pts, y_pts));
gp.send(std::make_tuple(x_pts, z_pts));

std::cout << "press enter to exit" << std::endl;
std::cin.get();
	return 0;
}
