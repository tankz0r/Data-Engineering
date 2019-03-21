# FDE Bonus Projects

## Task 1

Implement the avg function and constructor in src/JoinQuery.cpp. See the corresponding header include/JoinQuery.hpp 
for documentation of this function. All solutions that compute correct results 
and achieve a time measurement better than 15 seconds on the leaderboard will be accepted for 
the bonus.

The performance evaluation will run the binary compiled from main.cpp so that the avg query is run
for multiple market segments on a TPC-H dataset with scale factor 1. The overall runtime from start to end is measured.
Details about the machine this will be executed on: Intel(R) Core(TM) i7-3930K CPU @ 3.20GHz with 6 cores, 12 hyperthreads and 64GB of memory.

You can use the test provided in test/JoinQuery.cpp to check if your implementation works
correctly.

When you do performance tuning, you may use and extend the benchmark suite in bench/JoinQuery.cpp.

## Build
A configuration file is provided to build this project with CMake. This allows you to build the project
in the terminal but also provides the option to use Jetbrains CLion or Microsoft Visual Studio and other
IDEs.

Building from Terminal:
Start in the project directory.
```
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```

This creates the binaries test_all, bench and main.

## Solution
**Runtime(ms):** 6567
- Figuring out where a program spends time requires profiling: `perf record` && `perf report` && `perf stat -d`
- ussage of `mmap` which gives a possibility to avoid copying the data by mapping the file into memory
```C++
int handle=open(argv[1],O RDONLY);
lseek(handle,0,SEEK END);
auto size=lseek(handle,0,SEEK CUR);
auto
data=mmap(nullptr,size,PROT READONLY,MAP SHARED,handle,0);
```
- blockwise terminator search
```C++
uint64 t block=*reinterpret cast<const uint64 t*>(iter);
constexpr uint64 t high=0x8080808080808080ull;
constexpr uint64 t low=0x7F7F7F7F7F7F7F7Full;
constexpr uint64 t pattern=0x0A0A0A0A0A0A0A0Aull;
uint64 t lowChars=( ̃block)&high;
uint64 t found0A= ̃((((block&low)ˆpattern)+low)&high);
uint64 t matches=matches0A&lowChars;
```
- parallelism
``` C++
unsigned chunks=thread::hardware concurrency();
vector<thread>threads;
for (unsigned index=1;index<chunks;++index)
threads.push back(thread(sumChunk(index));
sumChunk(0);
```
