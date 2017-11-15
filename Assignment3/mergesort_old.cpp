#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

int nbthreads, n;
if (argc < 3) {
  std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
  return -1;
}

nbthreads = atoi(argv[2]);
n = atoi(argv[1]);
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

  generateMergeSortData (arr, atoi(argv[1]));

  //write code here
  for(int i=0; i<Math.ceil(log(n)); i++){
    cout<<Math.pow(2,i);
  }


  checkMergeSortResult (arr, atoi(argv[1]));

  delete[] arr;

  return 0;
}
