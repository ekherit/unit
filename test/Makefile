#all : benchmark example
all : example benchmark

CXXOPTS=-std=c++23 -fconcepts -I../include

benchmark: benchmark.cpp
	g++ $^ $(CXXOPTS) -Ofast -DUNIT    -o benchmark_unit_Ofast
	strip benchmark_unit_Ofast
	objdump -d benchmark_unit_Ofast > benchmark_unit_Ofast.asm 
	g++ $^ $(CXXOPTS) -Ofast -DDOUBLE  -o benchmark_double_Ofast 
	strip benchmark_double_Ofast
	objdump -d benchmark_double_Ofast > benchmark_double_Ofast.asm 
	g++ $^ $(CXXOPTS) -O3 -DUNIT    -o benchmark_unit_O3
	objdump -d benchmark_unit_O3 > benchmark_unit_O3.asm 
	g++ $^ $(CXXOPTS) -O3 -DDOUBLE  -o benchmark_double_O3 
	objdump -d benchmark_double_O3 > benchmark_double_O3.asm 
	g++ $^ $(CXXOPTS) -O2 -DUNIT    -o benchmark_unit_O2
	objdump -d benchmark_unit_O2 > benchmark_unit_O2.asm 
	g++ $^ $(CXXOPTS) -O2 -DDOUBLE  -o benchmark_double_O2 
	objdump -d benchmark_double_O2 > benchmark_double_O2.asm 
	g++ $^ $(CXXOPTS)  -DUNIT    -o benchmark_unit_unoptimized
	objdump -d benchmark_unit_unoptimized > benchmark_unit_unoptimized.asm 
	g++ $^ $(CXXOPTS)  -DDOUBLE  -o benchmark_double_unoptimized
	objdump -d benchmark_double_unoptimized > benchmark_double_unoptimized.asm 

example : example.cpp
	g++ $^ $(CXXOPTS) -O -o example

clean:
	rm -f *.asm benchmark_unit* benchmark_double* example
