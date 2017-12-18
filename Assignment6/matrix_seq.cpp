#include <iostream>
#include <chrono>
#include <mpi.h>
#include <math.h>
#include <stdio.h>

int main (int argc, char* argv[]) {
 
  int N = atoi(argv[1]);
  int val = 0;
  //X is matrix  with Nx1
  int ** X  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	X[i] = new int[1];  
  }    
  //APrev is the matrix A with NxN size
  int **APrev  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	APrev[i] = new int[N];  
  }
  //c is final result of marrix product
  int ** c  = new int*[N];
  for (int i = 0; i < N ; ++i){		
	c[i] = new int[1];  
  } 
  //Initialise the matrix X with all 0's
  for (int i=0; i<N; i++) 
  {
	X[i][0]= 0;
  }

    for (int i=0; i<N; i++) 
	{	
	  
      for (int j=0; j<N; j++) 
	  { 
        APrev[i][j]= val++;
      }
    }
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	for(int iter = 0 ; iter<20; iter++)
	{	
		/* Matrix multiplication */
		//for (int k=0; k<N; k++)
		  for (int i=0; i<N; i++) {
			c[i][0] = 0.0;
			for (int j=0; j<N; j++){
			  c[i][0] = c[i][0] + APrev[i][j] * X[j][0];
			}
		  }

		std::cout<<"iteration "<<iter<<" is over"<<std::endl;
		for(int i = 0; i < N; ++ i)
		{
		  memcpy(&(X[i][0]), &(c[i][0]), 1 * sizeof(int));
		}	
	}
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;
	
	std::cerr<<elapsed_seconds.count()<<std::endl;		  
  }