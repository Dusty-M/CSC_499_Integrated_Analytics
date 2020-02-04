rm -r bin/* 
cmake -H. -Bbin 
cmake --build bin 
bin/test_prog

