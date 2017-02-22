/* cat.c
 *	Simple program to test whether running a user program works.
 */

#include "syscall.h"

int
main(int argc, char** argv)
{
    char buffer[1];
    int ok = 1;

    OpenFileId descriptor = Open(argv[0]);  

    Write('\n', 1, ConsoleOutput);
    while(ok > 0){
        ok = Read(&buffer[0], 1, descriptor);
        Write(&buffer[0], 1, ConsoleOutput);
    }
    Write('\n', 1, ConsoleOutput);

    Close(descriptor);
    Exit(0);
}
