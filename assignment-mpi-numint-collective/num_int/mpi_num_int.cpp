#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

// method to parse function 
typedef float (*ptr) (float,int);
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

#ifdef __cplusplus
}
#endif

  
int main (int argc, char* argv[]) {
  
  // start timer
  auto start = std::chrono::system_clock::now();

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int func = atoi(argv[1]);
  int a = atoi(argv[2]);
  int b = atoi(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  float result = 0; // init result
  float (*ptr)(float, int) = getFunction(func); // get function
  float co =  (b - a) / float (n); // calculate coefficient

  // MPI start
  MPI_Init(&argc, &argv);
  int size, rank;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int loop_start = rank * (n / size);
  int loop_end = (rank + 1) * (n / size);
  double rank_val = 0;

  if (loop_start + 1 == rank) loop_end = n;

  for (int i = loop_start; i < loop_end; i++) {
    rank_val += (*ptr)(a + ((i + .5) * co), intensity);
  }

  result += rank_val;
  std::cout << rank << " , " << result << std::endl;
  // if (rank == 0){
  //   result *= co;

  //   // get runtime
  //   auto end = std::chrono::system_clock::now();
  //   std::chrono::duration<double> diff = end - start;

  //   // print results
  //   std::cout << result << std::endl;
  //   std::cerr << diff.count() << std::endl;
  // }

  // MPI end
  MPI_Finalize();

  return 0;
}
