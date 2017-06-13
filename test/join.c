/* join.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */

#include "syscall.h"

int
main()
{
    int dummy = 0;

    SpaceId newproc = Exec("../test/writetest");
    Join(newproc);

    // will be execute when finish 'writetest' program.
    for(int i=0; i < 10000; i++)
        dummy++; // do nothing...
    OpenFileId o = Open("writetest.txt");
    Write("\nGoodbye !!!\n", 13, o);
    Close(o);

    Exit(0);
}
