#include <iostream>
#include <ctime>
#include <string>
#include <math.h>

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
  int intensity, start;
  float (*f)(float, int);
};

pthread_mutex_t mut;
int value = 0;
float sum = 0.0;
void* printParams(void* i);
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync);
void *evaluate_iteration(void* context);
int main (int argc, char* argv[]) {

  if (argc < 6) {
    cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<endl;
    return -1;
  }

  float a, b, n, sum;
  int id, intensity, nbthreads;
  string sync;  

  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  sync = argv[7];
  implement(f1, a, b, n, intensity, nbthreads, sync);
  sum = 0.0;  
  
  /*switch(id){
    case 1:
    	implment(f1,a,b,n,intensity,nbthreads,sync);
    break;

    case 2:
	implment(f2,a,b,n,intensity,nbthreads,sync);
    break;

    case 3:
	implment(f3,a,b,n,intensity,nbthreads,sync);
    break;

    case 4:
	implment(f4,a,b,n,intensity,nbthreads,sync);
    break;
    
    default:
	return 0;
    break;
   }*/
 
  //cout<<sum;
  return 0;
}
void* evaluate_iteration(void* context){
	//float sum = 0.0;
	//clock_t start;
        //double duration;
	//start = clock();
	float a, b, n, sum;
	int intensity,start;
	float (*f)(float, int);
	struct Parameters* params = (Parameters*) context;
	a = params->a;
        b = params->b;
        n = params->n;
        start = params->start;
	intensity = params->intensity;
	f = params->f;
	//cout<< "start_thread: "<< start<< endl;
	for(int i=start; i<n; i++){
	  //cout<<i<<endl;
	  pthread_mutex_lock(&mut);
	    //sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	     sum++;
	     cout<<"Sum: "<< sum << endl;
	   pthread_mutex_unlock(&mut);

	}

	//duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
        //cerr<<duration;

	//return sum;
}
void* printParams(void* i){
  struct Parameters* temp = (Parameters*) i;
  cout<<"Received start: "<<temp->start<<" Received 'n': "<<temp->n<<endl;
  value++;
  
} 
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int nbthreads, string sync){
	pthread_t threads[nbthreads];
	float* ranges = new float[nbthreads];
	int i;
	for(i=0; i<nbthreads-1; i++)
	  ranges[i] = floor((i+1)*(n/nbthreads));
	ranges[i] = n;
	ranges[-1] = 0.0;
	/*struct Parameters param;
	param.a = a;
	param.b = b;
	param.n = n;
	param.intensity = intensity;
	pthread_t temp;
	pthread_create(&temp, NULL, printParams, &param);
	cout<<"sent"<<param.n<<endl;*/
	Parameters* params = new Parameters[nbthreads];
	for(i=0; i<nbthreads; i++){
	  Parameters param;
	  param.a = a;
	  param.b = b;
	  param.n = ranges[i];
	  param.start = ranges[i-1];
	  param.intensity = intensity;
	  param.f = f;
	  params[i] = param;
	  //cout<<"Start: "<<ranges[i-1];
	  pthread_create(&threads[i], NULL, evaluate_iteration, &params[i]);
	  if(sync.compare("iteration")==0){
	    //cout << "Equal" << endl;
	  }
	}
  	
	for(i=0; i<nbthreads; i++)
		pthread_join(threads[i],NULL);
	cout<<"Value of Sum is: "<< sum <<endl;
	//cout<<"Value of value is: "<< value <<endl;

}

