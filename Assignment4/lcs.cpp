#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <omp.h>
#include <chrono>

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

  
#ifdef __cplusplus
}
#endif


static const char alphabets[] = "abcdefghijklmnopqrstuvwxyz";

void get_random_string(char* a,int length);
void generateLCS(char* s1,char* s2, int m, int n ,int** L);
void get_random_string(char* a,int length);
void getLCS(int m, int n, int** L, char* s1, char*s2);

int main (int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr<<"Usage: "<<argv[0]<<"<nbthreads> <string 1 size> <string 2 size>"<<std::endl;
    return -1;
  } 
  
  int nbthreads = atoi(argv[1]);
  int m = atoi(argv[2]);
  int n = atoi(argv[3]);
  int temp;
  if(m<n){//Making sure the matrix is long vertically
    temp = m;
    m = n;
    n = temp;
  }
  
 
  omp_set_num_threads(nbthreads);
  int** L;

  //forces openmp to create the threads beforehand
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
  
  char* s1 = new char[m];
  char* s2 = new char[n];
 
	
  //write code here
  get_random_string(s1,m);
  get_random_string(s2,n);
  L = (int**) malloc(sizeof(int*) * (m + 1));
  for(int i = 0; i <= m; i++) {
  	L[i] = (int*) malloc(sizeof(int) * (n + 1));
  }
  //Set first row and col = 0
  #pragma omp parallel for
  for(int i=0;i<=m;i++){
  	L[i][0] = 0;
  }
  
  #pragma omp parallel for
  for(int i=0;i<=n;i++){
  	L[0][i] = 0;
  }
  
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  generateLCS(s1,s2,m,n,L);
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;    
  std::cerr<<elapsed_seconds.count()<<std::endl;
  getLCS(m,n,L,s1,s2);

  free(L);
  delete[] s1;
  delete[] s2;
  return 0;
}
void getLCS(int m, int n, int** L, char* s1, char*s2){ //Function to print out the LCS (Done sequentially though)
  int index = L[m][n];
  
  char lcs[index+1];
  lcs[index] = '\0'; 
  
  int i = m;
  int j = n;
  while (i > 0 && j > 0) {
      if (s1[i-1] == s2[j-1]) {
        lcs[index-1] = s1[i-1]; 
        i--;
        j--;
        index--;
      }
  
      else if (L[i-1][j] >= L[i][j-1])
        i--;
      else
        j--;
    }
  cout<<lcs<<endl;
}

void generateLCS(char* s1,char* s2, int m, int n ,int** L){
  int sum=1;
  int d=0;
  
  for(int i=1;i<m+n;i++){
    sum++;
    if(i<=m)
    	d = min(i,n);
    else
    	d = n-(i-m);// Calculate the length of the diagonals
    #pragma omp parallel for
    for(int k=0;k<d;k++){
    	int a = min(i,m) - k;
    	int b = sum - a;
    	if(s1[a-1] == s2[b-1])
    		L[a][b] = L[a-1][b-1]+1;
    	else
    		L[a][b] = max(L[a-1][b],L[a][b-1]);
    }
  }
}

void get_random_string(char* a,int length)  // Random string generator function.
{
    #pragma omp parallel for
    for(int i=0;i<length;i++){
        a[i]=alphabets[rand() % 26];
    }
}

