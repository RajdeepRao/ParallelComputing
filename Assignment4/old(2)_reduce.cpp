#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {

  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

 int numOfThreads, n, granularity, sum_value=0;
 string scheduling;
 
 numOfThreads = atoi(argv[2]);
 n = atoi(argv[1]);
 granularity = atoi(argv[4]);
 granularity = granularity>0 && granularity<=n ? granularity : (granularity<0?1: (granularity>n?n:granularity) );
 omp_set_num_threads(numOfThreads);
 if(scheduling.compare("static"))
      omp_set_schedule(omp_sched_static, granularity);
 else if(scheduling.compare("dynamic"))
      omp_set_schedule(omp_sched_dynamic, granularity); 
 else
      omp_set_schedule(omp_sched_guided, granularity);
  
 
 //forces openmp to create the threads beforehand
 #pragma omp parallel
 {
   int fd = open (argv[0], O_RDONLY);
   if (fd != -1) {
     close (fd);
   }
   else {
     std::cerr<<"something is amiss"<<std::endl;
   }
 }
 
 int * arr = new int [atoi(argv[1])]; // Creating a new array
 
 generateReduceData (arr, atoi(argv[1])); // Populating array with random values
 chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();    
 #pragma omp parallel for schedule(runtime) reduction(+:sum_value) 
   for(int i=0; i<n; i++){
       sum_value += arr[i];
   }
 cout<<sum_value<<endl;
 
 chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
 chrono::duration<double> elapsed_seconds = end-start; 
 cerr<<elapsed_seconds.count()<<endl;
 delete[] arr;
 return 0;
}

