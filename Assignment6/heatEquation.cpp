#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <cstdlib>
#include <chrono>
#include <math.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

double **H, **Hprime;
int n, p, powers;
MPI_Request req_send_top, req_send_bottom, req_recv_top, req_recv_bottom;
MPI_Status *status_top, *status_bottom;
double *recv_top, *recv_bottom;

void fill_random_values();
void printMatrix();
void communicate(int rank);
void calculate();
void calculate(int rank, double *recv);
void calculate(int rank, double *recv_top, double *recv_bottom);

int main (int argc, char* argv[]) {

  if (argc < 3) {
    std::cerr<<"usage: mpirun "<<argv[0]<<"<Matrix Row Size> <No of Iterations>"<<std::endl;
    return -1;
  }
  
  int rank, world_size;
  
  MPI_Init(&argc, &argv);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  
  p = world_size;
  n = atoi(argv[1]);
  powers = atoi(argv[2]);
  H = new double*[n/world_size];
  Hprime = new double*[n/world_size];
   
  for(int i=0; i<n/world_size; i++){
	H[i] = new double[n];
  	Hprime[i] = new double[n];
  }
  srand(rank);
  fill_random_values();
  

  chrono::time_point<chrono::system_clock> start;
  //printMatrix();
  if(rank==0)
	start = chrono::system_clock::now();
  for(int k=0; k<powers; k++){
  	communicate(rank);
	MPI_Barrier(MPI_COMM_WORLD);
	for(int i=0; i<n/p; i++)
		for(int j=0; j<n; j++)
			H[i][j] = Hprime[i][j];
	//cout<<endl;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank==0){
 	chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
  	chrono::duration<double> elapsed_seconds = end-start; 
  	cerr<<endl<<elapsed_seconds.count()<<endl;
  }
  

  //printMatrix();
  //cout<<"I am "<<rank<<" out of"<<world_size<<endl;
  //cout<<"My Rank is: "<<rank<<endl;
  MPI_Finalize();
  for(int i=0; i<n/p; i++){
	 delete[] H[i];
  	 delete[] Hprime[i];
  }
  delete[] H;
  delete[] Hprime;
  delete[] recv_top;
  delete[] recv_bottom; 
  /*if(rank==0){
        chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end-start; 
        cerr<<endl<<elapsed_seconds.count()<<endl;
 
  }*/
  return 0;
}
void fill_random_values(){
	for(int i=0; i<n/p; i++){
		for(int j=0; j<n; j++){
			H[i][j] = (rand()%1000)*1.0;
		//	H[i][j]=1.0;
		}
	}
}
void printMatrix(){
	for(int i=0; i<n/p; i++){
		for(int j=0; j<n; j++){
			cout<<H[i][j]<<"\t";
		}
		cout<<endl;
	}
}

void communicate(int rank){
	if(rank==0){
		recv_bottom = new double[n];

		MPI_Isend(H[n/p-1], n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &req_send_bottom);
		MPI_Irecv(recv_bottom, n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &req_recv_bottom);
		
		calculate();
		MPI_Wait(&req_recv_bottom, status_bottom);
		calculate(rank, recv_bottom);

		/*cout<<"Printing BOTTOM values recv by "<<rank <<"from "<<rank+1<<endl;
		for(int i=0; i<n; i++)
			cout<<recv_bottom[i]<<"\t";
		cout<<endl;
		*/
	}
	else if(rank==p-1){
		recv_top = new double[n];

		MPI_Isend(H[0], n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_send_top);
		MPI_Irecv(recv_top, n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_recv_top);
		
		calculate();
		MPI_Wait(&req_recv_top, status_top);
		calculate(rank, recv_top);

		/*cout<<"Printing TOP values recv by "<<rank <<"from "<<rank-1<<endl;
		for(int i=0; i<n; i++)
			cout<<recv_top[i]<<"\t";
		cout<<endl;
		*/
	}
	else{
		recv_top = new double[n];
		recv_bottom = new double[n];
		
		MPI_Isend(H[n/p-1], n, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &req_send_bottom);
		MPI_Isend(H[0], n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_send_top);
		MPI_Irecv(recv_bottom, n, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &req_recv_bottom);
		MPI_Irecv(recv_top, n, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &req_recv_top);
		
		calculate();
		MPI_Wait(&req_recv_top, status_top);
		MPI_Wait(&req_recv_bottom, status_bottom);
		calculate(rank, recv_top, recv_bottom); //Can be made better by not waiting for both top and bottom. Top over -> Implement top something like that

		/*cout<<"Printing TOP values recv by "<<rank <<"from "<<rank-1<<endl;
		for(int i=0; i<n; i++)
			cout<<recv_top[i]<<"\t";
		cout<<endl;
		
		cout<<"Printing BOTTOM values recv by "<<rank <<"from "<<rank+1<<endl;
		for(int i=0; i<n; i++)
			cout<<recv_bottom[i]<<"\t";
		cout<<endl;
		*/
	
	}
}
void calculate(){
  for(int i=1; i<n/p-1; i++){
	for(int j=0; j<n; j++){
		if(j==0)
		  Hprime[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j])/6;
		else if(j==n-1)
		  Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/6;
		else
		  Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/9;
	}
  }
}
void calculate(int rank, double *recv){
	if(rank==0){
		int i=0;
		for(int j=0; j<n; j++){
			if(j==0)
				Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j])/4;
			else if(j==n-1)
				Hprime[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/4;
			else
				Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j])/6;
		
			//cout<<Hprime[i][j]<<"\t";
		}
		i=n/p-1;
		for(int j=0; j<n; j++){
			if(j==0)
		  		Hprime[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv[1]+recv[0]+H[i][j])/6;
			else if(j==n-1)
		  		Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+recv[n-1]+recv[n-2]+H[i][j-1]+H[i][j])/6;
			else
		  		Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv[j+1]+recv[j]+recv[j-1]+H[i][j-1]+H[i][j])/9;
		}
	}
	else if(rank==p-1){
		//Hprime[n/p]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i][j-1]+H[i][j]);
		int i=n/p-1;
		for(int j=0; j<n; j++){
			if(j==0)
				Hprime[i][j]=(H[i][j+1]+H[i-1][j+1]+H[i][j]+H[i-1][j])/4;
			else if(j==n-1)
				Hprime[i][j]=(H[i][j]+H[i][j-1]+H[i-1][j-1]+H[i-1][j])/4;
			else
		  		Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i][j-1]+H[i][j])/6;
		
			//cout<<Hprime[i][j]<<"\t";
		}
		i=0;
		for(int j=0; j<n; j++){
			if(j==0)
				Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j]+recv[0]+recv[1])/6;
			else if(j==n-1)
				Hprime[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv[n-1]+recv[n-2])/6;
			else
				Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv[j-1]+recv[j]+recv[j+1])/9;
		
		}
		  //Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]);
	}
	
	
}
void calculate(int rank, double *recv_top, double *recv_bottom){
	
	int i=n/p-1;
	for(int j=0; j<n; j++){
		if(j==0)
	  		Hprime[i][j]=(H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv_bottom[1]+recv_bottom[0]+H[i][j])/6;
		else if(j==n-1)
	  		Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+recv_bottom[n-1]+recv_bottom[n-2]+H[i][j-1]+H[i][j])/6;
		else
	  		Hprime[i][j]=(H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j+1]+recv_bottom[j+1]+recv_bottom[j]+recv_bottom[j-1]+H[i][j-1]+H[i][j])/9;
	}

	i=0;
	for(int j=0; j<n; j++){
		if(j==0)
			Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i][j]+recv_top[0]+recv_top[1])/6;
		else if(j==n-1)
			Hprime[0][j]=(H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv_top[n-1]+recv_top[n-2])/6;
		else
			Hprime[0][j]=(H[i][j+1]+H[i+1][j+1]+H[i+1][j]+H[i+1][j-1]+H[i][j-1]+H[i][j]+recv_top[j-1]+recv_top[j]+recv_top[j+1])/9;
	
	}

}
