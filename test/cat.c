/* cat.c
 *	Simple program to test whether running a user program works.
 */

#include "syscall.h"

int
main(int argc, char** argv)
{
    char buffer[1];
    int ok = 1;

    if(argc == 1) {

        Write("Entrando al CAT: argv= ", 60, ConsoleOutput);
        Write(argv[0], 60, ConsoleOutput);
        Write("\n", 60, ConsoleOutput);
        OpenFileId o = Open(argv[0]);  
        Write("descriptor o= ", 60, ConsoleOutput);
        Write(o, 1, ConsoleOutput);
        Write("\n", 60, ConsoleOutput);
        do {
            ok = Read(&buffer[0], 1, o);
            Write(&buffer[0], 1, ConsoleOutput);
        } while(ok > 0);

        Close(o);
    }
    Exit(0);
}
