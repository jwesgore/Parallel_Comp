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

void printlcs_arr(std::vector<std::vector<int>> arr, int n, int m, char* x, char* y){
  for (int i = 0; i <= n; i++) {
    for (int j = 0; j <= m; j++) {
      //std::cout << y[i - 1] << x[j - 1] << " , ";
      std::cout << arr[i][j] << " , ";
    }
    std::cout << std::endl;
  }
}

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
  OmpLoop o1, o2;
  std::vector<std::vector<int>> lcs_arr;
  for (int i = 0; i <= n; i ++){
    std::vector<int> vector_row;
    for(int j = 0; j <= m; j++){
      vector_row.push_back(0);
    }
    lcs_arr.push_back(vector_row);
  }

  // parfor loop
  o2.setNbThread(atoi(argv[3]));
  //std::cout << "         " << X << " , " << Y << std::endl;
  for(int row_start = 0, col_start = 0; col_start <= m && row_start <= n; col_start++) {
    int diag = std::min(col_start + 1, n - row_start + 1);
    //std::cout << diag << std:: endl;
    //std::cout << row_start << " , " << col_start << std:: endl;
    //lcs_arr[row_start][col_start] = 1;
    o2.parfor<int*>(
      0, diag, 1,
      [&](int* & tls){
        tls = new int[diag];
      },
      [&](int i, int* & tls){
        int row_temp = row_start + i;
        int col_temp = col_start - i;
        //std::cout << "    " << row_temp << " , " << col_temp << std::endl;
        if (row_temp == 0 || col_temp == 0) {
          lcs_arr[row_temp][col_temp] = 0;
        } else if (X[col_temp-1] == Y[row_temp-1]) {
          //std::cout << "         " << X[col_temp-1] << " , " << Y[row_temp-1] << std::endl;
          lcs_arr[row_temp][col_temp] = lcs_arr[row_temp-1][col_temp-1] + 1;
        } else {
          //std::cout << "         " << X[col_temp-1] << " , " << Y[row_temp-1] << std::endl;
          lcs_arr[row_temp][col_temp] = std::max(lcs_arr[row_temp-1][col_temp], lcs_arr[row_temp][col_temp-1]);
        }
      },
      [&](int* & tls){
        
      }
    );

    if (col_start >= m){
      col_start = m - 1;
      row_start++;
    }
  }

  //printlcs_arr(std::ref(lcs_arr), n, m, X, Y);

  checkLCS(X, m, Y, n, lcs_arr[n][m]);

  // get runtime
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = end - start;
  std::cerr << diff.count() << std::endl;


  return 0;
}
