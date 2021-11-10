#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {

  // start timer
  auto start = std::chrono::system_clock::now();

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  const int m = atoi(argv[1]);
  const int n = atoi(argv[2]);  

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);

  //insert LCS code here.
  int result = -1; // length of common subsequence

  auto 

  for (int row = 1; row <= m ; row++) {
    for (int col = 1; col <= n ; col++) {
      dynamic[row][col] = -1;
    }
  }

  OmpLoop o1;
  std::condition_variable cv;
  std::mutex mu1, mu2;
  
  // parfor loop
  o1.setNbThread(atoi(argv[3]));
  o1.parfor<int>(
    0, m + n + 1, 1,
    [&](int & tls){
      
    },
    [&](int i, int & tls){

      // get diag length
      int diag = 0;
      int val, row, col;
      std::unique_lock<std::mutex> lock(mu1);

      if (i < m) {
        diag = i;
      } else if (i >= m && i <= n) {
        diag = m;
      } else {
        diag = (m + n) - i;
      }

      for (int j = 0; j <= diag; j++) {
        i >= n ? (row = (i - n) + j, col = n - j) : (row = j, col = i - j);

        if (row == 0 || col == 0) continue;
        X[row - 1] == Y[col - 1] ? val = 1 : val = 0;
        
        cv.wait(lock, [&](){
          return !(dynamic[row-1][col-1] < 0 || dynamic[row][col-1] < 0);
        });

        val ? val = dynamic[row - 1][col - 1] + 1 :
          val = std::max(dynamic[row - 1][col], dynamic[row][col - 1]);
        
        dynamic[row][col] = val;
        cv.notify_one();
      }
    },
    [&](int & tls){
      
    }
  );

  result = dynamic[m][n];
  checkLCS(X, m, Y, n, result);

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cerr << diff.count() << std::endl;


  return 0;
}
