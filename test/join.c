/* join.c
 *
 *	Simple program to test whether running a user program works.
 *	
 */

#include "syscall.h"

int
main()
{
    SpaceId newproc = Exec("../test/writefile", 0, '\0');
    Join(newproc);

    // will be execute when finish 'writetest' program.
    OpenFileId o = Open("../userprog/writetest.txt");
    Write("\n Goodbye !!!\n", 14, o);
    Close(o);

    return 0;
}
