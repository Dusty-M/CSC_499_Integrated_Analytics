#include "myLib.hpp"
#include <iostream>

int main( int argc, char **argv ) {
	CSVParser t1{ argv[ 1 ], ','};
	t1.getData();
	return 0;
}
