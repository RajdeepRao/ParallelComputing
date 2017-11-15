#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include <chrono>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif


void reduce(int arr[], int begin, int end, int thArr[]);
void driver(int arr[],int n, int threadArr[], int nbthreads,int granularity);

int main (int argc, char* argv[]) {
  
  if (argc < 4) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <granularity>"<<std::endl;
    return -1;
  }
  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int granularity = atoi(argv[3]);
  if(granularity < 0)
  	granularity = 1;
  if(granularity > n)
  	granularity = n;
  omp_set_num_threads(nbthreads);
  int sum = 0;
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
  
  int * arr = new int [n];

  generateReduceData (arr, atoi(argv[1]));
  int threadArr[nbthreads];

  for(int i=0;i<nbthreads;i++)
  	threadArr[i] = 0;

  std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
  #pragma omp parallel
  {
    #pragma omp single
    {
      driver(arr,n,threadArr,nbthreads,granularity);
    }
  }
  
  for(int i=0; i<nbthreads;i++){
  	sum+=threadArr[i];
  }
  cout<<sum<<endl;
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-begin;    
  std::cerr<<elapsed_seconds.count()<<std::endl;
  delete[] arr;

  return 0;
}
void reduce(int arr[], int begin, int end, int thArr[]){
	int threadNum = omp_get_thread_num();
	
	int localSum = 0;
	for(int i = begin; i < end; i++)
		localSum+=arr[i];
	
	thArr[threadNum] += localSum;
}
void driver(int arr[],int n, int threadArr[], int nbthreads,int granularity){
	  
	for(int i=0; i<n; i=i+granularity){
		int begin = i;
		int end = i+granularity;
		if(end > n)
			end = n;
		#pragma omp task
			reduce(arr, begin, end, threadArr);
	}
	#pragma omp taskwait
}
