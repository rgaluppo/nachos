#include "syscall.h"

int main( int argc, char ** argv){
	char *origen = argv[0];
	char *destino = argv[1];
	char buffer[1];
	int i = 1;
	OpenFileId descOr, descDest;

	
	Create(destino);
	descOr = Open(origen);
	descDest = Open(destino);
    Write("\n", 1, CONSOLE_OUTPUT);
	while(i > 0){
		i=Read(&buffer[0], 1, descOr); 
		Write(&buffer[0], 1, descDest);		
	}

	Close(descOr);
	Close(descDest);
	
	Exit(0);
	return 0;
}
