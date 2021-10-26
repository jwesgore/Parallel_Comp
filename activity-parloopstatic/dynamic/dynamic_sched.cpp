#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>
#include <mutex>
#include <vector>
#include <array>
#include "../sequential/seq_loop.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef float (*ptr) (float,int);
float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

ptr getFunction(int f) {
  switch (f) {
    case 1:
      return &f1;
    case 2:
      return &f2;
    case 3:
      return &f3;
    default:
      return &f4;
  }
} 

int main (int argc, char* argv[]) {

  // setup
  auto start = std::chrono::system_clock::now();
  std::mutex mu1, mu2;
  std::vector<std::thread> threads;
  int count = 0;
  float result = 0;

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <granularity>"<<std::endl;
    return -1;
  }

  // parse input
  std::array<float, 7> vals;

  for (int i = 0; i < vals.size(); i++) {
    vals[i] = atoi(argv[i + 1]);
  }

  // calculate coefficient
  float co = (vals[2] - vals[1]) / vals[3];

  // get function
  float (*ptr)(float, int) = getFunction((int) vals[0]);

  // threads
  for (int i = 0; i < vals[5]; i++) {
    threads.push_back(
      std::thread([&](){ 
        
        float temp = 0;
        while (count < vals[3]){
          
          // keep count of itteration
          mu1.lock();
          int local_count = count;
          count+= vals[6];
          mu1.unlock();

          // do computation
          for (int i  = local_count; i < local_count + vals[6] && i < vals[3]; i++){

            temp += (*ptr)(vals[1] + ((i + .5) * co), vals[4]) * co;
            
          }
        }

        // add results at the end
        mu2.lock();
        result += temp;
        mu2.unlock();
      })
    );
  }

  // join threads
  for (auto & t : threads){
    t.join();
  }

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;

  // print results
  std::cout << result << std::endl;
  std::cerr << diff.count() << std::endl;

  return 0;
}
