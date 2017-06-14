#include "syscall.h"

int main( int argc, char ** argv){
    int i = 250;
	
	while(i-- > 0){
        Write("\n", 1, CONSOLE_OUTPUT);
	}

	Exit(0);
}
