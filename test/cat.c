#include "syscall.h"

int main( int argc, char ** argv){
	int id = Open(argv[0]);
	const int size = 1;
	char buffer[size];
	int i = 1;

	Write("\n", 1, ConsoleOutput);
	while(i > 0){
		i=Read(&buffer[0], 1, id); 
		Write(&buffer[0], 1, ConsoleOutput);		
	}
	Write("\n", 1, ConsoleOutput);
	Close(id);
	
	Exit(0);
	return 0;
}
