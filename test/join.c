/* join.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */

#include "syscall.h"

int
main()
{
    int dummy = 0, i;
    SpaceId newproc = Exec("../test/filetest");
    Join(newproc);
    for(i=0; i < 10000; i++)
        dummy++;
    OpenFileId o = Open("test.txt");  
    Write("Goodbye !!!\n", 12, o);
    Close(o);
    Exit(0);
}
