# unit
Strong typed library for working with values of arbitrary dimensions

## features
- Tested only with g++-12 -std=c++23
- Use "__\_\_PRETTY_FUNCTION\_\___" for comptile time type identification
- Type erasure when optimization turned on
- Identical to "double" type calculation asm  code with -Ofast optimization

## Usage
- Header only library
## Example
- Declare new dimension names
  ```c++
   struct Length {};
   struct Time {};
  ```
- Define length and time etalon
- ```c++
  auto meter = ibn::make_unit<Length>();
  auto second = ibn::make_unit<Time>();
  ```
- Define velocity 60 km/hour
  ```c++
  auto v = 60*(1000*meter)/(3600*second);
  ```
- Do some calculations
  ```
  auto g = 9.80665*meter/second/second;
  auto h = v*v/2/g;
  ```
- Print value
  ```c++
  std::cout << "For v = " << v/(meter/second) << " m/s flight height is "  << h/meter << " m\n";
  ```
- for more detail see
  ```
  test/example.cpp
  ```
## Benchmark
- go to test directory and do make  to compile 
 ```
   test/benchmark.cpp
 ```
- When using -Ofast optimization the code is identical to calculation with double type
  ```
   test/benchmark_double_Ofast
   test/benchmark_unit_Ofast
  ```

 
