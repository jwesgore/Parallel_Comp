#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"

#include <chrono>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

void merge(int start, int mid, int end, int arr[]){
  
  std::vector<int> left, right;
  std::cout<< start<< " " << mid << " " << end<< std::endl;

  // fill left and right vectors
  for (int i = start; i < mid + 1; i++)
    left.push_back(arr[i]);
  
  for (int i = mid; i < end; i++)
    right.push_back(arr[i+1]);
  
  std::cout << "   " << left[0] << " , " << right[0] << std::endl;

  // sort left and right vectors
  int m1 = 0, m2 = 0, point = start;
  while (m1 < left.size() && m2 < right.size()) {
    if (left[m1] < right[m2])
      arr[point++] = left[m1++];
    else
      arr[point++] = right[m2++];
  }

  while (m1 < left.size()) arr[point++] = left[m1++];
  while (m2 < right.size()) arr[point++] = right[m2++];
}

int main (int argc, char* argv[]) {
  
  // start timer
  auto start = std::chrono::system_clock::now();

  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  for (int i = 0; i < n; i++)
    std::cout<< arr[i] << " ";
  std::cout<<std::endl;

  //insert sorting code here.
  OmpLoop o1;
  int clump = 1;
  o1.setNbThread(atoi(argv[2]));

  while (clump <= n){
    
    clump = clump << 1;
    // std::cout << clump << std::endl;

    o1.parfor<int>(
      0, n-1, clump,
      [&](int & tls){},
      [&](int i, int & tls){
        
        int end;
        i + clump >= n ? end = n - 1 : end = i + clump -1;
        merge(i, (i + end) / 2, end, std::ref(arr));
      },
      [&](int & tls){}
    );
  }
  
  if (clump >> 1 != n) {
    //merge(0, (clump >> 1) + 1, n-1, std::ref(arr));
  }

  // if (clump >> 1 != n) {
  //   std::cout << "Clump is: " << clump << std::endl;
  //   std::cout << "N is: " << n << std::endl;

  //   int temp = 0;
  //   for (int i = 0; i < n; i++) {
  //     if (temp <= arr[i])
  //       temp = arr[i];
  //     else {
  //       std::cout << "Split point is: " << i <<std::endl;
  //       break;
  //     }
  //   }

  // }

  
  //std::vector<int> temp = merge(0, (n-1) / 2 ,n, arr);
  
  for (int i = 0; i < n; i++)
    std::cout<< arr[i] << " ";
  std::cout<<std::endl;

  checkMergeSortResult (arr, n);

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cerr << diff.count() << std::endl;
  
  delete[] arr;

  return 0;
}
