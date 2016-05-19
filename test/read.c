#include <syscall.h>

int
main()
{
	//Create("/home/agraf/nachos/test/hola.txt");
	int descriptor = Open("/home/agraf/nachos/test/hola.txt");
	char buffer[]="hola\n";
	Read(buffer,6, descriptor);
	Write(buffer, 6, ConsoleOutput);
	Exit(0);          
//	return 0;
}
