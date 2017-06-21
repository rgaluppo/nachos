/* filetest.c
 *
 *	Simple program to test whether running a user program works.
 *
 */
#include "syscall.h"

int
main()
{
    char buffer[100];

    OpenFileId descriptor = Open("../userprog/readtest.txt");
    Read(buffer, 100, descriptor);
    Close(descriptor);
    Write(buffer, 100, CONSOLE_OUTPUT);

    return 0;
}
