/* cat.c
 *	Simple program to test whether running a user program works.
 */

#include "syscall.h"

int
main(int argc, char** argv)
{
    char c;

    if(argc == 1) {

        OpenFileId o = Open(argv[0]);  
        do {
            Read(&c, 1, o);
            Write(&c, 1, ConsoleOutput);
        } while(c != '\0');

        Close(o);
    }
    Exit(0);
}
