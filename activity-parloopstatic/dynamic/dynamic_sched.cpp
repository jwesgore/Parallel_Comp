#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  float co =  (b - a) / float (n); // calculate coefficient
  float (*ptr)(float, int) = getFunction(func); // get function

  // parloop
  s1.setNBThread(threads);
  s1.setGranularity(granularity);
  s1.setDynamic(true);
  s1.parfor<double>(
    0, n, 1, 
    [&](double (&tls)) -> void {
      tls = 0.0;
    },
    [&](int i, double (&tls)) -> void { 
      tls += (*ptr)(a + ((i + .5) * co), intensity);
    },
    [&](double (&tls)) -> void {
      result += tls;
    }
  );

  result = result * co;

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;

  // print results
  std::cout << result << std::endl;
  std::cerr << diff.count() << std::endl;

  return 0;
}
