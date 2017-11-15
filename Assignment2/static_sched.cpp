#include <iostream>
#include <ctime>
#include <string>
#include <math.h>
#include <chrono>
#include <ctime>

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
  
struct Parameters{
  float a,b,n;
  int intensity, end, start;
  float (*f)(float, int);
  string sync;
};

pthread_mutex_t mut;
float sum = 0.0;

void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync);
void *evaluate(void* context);

int main (int argc, char* argv[]) {

  if (argc < 6) {
    cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<endl;
    return -1;
  }

  float a, b, n;
  int id, intensity, nbthreads;
  string sync;  

  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  sync = argv[7];
  
  if(id==1)
    implement(f1,a,b,n,intensity,nbthreads,sync);
  else if(id==2)
    implement(f2,a,b,n,intensity,nbthreads,sync);
  else if(id==3)
    implement(f3,a,b,n,intensity,nbthreads,sync);
  else if(id==4)
    implement(f4,a,b,n,intensity,nbthreads,sync);
  else
    return 0;
}

void* evaluate(void* context){
	float a, b, n;
	int intensity,start, end;
	float (*f)(float, int);
	struct Parameters* params = (Parameters*) context;
	string sync;
	a = params->a;
        b = params->b;
        n = params->n;
        start = params->start;
	end = params->end;
	intensity = params->intensity;
	f = params->f;
	sync = params->sync;
	if(sync.compare("iteration")==0){
	  for(int i=start; i<end; i++){
	    pthread_mutex_lock(&mut);
	    sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	    pthread_mutex_unlock(&mut);
	  }
	}else{
	  float local_sum = 0.0;
	  for(int i=start; i<end; i++){
	    local_sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	  }
	    pthread_mutex_lock(&mut);
	      sum+=local_sum;
	    pthread_mutex_unlock(&mut);
	}

}

void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync){
	pthread_t threads[nbthreads];
	float* ranges = new float[nbthreads];
	int i;
	for(i=0; i<nbthreads-1; i++)
	  ranges[i] = floor((i+1)*(n/nbthreads));
	ranges[i] = n;
	ranges[-1] = 0.0;
	Parameters* params = new Parameters[nbthreads];
	std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
	for(i=0; i<nbthreads; i++){
	  Parameters param;
	  param.a = a;
	  param.b = b;
	  param.n = n;
	  param.start = ranges[i-1];
	  param.intensity = intensity;
	  param.f = f;
	  param.end = ranges[i];
	  param.sync = sync;
	  params[i] = param;
	  pthread_create(&threads[i], NULL, evaluate, &params[i]);
	}
  	
	for(i=0; i<nbthreads; i++)
		pthread_join(threads[i],NULL);
	cout<< sum <<endl;
	 
	std::chrono::time_point<std::chrono::system_clock> finish = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = finish-begin;
 	std::cerr<<elapsed_seconds.count()<<std::endl;
}

