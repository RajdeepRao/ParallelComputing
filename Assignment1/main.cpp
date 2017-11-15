#include <iostream>
#include <ctime>
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
  
float evaluate(float (*f)(float, int), float a, float b,float n, int intensity);
int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  float a, b, n, sum;
  int id, intensity;
  
  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  sum = 0.0;  
  
  switch(id){
    case 1:
    	sum = evaluate(f1,a,b,n,intensity);
    break;

    case 2:
	sum = evaluate(f2,a,b,n,intensity);
    break;

    case 3:
	sum = evaluate(f3,a,b,n,intensity);
    break;

    case 4:
	sum = evaluate(f4,a,b,n,intensity);
    break;
    
    default:
	return 0;
    break;
   }
 
  std::cout<<sum;
  return 0;
}
float evaluate(float (*f)(float, int), float a, float b, float n, int intensity){
	float sum = 0.0;
	std::clock_t start;
        double duration;
	start = std::clock();
	for(int i=0; i<n; i++){
	  sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}

	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cerr<<duration;

	return sum;
}

