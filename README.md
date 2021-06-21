# How to install on linux
1. install z3 and boost<br>
2. change BASEDIR in ./make.properties<br>
3. change m_prefix in ./include/parser/LogicParser.h<br>
4. change m_prefix in ./include/parser/TheoryParser.h<br>
5. $make tp<br>
6. get testparser in ./build<br>

# How to generate libslah.so
1. $g++ -std=c++11 -I./include/ -I./include/z3 -c ./src/api/slah_api.cpp ./src/solver_slah/PASolver.cpp ./src/solver_slah/Fts.cpp ./src/solver_slah/Relation.cpp ./src/component/Problem.cpp ./src/component/Predicate_SLAH.cpp ./src/component/Predicate.cpp ./src/solver_slah/HeapChunk.cpp -fPIC -lz3 -L./lib<br>
2. $g++ -std=c++11 -shared -o libslah.so slah_api.o PASolver.o Fts.o Relation.o Problem.o Predicate_SLAH.o Predicate.o HeapChunk.o<br>
# How to use libslah.so
1. move libslah.so to ./testAPI/lib<br>
2. $cd ./testAPI<br>
3. $g++ -std=c++11 test.cpp -o test.out -I./include/ -I./include/z3 -lslah -lz3 -L./lib<br>
4. $export LD_LIBRARY_PATH=./lib<br>
5. $./test.out<br>

#How to use it
For satisfiability problem, "The result: sat" means it is satisfiable.<br>
For entailment problem, "The result: unsat" means it is true.<br>