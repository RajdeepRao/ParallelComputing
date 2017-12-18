#include <iostream>
#include <chrono>


__global__ void polynomial_expansion (float* poly, int degree, int n, float* array) {
  float out = 0.;
  float xdegree = 1.;
  
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n){
    float x = array[i];
    for (int k=0; k<=degree; ++k) {
      out += xdegree*poly[k];
      xdegree *= x;
    }
    array[i] = out;
  }


}

int main (int argc, char* argv[]) {
  //TODO: add usage
  
  if (argc < 3) {
     std::cerr<<"usage: "<<argv[0]<<" n degree"<<std::endl;
     return -1;
  }

  int n = atoi(argv[1]); 
  int degree = atoi(argv[2]);
  int nbiter = 1;

  float* array = new float[n];
  float* poly = new float[degree+1];
  float *xPointer, *coeffPointer; 
  
  for (int i=0; i<n; ++i)
    array[i] = 1.0;

  for (int i=0; i<degree+1; ++i)
    poly[i] = 1.0;

  
  cudaMalloc(&xPointer, n*sizeof(float)); 
  cudaMalloc(&coeffPointer, (degree+1)*sizeof(float)); 
 
  cudaMemcpy(xPointer, array, n*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(coeffPointer, poly, (degree+1)*sizeof(float), cudaMemcpyHostToDevice); 
  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();
  
  
  for (int iter = 0; iter<nbiter; ++iter){
     polynomial_expansion<<<(n+255)/256, 256>>>(coeffPointer, degree, n, xPointer);
  }
  cudaDeviceSynchronize();
  
  cudaError_t err = cudaGetLastError();
  if (err != cudaSuccess) 
    printf("Error: %s\n", cudaGetErrorString(err)); 

  cudaMemcpy(array, xPointer, n*sizeof(float), cudaMemcpyDeviceToHost);
  
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin)/nbiter;

  std::cerr<<array[0]<<std::endl;
  std::cout<<n<<" "<<degree<<" "<<totaltime.count()<<std::endl;

  delete[] array;
  delete[] poly;
  cudaFree(xPointer); 
  cudaFree(coeffPointer);
  return 0;
}
