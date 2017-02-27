#include "syscall.h"

int main( int argc, char ** argv){
	int i = 50;
	
	while(i-- > 0){
		Write("\n", 1, ConsoleOutput);
	}

	Exit(0);
}
