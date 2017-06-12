/*testConsole.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */
#include "syscall.h"

int
main()
{
    int dummy = 0, size = 256;
	char buffer[size];

	Read(buffer, size, CONSOLE_INPUT);
    Write(buffer, size, CONSOLE_OUTPUT);

    Exit(0);
}
