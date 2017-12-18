#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

using namespace std;

#define MAX_NUMBER 100
int n,power;
double** H;

void rand_fill_values()
{
    for(int i = 0;i<n; i++)
    {
        for(int j = 0;j<n;j++)
        {
            H[i][j] = (rand()%MAX_NUMBER + 1)*1.0;
        }
    }
    return;
}

double calculate_value(int i, int j)
{
    double result = 0.0;
    if(i == 0 || i == n-1 || j == 0 || j == n-1)
    {
        if((i == 0 && j == 0))
        {
            result = 6*H[i][j] + H[i][j+1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(i == 0 && j == n-1)
        {
            result = 6*H[i][j] + H[i][j-1] + H[i+1][j] + H[i+1][j-1];
        }
        else if(i == n-1 && j == 0)
        {
            result = 6*H[i][j] + H[i][j+1] + H[i-1][j] + H[i-1][j+1];
        }
        else if(i == n-1 && j == n-1)
        {
            result = 6*H[i][j] + H[i-1][j] + H[i][j-1] + H[i-1][j-1];
        }
        else if(i == 0)
        {
            result = H[i][j-1] + 4*H[i][j] + H[i][j+1] + H[i+1][j-1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(i == n-1)
        {
            result = H[i-1][j-1] + H[i-1][j] + H[i-1][j+1] + H[i][j-1] + 4*H[i][j] + H[i][j+1];
        }
        else if(j == 0)
        {
            result = H[i-1][j] + H[i-1][j+1] + 4*H[i][j] + H[i][j+1] + H[i+1][j] + H[i+1][j+1];
        }
        else if(j == n-1)
        {
            result = H[i-1][j-1] + H[i-1][j] + H[i][j-1] + 4*H[i][j] + H[i+1][j-1] + H[i+1][j];
        }
    }
    else
    {
        result = H[i-1][j-1]+H[i-1][j]+H[i-1][j+1]+H[i][j-1]+H[i][j]+H[i][j+1]+H[i+1][j-1]+H[i+1][j]+H[i+1][j+1];
    }
    return result/9.0;
}

void cal_heat_equation(){
  double **tmp = new double*[n];

  for(int i = 0; i < n; i++)
    {
        tmp[i] = new double[n];
    }

    //int cur_power=0;

    while(power--){
        
        for(int i = 0; i<n;i++) 
        {
            for(int j = 0;j<n;j++)
            {
                tmp[i][j] = calculate_value(i,j);
            }
        }

        for(int i = 0; i<n;i++) 
        {
            for(int j = 0;j<n;j++)
            {
                H[i][j] = tmp[i][j];
            }
        }
        //cur_power++;
        //cout<<cur_power<<endl;
    }
    delete[] *tmp;
    return; 

}

int main (int argc, char* argv[]) {

    srand(time(NULL));
    if (argc < 3) {
        std::cerr<<"usage: mpirun "<<argv[0]<<" <n> <power>"<<std::endl;
        return -1;
    }

    
    n = atoi(argv[1]);
    power = atoi(argv[2]); 
    H=new double*[n];
    for(int i = 0; i < n; i++)
    {
        H[i] = new double[n];
    }

    rand_fill_values();
    
    std::chrono::time_point<std::chrono::system_clock> start_time=std::chrono::system_clock::now(); 
   
    cal_heat_equation();

    std::chrono::time_point<std::chrono::system_clock> end=std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start_time;
    std::cerr<<elapsed_seconds.count()<<std::endl;
    
    return 0;
}
