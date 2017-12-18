#include <iostream>
#include <mpi.h>
#include <stdlib.h>
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

double global_sum;

void implement(float (*f)(float, int), float a, float b,float n, int intensity, int rank, int world_size);
int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  float a, b, n, sum, nbthreads, granularity;
  int id, intensity;
  string scheduling;

  id = atoi(argv[1]);
  a = atoi(argv[2]);
  b = atoi(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  sum = 0.0;  
  //cout<<"I am "<<rank<<" out of"<<world_size<<endl;
  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
  if(id==1)
    implement(f1,a,b,n,intensity, rank, world_size);
  else if(id==2)
    implement(f2,a,b,n,intensity, rank, world_size);
  else if(id==3)
    implement(f3,a,b,n,intensity, rank, world_size);
  else if(id==4)
    implement(f4,a,b,n,intensity, rank, world_size);
  else
    return 0;
  
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();



  if(rank==0){
  	cout<<global_sum<<endl;
        chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end-start; 
        cerr<<endl<<elapsed_seconds.count()<<endl;
 
  }
  return 0;
}
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int rank, int world_size){
	int start,end;
	double sum = 0.0;
	start = n*rank/world_size;
	end = n*(rank+1)/world_size;
	if(rank == world_size-1)
		end = n;
	//cout<<"I am "<<rank<<" and My range is: "<<start<<" ---> "<<end<<endl;
	for(int i=start; i<end; i++){
		sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	//cout<<"I am "<<rank<<" sum= "<<sum<<endl;
	MPI_Reduce(&sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
}

