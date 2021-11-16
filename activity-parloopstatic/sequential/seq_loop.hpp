#ifndef __SEQ_LOOP_H
#define __SEQ_LOOP_H

#include <functional>
#include <vector>
#include <thread>
#include <mutex>



class SeqLoop {

  int threads, granularity;
  bool dynamic;
public:
  SeqLoop()
    :threads(1), granularity(1), dynamic(false)
  {}

  void setNBThread(int i){
    threads = i;
  }

  void setGranularity(int i) {
    granularity = i;
  }

  void setDynamic(bool b) {
    dynamic = b;
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.

  template<typename TLS>
  void parfor (size_t beg, size_t end, size_t increment,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    TLS* tls = new TLS[threads];
    
    std::vector<std::thread> threadpool;
    
    // before
    for (int i = 0; i < threads;i++) 
      before(tls[i]);

    
    // dynamic
    if (dynamic) {
      
      int point = beg;
      std::mutex mu;

      for (size_t i = 0; i<threads; i++) {
        threadpool.push_back(std::thread(
        [&, i](){

          mu.lock();
          int local_point = point;          
          point += granularity;
          mu.unlock();

          while (local_point < end) {
            for (int j = local_point; j < (local_point + granularity) && j < end ;j+=increment)
              f(j, tls[i]);

            mu.lock();
            local_point = point;          
            point += granularity;
            mu.unlock();
          }

        }));
      }
      
    } 

    // static
    else {
      // determine static spacing
      std::vector<int> spacing;
      int gap = (end - beg) / threads;

      for(int i = beg; i < end;) {
        spacing.push_back(i);
        i += gap;
        if ((end - i) % threads != 0) i++;
      }
      spacing.push_back(end);
    
      // run loop
      for (size_t i = 0; i<threads; i++) {
        threadpool.push_back(std::thread(
        [&, i](){
          for (int j = spacing[i]; j < spacing[i+1]; j++)
            f(j, tls[i]);
        }));
      }
    }
    
    for (int i = 0; i < threads; i++){
      threadpool[i].join();
      after(tls[i]);
    }
    
  }
  
};

#endif
