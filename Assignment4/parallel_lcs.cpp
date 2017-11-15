#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <iostream>
#include <chrono>

using namespace std;

int max(int a, int b) {
  return (a > b) ? a : b;
}


/* Returns length of LCS for X[0..m-1], Y[0..n-1] */
void lcs( char *X, char *Y, int m, int n ) {
  unsigned short int** L;
  int i, j;

  int size = 1;
  int inc = 1;
  int x, y;
  int init_x, init_y;
  int nthreads;

  L = (unsigned short int**) malloc(sizeof(unsigned short int*) * (m + 1));
  for(i = 0; i <= m; i++) {
    L[i] = (unsigned short int*) malloc(sizeof(unsigned short int) * (n + 1));
  }

  /* Following steps build L[m+1][n+1] in bottom up fashion. Note
  that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] */
  for(i = 0; i <= m; i++) {
    L[i][0] = 0;
  }

  for(i = 0; i <= n; i++) {
    L[0][i] = 0;
  }

  for (i = 1; i <= m + n - 1; i++) {
    if (i >= m) {
      x = m;
      y = 1 + i - m;
    } else {
      x = i;
      y = 1;
    }

    if (i == m || i == n) {
      inc--;
    }

    if (i == m && i == n) {
      inc--;
    }

    init_x = x;
    init_y = y;

    //printf("Num threads: %d\nThread id: %d\n", nthreads, tid);
    #pragma omp parallel for private(x, y)
    for (j = 0; j < size; j++) {
      x = init_x - j;
      y = init_y + j;

      if (X[x-1] == Y[y-1]) {
       L[x][y] = L[x-1][y-1] + 1;
      } else {
        L[x][y] = max(L[x-1][y], L[x][y-1]);
      }
    }
   
    size += inc;
  }

  
  //printf("For Time: %f\n", time);

  // Following code is used to print LCS
  int index = L[m][n];

  // Create a character array to store the lcs string
  char lcs[index+1];
  lcs[index] = '\0'; // Set the terminating character

  // Start from the right-most-bottom-most corner and
  // one by one store characters in lcs[]
  i = m;
  j = n;
  while (i > 0 && j > 0) {
    // If current character in X[] and Y are same, then
    // current character is part of LCS
    if (X[i-1] == Y[j-1]) {
      lcs[index-1] = X[i-1]; // Put current character in result
      i--;
      j--;
      index--;     // reduce values of i, j and index
    }

    // If not same, then find the larger of two and
    // go in the direction of larger value
    // If equal go left (as statement rule says so)
    else if (L[i-1][j] >= L[i][j-1])
      i--;
    else
      j--;
  }
  //printf("While Time: %f\n", time);

  /* Print the lcs */
  std::cout<<L[m][n]<<lcs<<std::endl;

  for(i = 0; i <= m; i++) {
    free(L[i]);
  }
  free(L);
}

int main(int argc, char *argv[]) {

  char firstString[] = "AGGTAB";
  char secondString[] = "GXTXABB";
  int firstStringSize = strlen(firstString);
  int secondStringSize = strlen(secondString);
  /* Calling the function that compute the LCS (In the statement says: "In order for the
   * solution be unique you should follow these rules: associate the first string with the
   * rows (and the second with the columns); when moving backwards in the matrix to determine
   * the subsequence, in case up and left have the same vaue always move left." */
   omp_set_num_threads(16);
   chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
   lcs(secondString, firstString, secondStringSize, firstStringSize);
   chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
   chrono::duration<double> elapsed_seconds = end-start; 
   cerr<<elapsed_seconds.count()<<endl;
	return EXIT_SUCCESS;
}
