#include <syscall.h>

int
main()
{
    //Create("~/nachos/code/test/hola.txt");
    //int descriptor = Open("~/nachos/code/test/hola.txt");
    char buffer[50];
    Read(buffer, 10, ConsoleInput);
    Write(buffer, 10, ConsoleOutput);
    //Exit(0);          
    return 0;
}
