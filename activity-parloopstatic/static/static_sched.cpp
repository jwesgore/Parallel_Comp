#include <iostream>
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

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {

  auto start = std::chrono::system_clock::now();
  float result;
  SeqLoop s1;

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }

  // parse input
  std::array<float, 6> vals;

  for (int i = 0; i < 6; i++) {
    vals[i] = atoi(argv[i + 1]);
  }

  float co = (vals[2] - vals[1]) / vals[3];
  
  // get function
  float (*ptr)(float, int);
  switch ((int) vals[0]) {
    case 1:
      ptr = &f1;
      break;
    case 2:
      ptr = &f2;
      break;
    case 3:
      ptr = &f3;
      break;
    case 4:
      ptr = &f4;
      break;
  }

  // parloop
  s1.parfor(0, vals[3], 1, 
  [&](int i) -> void{
    result += (*ptr)(vals[1] + ((i + .5) * co), vals[4]) * co;
    });

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;

  // print results
  std::cout << result << std::endl;
  std::cerr << diff.count() << std::endl;

  return 0;
}