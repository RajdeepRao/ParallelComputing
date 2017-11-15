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

void merge(int arr[], int left, int mid, int right,int temp[],int n)
{
  int k = left, i = left, j = mid+1;
  while(i<=mid && j<=right){
      if(arr[i] < arr[j])
          temp[k++] = arr[i++];
      else
          temp[k++] = arr[j++];

  }
  while(i <= mid){
      temp[k++] = arr[i++];
  }

  for(int i = left; i <= right; i++){
      arr[i] = temp[i];
  }
}

void mergeSort(int arr[], int n)
{
   int *temp;
   temp = (int*)malloc(n*sizeof(int));
   for(int i=0;i<n;i++)
       temp[i] = arr[i];
   for (int i=1; i<=n-1; i=2*i)
   {
       #pragma omp parallel for schedule(static)
       for (int ls=0; ls<n-1; ls+=2*i)
       {
           int mid = min(ls+i-1, n-1);
           int re = min(ls+2*i-1, n-1);
           merge(arr, ls, mid, re, temp, n);
       }
   }
}

int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
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

  int * arr = new int [atoi(argv[1])];
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  generateMergeSortData (arr, atoi(argv[1]));

  mergeSort(arr,n);
  //write code here
  //printArray(arr,n);
  checkMergeSortResult (arr, atoi(argv[1]));
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<<elapsed_seconds.count()<<std::endl;


  delete[] arr;

  return 0;
}
