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
  int intensity, end, start, granularity;
  float (*f)(float, int);
  string sync;
};

pthread_mutex_t mut,mut2;
float sum = 0.0, blocks, block_counter = 0.0;

float evaluate_chunk(float (*f)(float, int), float a, float b, float n, float start, float end, int intensity);
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync, int granularity);
float evaluate(float (*f)(float, int), float a, float b,float n, float start, float end, int intensity);
void evaluate_iteration(float (*f)(float, int), float a, float b,float n, float start, float end, int intensity);
void* scheduler(void* context);

int main (int argc, char* argv[]) {

  if (argc < 6) {
    cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<endl;
    return -1;
  }

  float a, b, n;
  int id, intensity, nbthreads, granularity;
  string sync;  

  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  sync = argv[7];
  granularity = atoi(argv[8]);
  granularity = granularity>n?n:granularity;
 
  if(id==1)
    implement(f1,a,b,n,intensity,nbthreads,sync,granularity);
  else if(id==2)
    implement(f2,a,b,n,intensity,nbthreads,sync,granularity);
  else if(id==3)
    implement(f3,a,b,n,intensity,nbthreads,sync,granularity);
  else if(id==4)
    implement(f4,a,b,n,intensity,nbthreads,sync,granularity);
  else
    return 0;
}
float evaluate_chunk(float (*f)(float, int), float a, float b, float n, float start, float end, int intensity){
	float local_sum = 0.0;
	for(int i=start; i<end; i++){
	  local_sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	pthread_mutex_lock(&mut);
	sum+=local_sum;
	pthread_mutex_unlock(&mut);
}
float evaluate(float (*f)(float, int), float a, float b, float n, float start, float end, int intensity){
	float local_sum = 0.0;
	for(int i=start; i<end; i++){
	  local_sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	return local_sum;
}
void evaluate_iteration(float (*f)(float, int), float a, float b, float n, float start, float end, int intensity){
	for(int i=start; i<end; i++){
	  pthread_mutex_lock(&mut);
	  sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	  pthread_mutex_unlock(&mut);
	}
}
void* scheduler(void* context){
  	float a, b, n, start, end;
	int intensity, granularity, local_sum = 0.0;
	float (*f)(float, int);
	struct Parameters* params = (Parameters*) context;
	string sync;
	a = params->a;
        b = params->b;
        n = params->n;
	intensity = params->intensity;
	f = params->f;
	sync = params->sync;
	granularity = params->granularity;
	blocks = n/granularity;
	if(sync.compare("chunk") == 0){
	    while(block_counter<blocks){
	    	pthread_mutex_lock(&mut2);
		start = block_counter * granularity;
	    	end = start + granularity;
	   	end = (end>n)?n:end;
	    	start = (start>n)?n:start;
	    	block_counter++;
		pthread_mutex_unlock(&mut2);
	    	evaluate_chunk(f, a, b, n, start, end, intensity); 
	  }
	}
	else if(sync.compare("iteration") == 0){
   	    while(block_counter<blocks){
	    	pthread_mutex_lock(&mut2); 
	    	start = block_counter * granularity;
	    	end = start + granularity;  
	    	end = (end>n)?n:end;
	    	start = (start>n)?n:start;
	    	block_counter++;
	    	pthread_mutex_unlock(&mut2);
	    	evaluate_iteration(f, a, b, n, start, end, intensity);
	    }
	}
	else{
 	    float local_sum = 0.0;
	    while(block_counter<blocks){
	    	pthread_mutex_lock(&mut);
	    	start = block_counter * granularity;
	    	end = start + granularity;
	    	end = (end>n)?n:end;
	    	start = (start>n)?n:start;
	    	block_counter++;
	    	pthread_mutex_unlock(&mut);
	    	local_sum = local_sum + evaluate(f, a, b, n, start, end, intensity);
	  } 
	    pthread_mutex_lock(&mut);	
	    sum = sum + local_sum;
	    pthread_mutex_unlock(&mut); 
	}
}
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync, int granularity){
	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
	pthread_t threads[nbthreads];
	int i;
	Parameters* params = new Parameters[nbthreads];
	for(i=0; i<nbthreads; i++){
	  Parameters param;
	  param.a = a;
	  param.b = b;
	  param.n = n;
	  param.intensity = intensity;
	  param.f = f;
	  param.sync = sync;
	  param.granularity = granularity;
	  params[i] = param;
	  pthread_create(&threads[i], NULL, scheduler, &params[i]);
	}
  	
	for(i=0; i<nbthreads; i++)
		pthread_join(threads[i],NULL);
	cout<<sum <<endl;
                                                                                            
	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	cerr<<elapsed_seconds.count()<<endl;

}
