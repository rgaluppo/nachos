/* filetest.c
 *
 *	Simple program to test whether running a user program works.
 *
 */
#include "syscall.h"

int
main()
{
    Create("../userprog/writetest.txt");
    OpenFileId descriptor = Open("../userprog/writetest.txt");
    Write("This is a test for write file syscall:\n Hello world! Put this on file 'writetest.txt'\n", 86, descriptor);
    Close(descriptor);

    return 0;
}
