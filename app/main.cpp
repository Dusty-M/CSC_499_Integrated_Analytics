#include "myLib.hpp"
#include <iostream>

void usage() {
	std::cout << "****************************************************" << std::endl;
	std::cout << "Error: too few args.  Proper usage:\n"
		<< "./test_prog <CSV filename> <row index>\n"
		<< "example: test_prog some_data.csv SE_T001_001\n"
		<< "NOTE: <row index> must be located on 2nd row of data" << std::endl;
	std::cout << "****************************************************" << std::endl;
}

int main( int argc, char **argv ) {
	if(argc < 3) {
		usage();
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
