#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <mpi.h>

float genA (int row, int col) {
  if (row > col)
    return 1.;
  else
    return 0.;
}

float genx0 (int i) {
  return 1.;
}


void checkx (int iter, long i, float xval) {
  if (iter == 1) {
    float shouldbe = i;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99 )
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }

  if (iter == 2) {
    float shouldbe =(i-1)*i/2;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99)
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }
}

//perform dense y=Ax on an n \times n matrix
void matmul(float*A, float*x, float*y, long n) {
  for (long row = 0; row<n; ++row) {
    float sum = 0;
    
    for (long col = 0; col<n; ++col) {
      //sum += x[col] *A[row][col]
      sum += x[col] * A[row*n+col];
    }

    y[row] = sum;
  }
}

int main (int argc, char*argv[]) {

  if (argc < 3) {
    std::cout<<"usage: "<<argv[0]<<" <n> <iteration>"<<std::endl;
  }

  bool check = true;
  long n = atol(argv[1]);
  long iter = atol(argv[2]);
  
  //initialize data
  float* A = new float[n*n];

  for (long row = 0; row<n; row++) {
    for (long col=0; col<n; col++) {
      A[row*n+col] = genA(row, col);
    }
  }

  // for (long row = 0; row<n; row++) {
  //   for (long col=0; col<n; col++) {
  //     std::cout<<A[row*n+col]<<" ";
  //   }
  //   std::cout<<std::endl;
  // }

  float* x = new float[n];

  for (long i=0; i<n; ++i)
    x[i] = genx0(i);

  // for (long i=0; i<n; ++i)
  //   std::cout<<x[i]<<" ";
  // std::cout<<std::endl;
  
  float* y = new float[n];

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  // MPI start
  MPI_Init(&argc, &argv);
  int size_world, rank_world, size_split, rank_split, part_len;

  MPI_Comm_size(MPI_COMM_WORLD, &size_world);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank_world);

  part_len = sqrt(size_world);

  // split
  MPI_Comm splitcomm;
  int color = rank_world / part_len;
  int key = rank_world;

  MPI_Comm_split(MPI_COMM_WORLD, color, key, &splitcomm);
  MPI_Comm_size(splitcomm, &size_split);
  MPI_Comm_rank(splitcomm, &rank_split);

  // calculate
  float* arr_part = new float[part_len];
  float* arr_split = new float[part_len];

  for (int i = 0; i < part_len; i++){
    arr_part[i] = 1;
  }
  float* arr_final = new float[n];
  
  // for (int i = 0; i < n / part_len; i++) {
  //   int row = n * (i + (color * n / part_len));
  //   float sum = 0;
  //   for (int j = 0; j < n / part_len; j++){
  //     sum += A[row + j] * x[j + (rank_split * part_len)];
  //   }
  //   arr_part[i] = sum;
  // }
  //std::cout << "hi" << std::endl;
  // reduce
  MPI_Reduce(arr_part, arr_split, part_len, MPI_FLOAT, MPI_SUM, 0, splitcomm);
  //std::cout << "hi again" << std::endl;
  // broadcast  
  // MPI_Gather(&arr_split, part_len, MPI_FLOAT, 
  //           &arr_final, part_len, MPI_FLOAT,
  //           0, MPI_COMM_WORLD);
  
  //std::cout << "hi pussy" << std::endl;


  // print results
  if (rank_world == 0){
    std::cout << part_len << std::endl;
    for (int i = 0; i < part_len; i++)
      std::cout << arr_split[i] << " ";
    std::cout << std::endl;
    // for (int it = 0; it<iter; ++it) {
    //   //matmul(A, x, y, n);
    //   {
    //     float*t = x;
    //     x=y;
    //     y=t;
    //   }

    //   // std::cout<<"x["<<it+1<<"]: ";
    //   // for (long i=0; i<n; ++i)
    //   //   std::cout<<x[i]<<" ";
    //   // std::cout<<std::endl;

    //   if (check) for (long i = 0; i<n; ++i) checkx (it+1, i, x[i]);
    // }
  
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<<elapsed_seconds.count()<<std::endl;
  }

  MPI_Finalize();
  
  delete[] A;
  delete[] x;
  delete[] y;
  
  return 0;
}
