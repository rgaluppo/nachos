/*testConsole.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */

#include "syscall.h"

int
main()
{
    int dummy = 0, size = 12;
	char buffer[size];

	Read(buffer, size, ConsoleInput);
    Write(buffer, size, ConsoleOutput);
    Exit(0);
}
