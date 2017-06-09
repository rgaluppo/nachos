/* create.c
 *	Simple program to test whether running a user program works.
 *	Just do a "syscall" that shuts down the OS.
 */

#include "syscall.h"

int
main()
{
    Create("test.txt");
    OpenFileId o = Open("test.txt");
    Close(o);
    Exit(0);
}
