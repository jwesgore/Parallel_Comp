#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <vector>
#include <array>


#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

void threadFunc(int i, int a, int intensity, float co, float (*ptr)(float, int),std::mutex& mu, float& result){
  //float temp = (*ptr)(vals[1] + (((float) i + .5) * co), vals[4]) * co;
  float temp =  (*ptr)(a + ((i + .5) * co), intensity);
  temp = temp * co;
  mu.lock();
  result += temp;
  mu.unlock();
}

#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  auto start = std::chrono::system_clock::now();
  std::mutex mu;
  std::vector<std::thread> threads;
  float result;

  if (argc < 7) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads>"<<std::endl;
    return -1;
  }

  std::array<float, 6> vals;

  for (int i = 0; i < 6; i++) {
    vals[i] = atoi(argv[i + 1]);
  }

  float co = (vals[2] - vals[1]) / vals[3];
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

  // loop creating threads
  for (int i = 0 ; i < vals[3]; i++) {
    //std::cout << i << std::endl;
    threads.push_back(
      std::thread(threadFunc, i, vals[1], vals[4], co, 
      (*ptr), std::ref(mu), std::ref(result)));
  }
  
  // join threads
  for (auto & t : threads) {
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