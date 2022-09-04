# BATED
BATch EDiting source code

Edit c++ source code by similar scripts:

doSomething.script:
moveP proc1 from file1.cpp to file2.cpp after proc2
..
moveP proc3 from file1.cpp to file2.cpp after proc1
moveD proc1 from file1.h to file2.h after proc2 // move declarations
..
moveD proc3 from file1.h to file2.h after proc1 
rename proc3 to NiceProcedure
