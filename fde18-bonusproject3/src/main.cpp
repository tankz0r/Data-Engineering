#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <future>
#include "DistCalculator.hpp"

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   using namespace std;
   if (argc != 2) {
      cout << "Usage: " << argv[0] << " <playedin.csv>";
      exit(1);
   }

   string playedinFile(argv[1]);
   std::vector<std::future<int64_t>> vec;
   // Create dist calculator
   DistCalculator dc(playedinFile);

   // read queries from standard in and return distances
   DistCalculator::Node a, b;
//   while (cin >> a && cin >> b) vec.push_back(std::async(std::launch::async, &DistCalculator::dist, &dc, a, b));
//
//   for (auto& result : vec) {
//      std::cout << result.get() << std::endl;
//   }

    while (cin >> a && cin >> b)
       cout << dc.dist(a, b) << "\n";

   // flush output buffer
   cout.flush();
}
//---------------------------------------------------------------------------
