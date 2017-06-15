/* join.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */

#include "syscall.h"

int
main()
{
    SpaceId newproc = Exec("../test/writetest", 0, '\0');
    Join(newproc);

    // will be execute when finish 'writetest' program.
    OpenFileId o = Open("writetest.txt");
    Write("\nGoodbye !!!\n", 13, o);
    Close(o);

    Exit(0);
}
