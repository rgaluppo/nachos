#include "syscall.h"

int main( int argc, char ** argv){

    //OpenFileId output = ConsoleOutput;
	/*if (argv[0] == ""){
		Write("Ingrese archivo", 20, output);	
	}*/
	
	int id = Open(argv[0]);
	const int size = 1;
	char buffer[size];


	//Read(&buffer, size, id);
	//Write(&buffer, size, ConsoleOutput);
	int i = 1;
	Write("shdfgb", 6, 1);
	while(i > 0)
	{
		i=Read(&buffer[0], 1, id); 
		
		Write(&buffer[0], 1, 1);
		
		
	}
	
	
	Exit(0);
	
	return 0;
}