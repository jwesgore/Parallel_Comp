#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "omploop.hpp"

#include <chrono>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  // start timer
  auto start = std::chrono::system_clock::now();

  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int threads = atoi(argv[2]);
  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  int * pr = new int [n+1];

  //insert prefix sum code here
  OmpLoop o1;
  o1.setNbThread(threads);
  o1.parfor<int*>(
    0,threads,1,
    [&](int* &tls) {
      tls = new int[n];
    },
    [&](int i, int* &tls){
      
      int val1 = 0, point = i;
      while(point < n){
        val1 += arr[point];
        for (int i = point; point < i + threads; i++)
          tls[point] += val1;
      }
    },
    [&](int* &tls){
      for (int j = 0; j < n; j++) pr[j + 1] += tls[j];
    }
  );

  
  checkPrefixSumResult(pr, n);

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cerr << diff.count() << std::endl;

  delete[] arr;

  return 0;
}
