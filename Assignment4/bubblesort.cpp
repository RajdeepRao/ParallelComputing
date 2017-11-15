#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <omp.h>
#include <chrono>

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

  
#ifdef __cplusplus
}
#endif

int * temp;

void bubblesort(int arr[], int n, int granularity);
void print(int A[], int size);
void swap(int *x, int *y);
void pass(int arr[], int begin, int end);

int main (int argc, char* argv[]) {
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int granularity = atoi(argv[3]);
  if(granularity%2 != 0)
  	granularity-=1;
  if(granularity<4)
  	granularity=4;
  if(granularity>n)
  	granularity=n;
  
  omp_set_num_threads(nbthreads);
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
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <granularity>"<<std::endl;
    return -1;
  }

  int * arr = new int [n];
  temp = new int [n];

  generateMergeSortData (arr, n);

  for(int i = 0; i < n; i++)
  	temp[i] = arr[i];
  
  std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
  bubblesort(arr,n,granularity);
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-begin;    
  std::cerr<<elapsed_seconds.count()<<std::endl;
  checkMergeSortResult (arr, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}

void print(int A[], int size)
{
  int i;
  for (i=0; i < size; i++)
      cout<<A[i]<<"\t";
  cout<<endl;
}

void swap(int *x, int *y){
  *y = *x + *y;
  *x = *y - *x;
  *y = *y - *x;
}

void pass(int arr[], int begin, int end){
  for(int i=begin; i<end;i+=2){
    if(arr[i]>arr[i+1])
	swap(arr[i],arr[i+1]);
    }
}

void bubblesort(int arr[], int n, int granularity){
	
	for(int i = 0; i < ceil(n/2); i++){
		#pragma omp parallel
		{
			#pragma omp for schedule(dynamic,granularity)
			for(int i=0; i<n-1;i+=2){
				if(arr[i]>arr[i+1])
					swap(arr[i],arr[i+1]);
			}
			//#pragma omp barrier
			#pragma omp for schedule(dynamic, granularity)
			for(int i=1; i<n-1;i+=2){
				if(arr[i]>arr[i+1])
					swap(arr[i],arr[i+1]);
			}
			
		}
		
	}
}

