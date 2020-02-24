rm bin/CSVParser_tester bin/unit_tests
cmake -H. -Bbin 
cmake --build bin 
# bin/CSVParser_tester census_2000_sample_places_sample_dimensions.csv SE_T001_001 1 2
bin/unit_tests

