#include <unistd.h>
#include <iostream>
int main(){
	char hostname[126];
 	gethostname(hostname, sizeof hostname);
	std::cout<<hostname<<std::endl;
	return 0;
}
