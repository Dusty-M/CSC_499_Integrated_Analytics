rm bin/test_prog
cmake -H. -Bbin 
cmake --build bin 
bin/test_prog census_2000_sample_places_sample_dimensions.csv

