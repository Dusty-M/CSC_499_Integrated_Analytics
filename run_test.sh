rm bin/CSVParser_tester bin/unit_tests bin/profiler bin/make_plot
cmake -H. -Bbin -DATLAS_BUILD_EXAMPLES=OFF
cmake --build bin
bin/unit_tests

