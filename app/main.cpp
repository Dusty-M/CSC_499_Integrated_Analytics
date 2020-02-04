#include "myLib.hpp"
#include <iostream>

int main() {
	CSVParser t1{"census_2000_sample_places_sample_dimensions.csv", ','};
	t1.getData();
	return 0;
}
