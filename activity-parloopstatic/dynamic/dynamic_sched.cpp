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

  // start timer
  auto start = std::chrono::system_clock::now();

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }

  // parse input
  std::array<float, 7> vals;
  std::mutex mu;
  SeqLoop s1;
  
  for (int i = 0; i < 7; i++) {
    vals[i] = atoi(argv[i + 1]);
  }

  double result = 0;
  
  int func = vals[0];
  int a = vals[1];
  int b = vals[2];
  int n = vals[3];
  int intensity = vals[4];
  int threads = vals[5];
  int granularity = vals[6];

  // calculate coefficient
  float co =  (b - a) / float (n);
  
  // iterations and threads
  int iterations = 0;
  std::vector<std::thread> tls_threads;

  // get function
  float (*ptr)(float, int) = getFunction(func);

  // parloop
  s1.parfor<double[100]>(
    0, threads, 1, 
    [&](double (&tls)[100]) -> void {
      
    },
    [&](int i, double (&tls)[100]) -> void { 
      tls_threads.push_back(std::thread(
        [&, i](){

          tls[i] = 0;

          mu.lock();
          int iteration = iterations;
          iterations += granularity;
          mu.unlock();

          while (iteration < n) {

            for (int j = iteration; j < iteration + granularity; j++) {
              if (iteration >= n) break;
              tls[i] += (*ptr)(a + ((j + .5) * co), intensity);
            }

            mu.lock();
            iteration = iterations;
            iterations += granularity;
            mu.unlock();
          }
          
        }
      ));
    },
    [&](double (&tls)[100]) -> void {
      for (int i = 0; i < threads; i++){
        tls_threads[i].join();
        result += tls[i];
      }
      result = result * co;
    }
  );

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;

  // print results
  std::cout << result << std::endl;
  std::cerr << diff.count() << std::endl;

  return 0;
}
