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


void MergeSort(int a[], int low, int high, int n);
void Merge(int a[], int low, int high, int mid, int n);

int * temp;


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
  temp = new int [atoi(argv[1])];

  generateMergeSortData (arr, atoi(argv[1]));

	for(int i = 0; i < n; i++)
		temp[i] = arr[i];
  
  //write code here
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  #pragma omp parallel
  {
      #pragma omp single
      {
         	MergeSort(arr,0,n-1,n);
       }
  }

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;    
  std::cerr<<elapsed_seconds.count()<<std::endl;
  for(int i=0; i<n-1; i++){
  	if(arr[i]>arr[i+1])
  		cout<<endl<<arr[i];
  }
  
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



void Merge(int a[], int low, int high, int mid, int n)
{
  // We have low to mid and mid+1 to high already sorted.
  int i, j, k;// temp[high-low+1];
  i = low;
  k = low;
  j = mid + 1;
  
  while (i <= mid && j <= high)
  {
    if (a[i] < a[j])
    {
      temp[k] = a[i];
      k++;
      i++;
    }
    else
    {
      temp[k] = a[j];
      k++;
      j++;
    }
  }
  
  // Insert all the remaining values from i to mid into temp[].
  while (i <= mid)
  {
    temp[k] = a[i];
    k++;
    i++;
  }
  
  // Insert all the remaining values from j to high into temp[].
  while (j <= high)
  {
    temp[k] = a[j];
    k++;
    j++;
  }
  
  // Assign sorted data stored in temp[] to a[].
  for (i = low; i <= high; i++)
  {
    a[i] = temp[i];
  }
}

void MergeSort(int a[], int low, int high, int n)
{
  int mid;
  if (low < high && (high - low)>10000)
  {
    mid=(low+high)/2;
    #pragma omp task
    MergeSort(a, low, mid, n);
    #pragma omp task
    MergeSort(a, mid+1, high, n);
    
    #pragma omp taskwait
    Merge(a, low, high, mid,n);
  
  }
  else if (low < high){
    mid=(low+high)/2;
    MergeSort(a, low, mid, n);
    MergeSort(a, mid+1, high, n);
    Merge(a, low, high, mid,n);
  }
}
