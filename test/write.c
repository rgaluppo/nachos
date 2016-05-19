#include <syscall.h>

int
main()
{
	//Create("/home/agraf/nachos/test/hola.txt");
	int descriptor = Open("/home/agraf/nachos/test/hola.txt");
	char buffer[] = "Chau!";
	Write(buffer, 5, descriptor);
	Exit(0);          
//	return 0;
}
