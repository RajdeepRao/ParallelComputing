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

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float evaluate_dynamic(float (*f)(float, int), float a, float b, float n, int intensity, int granularity);
int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  float a, b, n, sum, nbthreads, granularity;
  int id, intensity;
  string scheduling;

  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  scheduling = argv[7];
  granularity = atoi(argv[8]);
  sum = 0.0;  
 
  omp_set_num_threads(nbthreads);
  //forces openmp to create the threads beforehand

  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
  
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
  
   switch(id){
	    case 1:
		sum = evaluate_dynamic(f1,a,b,n,intensity,granularity);
	    break;

	    case 2:
		sum = evaluate_dynamic(f2,a,b,n,intensity,granularity);
	    break;

	    case 3:
		sum = evaluate_dynamic(f3,a,b,n,intensity,granularity);
	    break;

	    case 4:
		sum = evaluate_dynamic(f4,a,b,n,intensity,granularity);
	    break;
	    
	    default:
		return 0;
	    break;
   }
 
  cout<<sum;
  
  chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds = end-start; 
  cerr<<endl<<elapsed_seconds.count()<<endl;
 
  return 0;
}

float evaluate_dynamic(float (*f)(float, int), float a, float b, float n, int intensity, int granularity){
	float sum = 0.0;
 	#pragma omp parallel for schedule(dynamic, granularity) reduction(+:sum) 
	for(int i=0; i<(int)n; i++){
	 sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	return sum;
}

