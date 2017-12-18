#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <limits>

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
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int rank, int world_size, int granularity, int blocks);
void calculate(float(*f)(float, int), int start, int end, float a, float b, float n, int intensity, int rank);
int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
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
  granularity =  n/100;
  sum = 0.0;
  //cout<<"Granularity: "<<granularity<<endl;
  int blocks = (n/granularity);
  if(id==1)
    implement(f1,a,b,n,intensity, rank, world_size, granularity, blocks);
  else if(id==2)
    implement(f2,a,b,n,intensity, rank, world_size, granularity, blocks);
  else if(id==3)
    implement(f3,a,b,n,intensity, rank, world_size, granularity, blocks);
  else if(id==4)
    implement(f4,a,b,n,intensity, rank, world_size, granularity, blocks);
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
void implement(float (*f)(float, int), float a, float b,float n, int intensity, int rank, int world_size, int granularity, int blocks){
	int* ranges = new int[2];
	int i,min_val;
	bool flag = false;
	MPI_Status status;
	min_val = numeric_limits<int>::min();
	if(rank==0){
		int start=0,end=0;
		double local_sum=0.0;
		for(i=1; i<world_size; i++){
			start=granularity*(i-1);
			end = granularity*i;
			if(end>n)
				end=n;
			ranges[0]=start;
			ranges[1]=end;
			//cout<<"Sending to: "<<i<<"Start = "<<start<<endl;
			MPI_Send(ranges, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		while(i<=blocks){
			local_sum=min_val;
			MPI_Recv(&local_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			start=granularity*(i-1);
			end = granularity*i;
			if(end>n)
				end=n;
			ranges[0]=start;
			ranges[1]=end;
			if(local_sum!=min_val){
				global_sum += local_sum;
				//cout<<"Adding "<<local_sum<<endl;
				//cout<<"Sending to: "<<status.MPI_SOURCE<<" Start: "<<start<<endl;
				MPI_Send(ranges, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				i++;
			}
		}

		for(int j=1; j<world_size; j++){
			MPI_Recv(&local_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			global_sum += local_sum;
			ranges[0]=0;
			ranges[1]=0;
		//	cout<<"Final sending to ----------> "<<j<<endl;
			MPI_Send(ranges, 2, MPI_INT, j, 1, MPI_COMM_WORLD);
		}
	}
	else{
		while(true){
			MPI_Recv(ranges, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if(status.MPI_TAG == 1){
				//cout<<"Got the exit signal"<<endl;
				break;
			}
			if(ranges[0]>n || ranges[1]>n)
				break;
			calculate(f, ranges[0], ranges[1], a, b, n, intensity, rank);
		}
	}
	return;
}

void calculate(float(*f)(float, int), int start, int end, float a, float b, float n, int intensity, int rank){
	//cout<<"rank: "<<rank<<endl;
	double sum = 0.0;
	//cout<<"I am "<<rank<<" and My range is: "<<start<<" ---> "<<end<<endl;
	for(int i=start; i<end; i++){
		sum += f(a+(i+0.5)*(b-a)/n, intensity)*(b-a)/n;
	}
	//cout<<"I am Process no: "<<rank<<" My range is: "<<start<<" ----------> "<<end<<"      val = "<<sum<<endl;
	MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
}

