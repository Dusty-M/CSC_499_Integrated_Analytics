rm bin/CSVParser_tester bin/unit_tests bin/profiler
cmake -H. -Bbin
cmake --build bin
bin/unit_tests

