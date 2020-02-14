#include "myLib.hpp"
#include <iostream>

int main( int argc, char **argv ) {
	if(argc < 3) {
		std::cout << "Error: too few args" << std::endl;
		return 1;
	}
	CSVParser t1{ argv[ 1 ], ','};
	ColumnData cd;
	try {
		cd = t1.getData(argv[2]);
	}
	catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
		return 2;
	}

	std::cout << "sum of " << cd.header << ": " << cd.data << std::endl;
	return 0;
}
