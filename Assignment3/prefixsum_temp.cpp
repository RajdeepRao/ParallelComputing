#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
  
  
#ifdef __cplusplus
}
#endif


int* calcPrefixSum(int* arr, int* pr, int n, int nbthreads);
int main (int argc, char* argv[]) {
  int nbthreads, n;
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }
  
  nbthreads = atoi(argv[2]);
  n = atoi(argv[1]);
  

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
  int * pr = new int [atoi(argv[1])+1];
  generatePrefixSumData (arr, atoi(argv[1]));
  omp_set_num_threads(nbthreads);
  for(int i=0; i<n; i++)
	cout<<"arr: "<<arr[i]<<endl; 
  //write code here
  int* ans = calcPrefixSum(arr, pr, n, nbthreads); 
  for(int i=0; i<n; i++)
  	cout<<"ans: "<<ans[i]<<endl; 

  checkPrefixSumResult(ans, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}
int* calcPrefixSum(int* arr, int* pr, int n, int nbthreads){
 int* blocks = new int[nbthreads];
 #pragma omp parallel
 {
	int threadNum = omp_get_thread_num();
	#pragma omp single
	{
		blocks = new int[nbthreads+1];
		blocks[0]=0;
	}
	cout<<"Thread Num: "<<threadNum<<endl;
	int sum=0;
	#pragma omp for schedule(static)
	  for(int i=1; i<n; i++){
		sum += arr[i];
		arr[i] = sum;
	  }
	blocks[threadNum+1] = sum;
  	for(int i=0; i<=nbthreads; i++)
		cout<<"Blocks["<<i<<"]: "<<blocks[i]<<endl;
	 #pragma omp barrier
	  int offset=0;
	  for(int i=0; i<=threadNum; i++){
		offset+=blocks[i];
	 	cout<<"Offset: "<<offset<<" Nbthreads: "<<nbthreads<<endl;
	  }
	 #pragma omp for schedule(static)
	 for(int i=1; i<n; i++){
		pr[i] += offset;
	  } 
	  

 }
	
 return pr;
}
