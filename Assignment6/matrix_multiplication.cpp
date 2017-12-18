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

#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <number of rows> "<<std::endl;
    return -1;
  }
	int p, n, **ranks, *leaders_arr, *flag, **matrix;
	double  *vect, *reduceVect, *vector_parts, *y,*newVect;
	int color, rank, size, row_rank, col_rank, row_size, col_size, matrix_size, rank_ctr, leader_ctr, leader_size, leader_rank;
  
	n = atoi(argv[1]);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
 
  	matrix_size = n/sqrt(size);
  	matrix = (int **) malloc(matrix_size*sizeof(int*));
	int dimension = sqrt(size);

	MPI_Comm row_comm;
	MPI_Comm col_comm;
	int row_color = rank/dimension;
	int col_color = rank%dimension;

	MPI_Comm_split(MPI_COMM_WORLD, row_color, rank, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, col_color, rank, &col_comm);

	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_size(row_comm, &row_size);
	MPI_Comm_rank(col_comm, &col_rank);
	MPI_Comm_size(col_comm, &col_size);
	
	for(int i=0;i<matrix_size;i++){
		matrix[i] = (int *) malloc(matrix_size*sizeof(int));
	}

	for(int i=0;i<matrix_size;i++){
		for(int j=0;j<matrix_size;j++){
			matrix[i][j] = rand()%10 + 1;
		}
		
	}
	
	vect = (double *) malloc(n*sizeof(double));
	if(rank == 0){
		
		for(int i=0;i<n;i++){
			vect[i] = rand()%10 + 1;
		}
	}
	int iterations = 0;
	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
	while(iterations<20){
		MPI_Bcast(vect, n, MPI_DOUBLE, 0, col_comm);
		vector_parts = (double *) malloc(matrix_size*sizeof(double));
		if(row_rank == 0){			
			int ctr = 0;
			reduceVect = (double *) malloc(matrix_size*sizeof(double));
			for(int i=matrix_size; i<n; i++){
				vector_parts[ctr++] = vect[i];
				if(ctr % matrix_size == 0){
					ctr=0;
					MPI_Send(vector_parts, matrix_size, MPI_DOUBLE, i/matrix_size, 0, row_comm);
				}
			}
			for(int i=0;i<matrix_size;i++)
				vector_parts[i] = vect[i];
		}
		else{
			MPI_Recv(vector_parts, matrix_size, MPI_DOUBLE, 0, 0, row_comm, MPI_STATUS_IGNORE);
		}
		y = (double *) malloc(matrix_size*sizeof(double));
		for(int i = 0; i<matrix_size;i++) 
		{
				y[i] = 0;
				for(int j = 0;j<matrix_size;j++)
				{
					y[i] += matrix[i][j]*vector_parts[j];
				}
		}

		MPI_Reduce(y, reduceVect, matrix_size, MPI_DOUBLE, MPI_SUM, 0, row_comm);

		if(row_rank == 0){
			if(rank == 0){
				MPI_Status status;
				int rootP = 0;
				newVect = (double *) malloc(n*sizeof(double));
				for(int i=0;i<matrix_size;i++)
					newVect[i] = reduceVect[i];
				while(rootP<dimension-1){
					MPI_Recv(reduceVect, matrix_size, MPI_DOUBLE,MPI_ANY_SOURCE,0,col_comm,&status);
					int currColRank = status.MPI_SOURCE;
					int offset = currColRank*matrix_size;
					for(int i=0;i<matrix_size;i++)
						newVect[i+offset] = reduceVect[i];
					rootP++;
				}
				
				for(int i=0;i<n;i++)
					vect[i] = newVect[i];
			}
			else{
				MPI_Send(reduceVect, matrix_size, MPI_DOUBLE, 0, 0, col_comm);
			}
			
		}
		iterations++;
	}
	if(rank==0){
		for(int i=0;i<n;i++)
			cout<<newVect[i]<<"\t";
		cout<<endl;
        chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end-start; 
        cerr<<endl<<elapsed_seconds.count()<<endl;
 
  }
	
	

	delete[] y;
	delete[] vect;
	delete[] vector_parts;
	MPI_Comm_free(&row_comm);
	MPI_Comm_free(&col_comm);
 	MPI_Finalize();
  
  return 0;
}
