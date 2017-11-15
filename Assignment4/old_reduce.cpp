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

 int numOfThreads, n, granularity, sum_value=0;
 string scheduling;
 
 numOfThreads = atoi(argv[2]);
 n = atoi(argv[1]);
 
 int * arr = new int [atoi(argv[1])]; // Creating a new array
 omp_set_num_threads(n); 
 generateReduceData (arr, atoi(argv[1])); // Populating array with random values
 for(int i=0; i<n; i++)
   cout<<arr[i]<<endl;
 chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();    
 #pragma omp parallel
 {
  for(int i=0; i<n; i++){
    #pragma omp task firstprivate(sum_value)
    {
         sum_value += arr[i];
    }
  }
 }
 
 cout<<sum_value<<endl;
 
 chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
 chrono::duration<double> elapsed_seconds = end-start; 
 cerr<<elapsed_seconds.count()<<endl;
 delete[] arr;
 return 0;
}

