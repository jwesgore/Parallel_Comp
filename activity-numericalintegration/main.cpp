#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <limits>
#include <ctime>

using namespace std;

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

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  float vals[argc];
  auto pFunc = f1;

  for (int i = 0; i < 5; i++) {
    vals[i] = atoi(argv[i + 1]);
  }

  float co = (vals[2] - vals[1]) / vals[3];
  float result;

  switch ((int) vals[0]) {
    case 1:
      break;
    case 2:
      pFunc = f2;
      break;
    case 3:
      pFunc = f3;
      break;
    case 4:
      pFunc = f4;
      break;
  }

  for (int i = 0; i < vals[3]; i++) {
    float x = vals[1] + (((float) i + .5) * co);
    result += pFunc(x, vals[4]) * co;
  }

  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> diff = end - start;

  std::cout << result << std::endl;
  std::cerr << diff.count() << std::endl;

  return 0;
}