#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void minion(int n) {
  cout << "Hello! I am minion " << n << endl;;
}

int main (int argc, char** argv) {
  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbminions>\n";
    return -1;
  }
  //Your code goes here
  int n = atoi(argv[1]);

  vector<thread> threads(n);
  int count = 0;

  for(auto & t : threads ) {
	  t = thread(minion, ++count);
  }

  for(auto & t : threads ) {
	  t.join();
  }
  
  cout << "Hello Minions! I am the Overlord!";

  return 0;
}
